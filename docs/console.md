# Console driver

## Overview

This layer owns the **logical model** of what a terminal contains:
scrollback history, which portion of it is currently visible, and where
the write cursor is, entirely independent of how any of that gets turned
into pixels or VGA cells. It sits directly on top of the backend vtable
(`display_d`, see [display.md](display.md)) and is the only code allowed
to call into it.

```
shell_execute() / print_prompt()   (shell.c: command table dispatch,
      │                             see tty.md)
      ▼
editor_putchar() / editor_backspace() / editor_delete()
      │                            (tty.c: line editor, see tty.md)
      ▼
screen_putchar(c)                 (public entry point, everything funnels
      │                            through here: printf's out_target_t,
      │                            keyboard echo, the line editor)
      ▼
console.c                         (this document: scrollback buffer,
      │                            view offset, cursor math, multi-screen)
      ▼
t_display_driver (display_d)      (display.md: dumb physical drawing)
```

The backend never sees a logical line number. This layer never touches a
pixel or an I/O port directly.

**Naming note:** this entry point was called `kputchar` in an earlier
version of the kernel. It is now `screen_putchar`; the rest of this
document uses the current name.

## Why this layer exists

An earlier design let the display backends manage their own scroll and
clear logic directly on physical memory (`memmove` on VGA text memory or
framebuffer pixels). This had two compounding problems:

1. **Scrolling was destructive.** The line pushed off the top of the
   screen was gone, physically overwritten, with no way to recover it.
   Any future shell needs scrollback (`PgUp`) and the ability to review
   output without it vanishing the moment the screen fills up.
2. **A single "cursor" was doing three unrelated jobs**: tracking where
   the kernel writes next, tracking what the user is currently looking at,
   and bounding what a line editor is allowed to erase. As long as these
   shared one variable, there was no way to let a user scroll back through
   history without either losing their read position on the next `printf`,
   or corrupting where the next character gets written.

The fix is a strict separation into three roles:

| Role | Owner | Field(s) | Changes when |
|------|-------|----------|--------------|
| Write cursor | `screen_putchar` (console.c) | `head`, `col` | A character or `\n` is written. `head` is monotonic, never decremented. |
| View | user / display logic | `view_offset` | Manual scroll (`PgUp`/`PgDown`/`↑`/`↓`/`Home`), or an explicit snap-to-bottom. |
| Input boundary | line editor (`t_line_editor`, tty.c) | `input_boundary_col`, `edit_pos`, `len` | A prompt is printed (`editor_start`); bounds how far backspace/delete/left-arrow can go. See [tty.md](tty.md). |

`view_offset` is **not** a delta from `head`, it is an absolute logical
line number, exactly like `head`. Storing it as an offset would recouple
the two variables and defeat the entire point of the split: the user
could no longer scroll back through history while new output keeps
arriving at the bottom without their read position jumping around.

### `col` is now a shared-ownership field, by contract

The table above still lists `col` under the write cursor, but it is no
longer written to exclusively by `screen_putchar`. The line editor
(tty.c) needs to reposition the cursor and redraw arbitrary characters
on the *current, unsubmitted* line without going through the normal
write path (which would advance `col`, trigger `screen_newline` on
overflow, and disturb characters typed after the edit point). Two extra
entry points exist specifically for this:

- **`move_cursor_to(col)`** sets `s->col` directly and calls
  `display_d->cursor_update()`. It writes no character and does not
  touch `head`. It is how the line editor moves the visual cursor after
  an insert, delete, or arrow-key press.
- **`overwrite_at(col, c)`** writes a single character into `text_buf`/
  `color_buf` at `col` on the *current head line* and redraws it on
  screen if visible, exactly like `screen_putchar` does, but without
  advancing `col` and without ever calling `screen_newline`. It is how
  the line editor redraws the tail of the input line after a character
  is inserted or removed, one column at a time.

Both functions assume the caller stays within the current line
(`s->head`) and within `g_screen.total_cols`; neither of them wraps or
scrolls. That assumption is currently enforced by the line editor, not
by `console.c` itself, see [tty.md](tty.md) for the actual bound checks
and a caveat about the input line potentially exceeding one screen row.

`get_current_col()` and `set_term_color()` are simple accessors used the
same way: read the write cursor's column, or change the active color for
future writes on the current screen.

## Core data structure: `t_screen_data`

One instance per virtual screen (`g_screens[MAX_SCREENS]`, `MAX_SCREENS = 4`).
This, not `g_screen`, is where a virtual screen's actual content and
cursor state live, whether or not that screen is currently displayed.

| Field                  | Meaning |
|-------------------------|---------|
| `text_buf[SCREEN_COLS * SCROLLBACK_LINES]` | Character grid, circular |
| `color_buf[SCREEN_COLS * SCROLLBACK_LINES]` | Packed color per cell, circular |
| `col`   | Write cursor column on the `head` line |
| `head`  | Logical line index currently being written to. Monotonically increasing, never decremented, never bounded to screen height. |
| `view_offset` | Logical line index rendered at the top of the visible screen. Independent of `head`; stays fixed unless changed by manual scroll or explicit snap. |
| `color` | Packed fg/bg for this screen |

### Ring buffer indexing

`head`/`view_offset` are logical line numbers that grow without bound.
The physical row inside `text_buf`/`color_buf` is always:

```c
u32_t phys_line = logical_line % SCROLLBACK_LINES;
```

`SCROLLBACK_LINES` should be a power of two so the modulo can become a
bitmask (`& (SCROLLBACK_LINES - 1)`), a cheap win on hardware with no
FPU-assisted division.

### Memory budget

Storing history as character cells rather than pixels keeps this cheap
regardless of framebuffer resolution. Worked example at 1620×1080 with an
8×16 font (202 cols × 67 visible rows):

- One line: `202 cols × 2 bytes/cell ≈ 404 bytes`
- 1000 lines of scrollback: `≈ 395 KiB` per virtual screen
- Four virtual screens: `≈ 1.6 MB` total

Compare to a single front+back pixel buffer pair at the same resolution
32bpp: `≈ 13.3 MB`. The entire scrollback for all four virtual screens
costs less than one pixel buffer. `SCROLLBACK_LINES` should still be
sized against the real available RAM (Multiboot `mmap`) rather than
picked arbitrarily, since this is all static BSS allocation, no heap
exists yet at this stage of the kernel.

## Visibility helpers

```c
inline bool line_visible(t_screen_data *s, u32_t line)
{
	return line >= s->view_offset && line < s->view_offset + g_screen.total_rows;
}

inline bool pinned_to_bottom(t_screen_data *s)
{
	return s->view_offset + g_screen.total_rows - 1 == s->head;
}
```

Every write checks `line_visible()` before touching the backend at all,
if the user has scrolled away from the bottom, new output still updates
`text_buf`/`color_buf` (so it's there when they scroll back down) but
never calls into `display_d`, since nothing about the visible screen
actually changed.

## Writing a character: `screen_putchar`

```c
int screen_putchar(char c)
{
	t_screen_data *s = &g_screens[current_screen];

	if (c == '\n') { screen_newline(s); display_d->cursor_update(); return 1; }
	if (c == '\t') { /* expand to next multiple of 8 via repeated screen_putchar(' ') */ }

	// always write into the logical buffer, regardless of visibility
	u32_t idx = (s->head % SCROLLBACK_LINES) * SCREEN_COLS + s->col;
	s->text_buf[idx] = c;
	s->color_buf[idx] = s->color;

	// only touch the physical layer if this line is actually on screen
	if (line_visible(s, s->head)) {
		display_d->putchar_at(c, s->color, s->col, s->head - s->view_offset);
		display_d->flush_partial(s->col * font_info.width,
			(s->head - s->view_offset) * font_info.height,
			font_info.width, font_info.height);
	}

	if (++s->col >= g_screen.total_cols)
		screen_newline(s);
	display_d->cursor_update();
	return 1;
}
```

The cursor's **physical** row passed to the backend is always
`head - view_offset`, computed on demand, never stored as an absolute
row anywhere. Storing it separately (an earlier version had a `row` field
on `g_screen`) is what caused the cursor to silently desync after a
screen switch or after several newlines without a full redraw in between.

## Newline and auto-scroll: `screen_newline`

```c
static void screen_newline(t_screen_data *s)
{
	bool was_pinned = pinned_to_bottom(s);
	u32_t old_offset = s->view_offset;

	s->head++;
	if (s->head - s->view_offset >= SCROLLBACK_LINES)
		s->view_offset = s->head - SCROLLBACK_LINES + 1; // capacity clamp
	clear_line(s, s->head);
	s->col = 0;

	if (was_pinned) {
		display_d->scroll();
		s->view_offset++;
		g_screen.cursor_col = g_screen.cursor_row = -1; // cache invalidated
	}
	else if (s->view_offset != old_offset)
		screen_redraw(); // capacity clamp moved the view; resync the display
}
```

Two things worth calling out explicitly, both were real bugs during
development:

- **The `was_pinned` check must be captured before mutating `head`.**
  Evaluating "am I pinned to the bottom" after the increment always comes
  out true, since the formula becomes self-fulfilling once applied.
- **The capacity clamp branch is easy to miss.** If a user has scrolled
  back into history and enough new lines arrive to exceed
  `SCROLLBACK_LINES`, `view_offset` is forced forward so it never points
  at an already-overwritten ring slot, but if nothing then calls
  `screen_redraw()`, the physical screen keeps showing stale content while
  the logical view has silently moved. This is an unavoidable trade-off
  of a bounded scrollback (a real terminal's history limit has the same
  effect), but the display must stay in sync with it.

## Manual scrolling

`screen_scroll(int delta)` clamps the requested movement between how far
back scrollback actually extends and how far down the live edge is, then
picks between two rendering strategies:

- **Large jump** (`abs(delta) >= total_rows`): cheaper to throw away the
  entire visible screen and call `screen_redraw()` than to shift pixels.
- **Small jump**: `partial_shift()`, physically `memmove`s the existing
  pixels/VGA cells by the delta, then only rasterizes the newly-exposed
  rows at the edge, with exactly one `flush_screen()` at the end. This
  avoids redrawing rows that didn't actually change.

`screen_snap()` forces `view_offset` back to the live bottom (`head` minus
one screen height), used when the user explicitly asks to jump back to
"now" rather than scrolling incrementally.

### ⚠ Regression: the `'\0'` sentinel is back

An earlier version of this module used `'\0'` to mean "never written"
and skipped drawing those cells in both `screen_redraw()` and
`partial_shift()`. That was identified as a bug and fixed: `clear_line()`
was changed to write real `' '` characters, and the `'\0'` skip was
removed from both redraw paths, because `partial_shift()` only
`memmove`s existing pixels and never clears anything, so a skipped cell
leaves stale pixels visible whenever a scroll exposes a region mixing
written and never-written cells (this was the diagnosed cause of a boot
logo's background ghosting on first scroll).

**The current code has reverted to the old behavior**: `clear_line()`
fills with `'\0'` again, and both `screen_redraw()` and `partial_shift()`
still contain `if (text_buf[idx + c] == '\0') continue;`. On top of that,
`screen_clear(full=true)` fills with `' '` directly, so the two clearing
paths in this file no longer even agree with each other on what an empty
cell looks like.

This needs a decision: either reinstate the `' '`-everywhere invariant
(remove the `'\0'` skip from both redraw paths, make `clear_line` write
`' '`), or, if there's a new reason `'\0'` is needed, that reason should
replace this note rather than sit silently contradicted by it. Flagging
here rather than silently documenting the regression as if it were
intended.

## Clearing: `clear` vs `^L` (design intent)

Both must be **non-destructive**, clearing the visible screen should
push its content into scrollback, never erase history, exactly like a
real terminal's `clear`/`tput clear`. The mechanism already exists:
reusing `screen_newline()` in a loop for `total_rows` iterations pushes
the current screen up into history with zero special-cased memory
handling.

The two differ only in what happens *after* the screen is pushed:

- **`clear` (shell command)**, runs after Enter was pressed, the input
  line has already been submitted. Nothing else needed; the shell's
  normal loop prints the next prompt.
- **`^L`**, intercepted mid-edit, the current input line was never
  submitted. After clearing, the prompt and whatever the user had
  already typed must be explicitly redrawn from the line editor's own
  buffer. The line editor described in [tty.md](tty.md) now exists and
  keeps its buffer independent of screen content for exactly this
  reason, but nothing currently calls it in response to `^L`, the key
  itself isn't wired up yet. Still future work.

**Current status:** `screen_clear(bool full)` as implemented still does
a direct `memset` (full clear) or a `clear_line()` loop over the visible
rows (partial clear) rather than reusing `screen_newline()`, so both
paths remain destructive: history is not preserved, `full` additionally
resets `head`/`view_offset`/`col` to `0`, which throws away scrollback
entirely rather than pushing it further back in the ring buffer. Flagged
in [Future work](#future-work).

## Multi-screen management

**`g_screen` (singular, in display.md) and `g_screens[]` (plural, here)
answer different questions and must not be conflated.** `g_screen`
describes the one physical output device that exists. `g_screens[i]` is
the logical content of virtual terminal `i`, and every one of the four,
including whichever is currently displayed, behaves identically: `head`
is not special to "background" screens, it's simply how every screen
tracks its own write position, visible or not.

`current_screen` is the only thing that ties the two together, it's the
index into `g_screens[]` that `screen_putchar` writes into and that
`screen_redraw()` reads from.

`screen_switch(new_id)`:

1. Update `current_screen`.
2. `screen_redraw()`, clears the physical/back buffer and rasterizes
   `g_screens[new_id]`'s visible window (`view_offset` to
   `view_offset + total_rows`) cell by cell, then one `flush_screen()`.

**Pitfall already hit:** the physical clear primitives
(`clear_physical_fb`/`clear_physical_vga`, see display.md) must be purely
physical and must never reset `g_screens[current_screen].col` as a side
effect. An earlier version did exactly that, which meant every screen
switch silently reset the destination screen's column to `0` even though
its row/content were restored correctly, the saved column was real, it
was just being overwritten by a function that had no business touching
logical state.

## Future work

- Resolve the `'\0'`/`' '` sentinel regression above.
- Make `screen_clear()` non-destructive by reusing `screen_newline()`,
  per the design intent described above.
- Wire `^L` to the line editor's redraw path (tty.md).

See [TODO.md](TODO.md) for the rest of the current list.
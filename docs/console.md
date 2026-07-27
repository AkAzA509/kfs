# Console driver

## Overview

This layer owns the **logical model** of what a terminal contains:
scrollback history, which portion of it is currently visible, and where
the write cursor is, entirely independent of how any of that gets turned
into pixels or VGA cells. It sits directly on top of the backend vtable
(`display_d`, see [display.md](display.md)) and is the only code allowed
to call into it.

```
kputchar(c)                       (public entry point, everything funnels
      │                            through here: printf, keyboard echo)
      ▼
console.c                         (this document: scrollback buffer,
      │                            view offset, cursor math, multi-screen)
      ▼
t_display_driver (display_d)      (display.md: dumb physical drawing)
```

The backend never sees a logical line number. This layer never touches a
pixel or an I/O port directly.

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
   and (eventually) bounding what a line editor is allowed to erase. As
   long as these shared one variable, there was no way to let a user
   scroll back through history without either losing their read position
   on the next `printf`, or corrupting where the next character gets
   written.

The fix is a strict separation into three independent roles:

| Role | Owner | Field(s) | Changes when |
|------|-------|----------|--------------|
| Write cursor | kernel (`kputchar`) | `head`, `col` | A character or `\n` is written. Monotonic, never decremented. |
| View | user / display logic | `view_offset` | Manual scroll (`PgUp`/`PgDown`/`↑`/`↓`/`Home`), or an explicit snap-to-bottom. |
| Input boundary *(planned, shell bonus)* | line editor | not yet implemented | A prompt is printed; bounds how far backspace/left-arrow can go. |

`view_offset` is **not** a delta from `head`, it is an absolute logical
line number, exactly like `head`. Storing it as an offset would recouple
the two variables and defeat the entire point of the split: the user
could no longer scroll back through history while new output keeps
arriving at the bottom without their read position jumping around.

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
static inline bool line_visible(t_screen_data *s, u32_t line)
{
	return line >= s->view_offset && line < s->view_offset + g_screen.total_rows;
}

static inline bool pinned_to_bottom(t_screen_data *s)
{
	return s->view_offset + g_screen.total_rows - 1 == s->head;
}
```

Every write checks `line_visible()` before touching the backend at all,
if the user has scrolled away from the bottom, new output still updates
`text_buf`/`color_buf` (so it's there when they scroll back down) but
never calls into `display_d`, since nothing about the visible screen
actually changed.

## Writing a character: `kputchar`

```c
int kputchar(char c)
{
	t_screen_data *s = &g_screens[current_screen];

	if (c == '\n') { screen_newline(s); ... return 1; }
	if (c == '\t') { /* expand via repeated kputchar(' ') */ }

	// always write into the logical buffer, regardless of visibility
	u32_t idx = (s->head % SCROLLBACK_LINES) * SCREEN_COLS + s->col;
	s->text_buf[idx] = c;
	s->color_buf[idx] = s->color;

	// only touch the physical layer if this line is actually on screen
	if (line_visible(s, s->head)) {
		display_d->putchar_at(c, s->color, s->col, s->head - s->view_offset);
		display_d->flush_partial(...);
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

`clear_line` writes real space characters, not a `'\0'` sentinel, see
[Empty cells must be real cells](#empty-cells-must-be-real-cells) for why
that distinction matters.

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

### Empty cells must be real cells

Both `screen_redraw()` and `partial_shift()` must treat "never written"
cells identically. An earlier version used `'\0'` as a sentinel and
skipped drawing those cells, which worked for `screen_redraw()` (which
clears the physical buffer first, so skipped cells show a correct blank
background) but broke `partial_shift()`: that function only `memmove`s
existing pixels and never clears anything, so a skipped cell leaves
whatever stale pixels were physically there before the shift, visible as
ghosted/duplicated content with the wrong color whenever scrolling exposed
a region that mixed written and never-written cells.

The fix: initialize every cell to a real `' '` with the active color at
boot (and in `clear_line()`), and remove the `'\0'` skip entirely, always
draw every cell unconditionally. This makes the two redraw paths strictly
equivalent regardless of which one happens to fire.

This also means: **anything that should survive a scroll or a screen
switch must go through `kputchar`**, writing into `text_buf`/`color_buf`
like any other character. Content drawn by bypassing this (direct pixel
writes) has no representation in the logical buffer, so any later
`screen_redraw()`/`partial_shift()` will paint right over it with what it
believes is the correct (blank) content, this was diagnosed as the cause
of a boot logo with a temporary background color getting overwritten by
plain background on the first scroll.

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
- **`^L` (planned, once a line editor exists)**, intercepted mid-edit,
  the current input line was never submitted. After clearing, the prompt
  and whatever the user had already typed must be explicitly redrawn from
  the line editor's own buffer (which must be kept independent of screen
  content specifically so it can redraw itself without reading the
  screen back).

**Current status:** `screen_clear(bool full)` as implemented still does a
direct `memset` on `text_buf`/`color_buf` (destructive) rather than
reusing `screen_newline()`. Flagged in [Future work](#future-work) to
bring in line with the design above.

## Multi-screen management

**`g_screen` (singular, in display.md) and `g_screens[]` (plural, here)
answer different questions and must not be conflated.** `g_screen`
describes the one physical output device that exists. `g_screens[i]` is
the logical content of virtual terminal `i`, and every one of the four,
including whichever is currently displayed, behaves identically: `head`
is not special to "background" screens, it's simply how every screen
tracks its own write position, visible or not.

`current_screen` is the only thing that ties the two together, it's the
index into `g_screens[]` that `kputchar` writes into and that
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

See [TODO.md](TODO.md) for the current list of planned improvements to this
module.
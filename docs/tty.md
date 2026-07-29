# TTY: line editing and command dispatch

## Overview

This layer sits directly above `console.c` (see [console.md](console.md))
and is what turns raw keyboard input into an editable input line and,
once the user presses Enter, a dispatched shell command. It owns no
screen state of its own beyond its own edit buffer: everything it draws
goes back through `console.c`'s public API (`screen_putchar`,
`overwrite_at`, `move_cursor_to`, `get_current_col`).

```
keyboard IRQ handler
      │
editor_putchar(c) / editor_backspace() / editor_delete() / move_cursor(dir)
      │                                (tty.c: this document, t_line_editor)
      ▼
shell_execute(buffer, len)            (shell.c: command table dispatch)
      │
screen_putchar / overwrite_at / move_cursor_to / get_current_col
      ▼
console.c                             (console.md: scrollback, view, cursor)
```

This corresponds to the "Input boundary" role that `console.md` flagged
as planned but not yet implemented: it now exists here, as `t_line_editor`.

## Why a separate buffer instead of reading the screen back

The line editor keeps its own `buffer`/`len`/`edit_pos`, entirely
independent of `text_buf` in `console.c`. Two reasons:

1. **`console.c` has no concept of "this is still being edited."** Once
   a character is written via `overwrite_at`, it's just a screen cell
   like any other, indistinguishable from something `printf` wrote. If
   the editor needed to reconstruct the current input line by reading
   cells back out of `text_buf`, it would have no way to know where the
   prompt ends and the input begins without also tracking that boundary,
   which is exactly what `input_boundary_col` is for.
2. **Redraw-without-readback.** A planned `^L` handler (see
   [console.md](console.md#clearing-clear-vs-l-design-intent)) needs to
   reprint the prompt and whatever was typed so far after the screen is
   cleared. That's only possible if the input text lives somewhere other
   than the screen it just erased.

## Core data structure: `t_line_editor`

Single global instance, `g_line_editor`, one virtual screen's worth of
in-progress input at a time (there is currently no per-virtual-screen
line editor, unlike `t_screen_data`; switching screens mid-edit is not
accounted for, see [Known limitations](#known-limitations)).

| Field | Meaning |
|---|---|
| `buffer[MAX_LINE]` | Raw input text for the current, unsubmitted line |
| `len` | Number of valid characters currently in `buffer` |
| `edit_pos` | Cursor position *within the input*, `0..len` |
| `input_boundary_col` | Screen column where the prompt ends and input starts, captured once per line by `editor_start()` |

`input_boundary_col` is what makes this the "input boundary" role
described in console.md: `edit_pos` is relative to it, and every screen
write the editor performs is offset by it
(`input_boundary_col + edit_pos`, `input_boundary_col + i`, etc.),
converting an editor-relative position into an absolute column that
`console.c` understands.

## Starting a line: `editor_start`

Prints the prompt (`print_prompt()`, from `shell.c`), resets `len` and
`edit_pos` to `0`, and captures the current write cursor's column
(`get_current_col()`) as `input_boundary_col`. This is what anchors the
whole line: everything the editor does afterwards is expressed relative
to this captured column, never relative to `0` or to an absolute screen
column computed some other way.

## Typing a character: `editor_putchar`

- On `'\n'`: echoes the newline via `screen_putchar('\n')` (this is the
  one path that *does* go through the normal write cursor, since a
  submitted line is exactly like any other kernel output), then calls
  `shell_execute()` with the buffer and length, then `editor_start()`
  again for the next prompt.
- Otherwise: if inserting before the end of the current input
  (`edit_pos < len`), shifts the tail of `buffer` right by one with
  `memmove` to make room, then writes the new character at `edit_pos`
  and increments both `len` and `edit_pos`.
- Redraw: rewrites every character from the insertion point to the new
  end of the line via `overwrite_at`, one call per column, then calls
  `move_cursor_to` once to place the visual cursor after the inserted
  character. This is the same "many small physical writes, no early
  flush per cell" pattern the framebuffer backend uses, except here the
  flush-per-cell already happens inside `overwrite_at`
  (`display_d->flush_partial` per character, see console.md); there's no
  batching across the redrawn tail. Fine at interactive typing speed and
  `MAX_LINE = 100`, but worth knowing if `MAX_LINE` ever grows much
  larger and inserts near the start of a long line start feeling laggy.

## Deleting a character: `editor_backspace` / `editor_delete`

Both funnel into the same `editor_remove_char(le, pos)`:

- `memmove`s the tail of `buffer` left by one, decrements `len`.
- Rewrites every character from `pos` onward via `overwrite_at`
  (the string is now one shorter, so every position shifts left by one),
  then writes one trailing `' '` at the new end of the line to erase
  what used to be the last character on screen. This trailing-space
  erase is necessary precisely because `overwrite_at` never advances
  `col` or clears anything on its own; without it the old last character
  would remain on screen, duplicated.

`editor_backspace` additionally decrements `edit_pos` first (deleting
the character *before* the cursor) and bails out at `edit_pos == 0`.
`editor_delete` deletes *at* the cursor and bails out at
`edit_pos >= len`, leaving `edit_pos` untouched. Both then call
`move_cursor_to(input_boundary_col + edit_pos)` to resync the visual
cursor.

## Moving the cursor: `move_cursor`

Takes a signed direction (conventionally `+1`/`-1`; the doc comment on
this function warns that any other magnitude will move the cursor by
that many columns instead, so `move_cursor_to` should be preferred for
anything other than a single arrow-key step). Refuses to move past
either edge of the input (`edit_pos == 0` going left, or the bound check
below going right), and does nothing for `direction == 0`.

**Worth double-checking:** the right-edge guard compares
`edit_pos == g_screen.total_cols` (the *screen's* width), not
`edit_pos == len` (the *input's* length). Those only coincide if the
input line happens to be exactly as long as the screen is wide. As
written, this looks like it should bound movement against `len`, the
same way `editor_delete` does; left as-is here since it's a code
question rather than a documentation one, but flagging it since it's
the kind of thing that only shows up once someone types a line short
enough to hit the wrong edge first.

## Command dispatch: `shell.c`

Deliberately simple, and orthogonal to everything above: `shell_execute`
receives a finished line (`buffer`, `len`) from `editor_putchar` on
`'\n'`, and does an exact-match lookup against a parallel `cmd_table`
(names) / `cmd_handlers` (function pointers) pair, dispatching to the
matching handler with `strncmp` + an explicit `'\0'` check to reject
prefix matches (`"hal"` must not match `"halt"`).

Commands currently table-driven: `reboot`, `halt`, `print logo`,
`print stack`, `shutdown`. Unknown input is silently ignored (the error
path is commented out); this is presumably deliberate for now rather
than an oversight, but is worth a second look before this ships, since a
silent no-op on typos is a rougher UX than the rest of this layer.

`print_prompt()` also lives here rather than in tty.c proper, since the
prompt string itself (`"Tekos/root > "`) is shell-identity, not
line-editing logic. `editor_start()` (tty.c) calls it as a pure "print
this text" step and otherwise knows nothing about what it says.

## Known limitations

- **One line editor, not one per virtual screen.** `t_screen_data` is
  per-screen (`g_screens[MAX_SCREENS]`), but `g_line_editor` is a single
  global. Switching virtual screens mid-edit (if that's ever exposed to
  the user) would leave the editor's `input_boundary_col` pointing at a
  column on whatever screen was active when `editor_start()` last ran,
  not the newly active one.
- **No line-wrap handling.** `MAX_LINE` is `100`, independent of
  `g_screen.total_cols`. If a screen is narrower than 100 columns, an
  input line can exceed the visible row without the editor or
  `overwrite_at`/`move_cursor_to` doing anything about wrapping or
  scrolling; both of those functions operate on the current line only
  and have no fallback if a column falls outside `0..total_cols`.
- **`^L` isn't wired up yet.** The buffer/boundary split exists
  specifically to support it (see [Why a separate buffer](#why-a-separate-buffer-instead-of-reading-the-screen-back)),
  but no keyboard handler currently calls into a clear-and-redraw path.

## Future work

- Decide whether `move_cursor`'s right-edge bound should be `len`
  instead of `g_screen.total_cols`.
- Wire `^L` to clear the screen (via `console.c`) and replay
  `print_prompt()` + `g_line_editor.buffer` through `overwrite_at`.
- Decide whether unknown commands in `shell_execute` should report an
  error rather than silently doing nothing.
- Consider whether line wrapping is worth supporting before `MAX_LINE`
  or screen widths change enough to make it likely to matter in practice.

See [TODO.md](TODO.md) for the current list of planned improvements.
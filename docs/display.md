# Display Subsystem

## Overview

The display subsystem provides a unified terminal abstraction on top of two
mutually exclusive video backends:

- **VGA text mode** — writes directly to the `0xB8000` text buffer.
- **Linear framebuffer** — writes RGB pixels to a memory region provided by
  GRUB via the Multiboot info structure, and renders glyphs from a PSF1
  bitmap font.

The choice between the two is made **at runtime**, during boot, based on
what GRUB actually managed to set up — not at compile time. The rest of the
kernel (keyboard handler, `kprint`, etc.) never needs to know which backend
is active; it only talks to a small generic API (`putchar`, `scroll`,
`screen_switch`, ...).

```
kprint / kwrite / kputchar        (libc-like public API, kernel/helpers)
            │
            ▼
        terminal.c                 (generic orchestration: driver dispatch,
            │                       cursor, multi-screen logic)
            ▼
   t_display_driver (vtable)
      /            \
   vga.c        framebuffer.c      (hardware-specific implementations,
                                     know nothing about each other)
```

## Why runtime dispatch instead of compile-time `#ifdef`

An earlier version of this code picked the backend with a build-time
`VIDEO_MODE` macro (`#if VIDEO_MODE == MODE_FRAMEBUFFER`). This was dropped
because:

- GRUB may fail to provide a framebuffer even if the kernel requests one
  (old hardware, some VM configurations, etc.). A build-time choice cannot
  react to that; a runtime check can fall back to VGA automatically.
- `#ifdef`s scattered across data structures and logic make the code harder
  to reason about and effectively produce two different kernels to
  maintain.

The Multiboot header (`bootloader.s`) always requests a framebuffer and
always embeds the PSF1 font in the binary — the actual mode used is decided
in `init_display()` by inspecting `mbi->flags`.

## Core data structures (`display.h`)

### `t_screen` — active rendering state (single instance: `g_screen`)

Holds everything needed to draw to the **currently visible** output. There
is only ever one of these, regardless of how many logical screens exist.

| Field       | Meaning                                                              |
|-------------|-----------------------------------------------------------------------|
| `buf`       | Pointer to the physical output: `0xB8000` (VGA) or the framebuffer address (FB) |
| `back_buf`  | Single shared pixel back buffer, used only in framebuffer mode        |
| `col`,`row` | Current cursor position (in character cells)                          |
| `width`, `height`, `pitch`, `bpp` | Geometry reported by GRUB (FB) or fixed 80x25 (VGA) |
| `color`     | Current packed foreground/background color (see [Color](#color-system)) |
| `mode`      | `0` = framebuffer, `1` = VGA                                          |
| `cursor_col`, `cursor_row` | Last position where the blinking cursor was actually drawn (`-1` = none). Used to erase it cleanly before redrawing it elsewhere. |

### `t_screen_data` — logical content of one virtual screen (array: `g_screens[MAX_SCREENS]`)

A compact, backend-agnostic snapshot of a screen's text content. Cheap
enough to keep one per virtual screen (`MAX_SCREENS = 4`) without a heap
allocator.

| Field                | Meaning                                   |
|-----------------------|--------------------------------------------|
| `text_buf[80*25]`     | Character grid                            |
| `color_buf[80*25]`    | Packed color per cell                     |
| `col`, `row`, `color` | Saved cursor state for that screen        |

**Why this split matters (memory):** a naive design would keep a full pixel
back buffer (`800*400*4 bytes ≈ 1.25 MB`) per virtual screen — 5 MB for
4 screens, on a kernel with no heap yet. Instead, only **one** pixel back
buffer exists (`g_screen.back_buf`, sized once at boot), shared by whichever
screen is currently active. Inactive screens only keep their compact
`text_buf`/`color_buf` (~4 KB each) and get re-rasterized into the shared
back buffer on switch. Total cost: ~1.27 MB instead of ~5 MB.

### `t_display_driver` — the vtable

```c
typedef struct s_display_driver {
	void (*putchar)(char c);
	void (*scroll)(void);
	void (*clear)(void);
} t_display_driver;
```

`current_driver` points to either `vga_driver` or `fb_driver`, selected once
in `init_display()`. Every backend function has to match these exact
signatures — this is what lets `terminal.c` stay ignorant of which backend
it's talking to.

## Color system

Color is a backend-agnostic concept: a 4-bit foreground + 4-bit background
index into a fixed 16-color palette (the classic CGA/VGA palette),
packed into a single byte — exactly the format VGA text mode expects
natively.

```c
typedef enum e_color { COLOR_BLACK, COLOR_BLUE, ..., COLOR_WHITE } t_color;

u8_t  make_color(t_color fg, t_color bg);   // fg | (bg << 4)
u32_t color_to_rgb(t_color color);          // palette lookup, FB only
```

- **VGA** uses the packed byte directly — no conversion needed, it's the
  native hardware format for `0xB8000`.
- **Framebuffer** decodes each nibble and looks it up in `color_to_rgb()`
  to get a 32-bit RGB value.

This intentionally caps the framebuffer to the same 16 colors as VGA, even
though the hardware could display millions. That's a deliberate simplicity
trade-off for the current stage of the project — see
[Future work](#future-work).

## Rendering pipeline

### Writing a character

```
putchar(c)                          [terminal.c, public API]
  → current_driver->putchar(c)      [putchar_vga or putchar_fb]
      → putpixel_vga / draw_glyph   [draws into the physical buffer /
                                      shared back buffer, and mirrors the
                                      char+color into g_screens[current_screen]]
  → update_cursor()                 [erases old cursor, draws new one]
```

`putchar_vga` / `putchar_fb` handle `\n`/`\t`, cursor advancement, and
triggering a scroll when reaching the bottom of the screen. They do **not**
call `update_cursor()` themselves — that responsibility belongs entirely to
`terminal.c`, so backends never need to know about cursor logic.

### `putpixel_*` vs `putchar_*`

Two different needs are deliberately kept separate:

- **`putchar_*(c)`** — sequential input. "Write the next character and
  advance the cursor." Used for live typing / streamed output.
- **`putpixel_vga(c, color, x, y)` / `putpixel_fb(c, color, col, row)`** —
  direct positional write. "Draw exactly this character at this cell,
  nothing else." No cursor side effects, no scroll trigger. Used for
  restoring a screen's content (see [Screen switching](#screen-switching)),
  where positions are already known from `text_buf`/`color_buf` and must not
  be recomputed or perturbed by an in-flight scroll.

Reusing `putchar_*` for restoration was tried and rejected: it re-triggers
scroll logic and overwrites `g_screens[current_screen]` mid-read, corrupting
the very data being restored.

### The framebuffer back buffer

`g_screen.back_buf` is the **source of truth** for framebuffer content: it
always reflects what the terminal actually contains, with the cursor never
drawn into it. `swap_rect()` copies a rectangle from `back_buf` to the
physical framebuffer (`g_screen.buf`) whenever something changes.

The blinking cursor is drawn **only** on the physical buffer, never in
`back_buf`. To move it without leaving a trail, `draw_cursor()`:

1. If a previous cursor position is recorded (`g_screen.cursor_col >= 0`),
   restores that cell from `back_buf` via `swap_rect` (erasing the old
   cursor with the "clean" content).
2. Draws the new cursor directly onto the physical buffer.
3. Records the new position in `g_screen.cursor_col/row`.

Any function that does a **global** `swap_rect` (`scroll_fb`, `clear_fb`)
must reset `cursor_col`/`cursor_row` to `-1`, since the previous cursor
position becomes meaningless after a full repaint.

## Screen switching

The three layers involved don't all live at the same "depth", and data
moves between them in **two distinct flows**, not a single simple pipe:

- **(a) Typing a character** — writes fan out from the incoming char to
  *two* places at once: the visible buffer (drawn immediately) and the
  matching `g_screens[current_screen]` cell (mirrored, so it survives a
  future switch away and back).
- **(b) Switching screens** — the flow reverses: the newly selected
  screen's saved cells are read back out of `g_screens[new_id]` and
  rasterized into the shared back buffer, cell by cell.

### Framebuffer mode

```
                            CURSOR (draw_cursor)
                       writes directly, bypasses back_buf
                                     │
                                     ▼
              ┌────────────────────────────────────────┐
              │              FRONT BUFFER              │  ← physical
              │   physical pixels (g_screen.buf)       │    framebuffer,
              └─────────────────▲──────────────────────┘   what's visible
                                │  swap_rect()
                                │  copy: back_buf -> front
                                │  (this also erases the old cursor)
              ┌────────────────┴────────────────────────┐
              │               BACK BUFFER               │  ← single shared
              │    clean pixels (g_screen.back_buf)     │    instance, never
              └─────────────────▲───────────────────────┘    contains the cursor
                                │
                     (a) putpixel_fb(c, color, col, row)
                         one cell, drawn as it's typed
                                │▲
                                │└── (b) screen_switch(): rasterizes
                                │        every cell of the newly
                                │        selected screen, in a loop
                                ▼
       ┌─────────────────────────────────────────────────────┐
       │  (a) putchar_fb() ALSO mirrors the same char here,  │
       │      in parallel with drawing it above              │
       ▼                                                     │
 ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
 │ screens[0]  │ │ screens[1]  │ │ screens[2]  │ │ screens[3]  │
 │ text_buf    │ │ text_buf    │ │ text_buf    │ │ text_buf    │
 │ color_buf   │ │ color_buf   │ │ color_buf   │ │ color_buf   │
 └─────────────┘ └─────────────┘ └─────────────┘ └─────────────┘
                        ▲
                current_screen selects exactly one of these:
                the target of (a)'s mirror, the source of (b)'s rasterize
```

### VGA mode

No back buffer exists in this mode — `g_screen.buf` points straight at the
physical VGA memory (`0xB8000`), so "drawing" and "being visible" are the
same write. The hardware cursor (`set_cursor`) is a CRTC register, not a
pixel, so there's no draw/erase dance to do.

```
              ┌────────────────────────────────────────┐
              │              FRONT BUFFER              │  ← 0xB8000, IS the
              │        g_screen.buf == 0xB8000         │    visible output
              └─────────────────▲──────────────────────┘
                                │
                     (a) putpixel_vga(c, color, x, y)
                         one cell, written directly, no back buffer step
                                │▲
                                │└── (b) screen_switch(): rewrites every
                                │        cell of the newly selected screen
                                ▼
       ┌─────────────────────────────────────────────────────┐
       │  (a) putchar_vga() ALSO mirrors the same char here  │
       ▼                                                     │
 ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
 │ screens[0]  │ │ screens[1]  │ │ screens[2]  │ │ screens[3]  │
 │ text_buf    │ │ text_buf    │ │ text_buf    │ │ text_buf    │
 │ color_buf   │ │ color_buf   │ │ color_buf   │ │ color_buf   │
 └─────────────┘ └─────────────┘ └─────────────┘ └─────────────┘
```

Only one of these two panels is active at runtime, chosen once in
`init_display()` — never both, and never switched mid-boot.

Only the screen matching `current_screen` ever gets rasterized into the
shared back buffer (or, in VGA mode, straight into `0xB8000`); the other
three stay dormant as plain character grids until selected. This is what
keeps memory cost low: 4 compact grids (~4 KB each) instead of 4 full pixel
buffers (~1.25 MB each).

`screen_switch(new_id)` (in `terminal.c`) makes a virtual screen visible:

1. Update `current_screen`.
2. `current_driver->clear()` — wipes the physical/back buffer and resets
   the cursor tracking state.
3. Walk `g_screens[new_id].text_buf`/`color_buf` cell by cell, calling
   `putpixel_vga`/`putpixel_fb` directly (never `putchar_*`) to redraw each
   non-empty cell at its exact position.
4. Restore `g_screen.col`/`row`/`color` from the saved screen state and
   call `update_cursor()`.

Every `putchar_*` call also mirrors the character it just wrote into
`g_screens[current_screen]`, so the compact per-screen state always stays
in sync with what's on screen — no separate synchronization step is needed
outside of `screen_switch`.

## Boot-time setup

`bootloader.s` (Multiboot header) always requests a framebuffer (`1680x1000`,
32bpp) and always embeds the PSF1 font (`font_data`) in `.rodata`, regardless
of whether GRUB honors the request. This matches the runtime-driven design:
the kernel doesn't know at build time which mode it will end up in, so both
paths must always be available in the binary.

`init_display(multiboot_info *mbi)`:

```c
if (mbi->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO && mbi->framebuffer_type == 1) {
    init_fb(mbi);          // g_screen.buf = framebuffer_addr, back_buf = static array
    current_driver = &fb_driver;
} else {
    init_vga();             // g_screen.buf = 0xB8000, back_buf = NULL
    current_driver = &vga_driver;
}
```

## Public API

| Function                          | File          | Purpose |
|-----------------------------------|---------------|---------|
| `init_display(mbi)`               | `init.c`      | Detect backend, set up `g_screen` |
| `init_term(void)`                 | `init.c`      | Clear screen, load font (FB), reset cursor |
| `putchar(char c)`                 | `terminal.c`  | Write one character through the active driver + cursor |
| `scroll(void)`                    | `terminal.c`  | Scroll the active driver + cursor |
| `set_color(t_color fg, t_color bg)` | `terminal.c` | Set `g_screen.color` |
| `screen_switch(int id)`           | `terminal.c`  | Make virtual screen `id` visible |
| `kputchar(char c)`                | `kprint/`     | libc-like alias for `putchar` |
| `kwrite(const void*, size_t)`     | `kprint/`     | libc-like `write()` |
| `kprint(const char*, ...)`        | `kprint/`     | libc-like `printf()` |

Nothing outside `terminal.c` should call `current_driver->putchar` (or
`putchar_vga`/`putchar_fb`) directly — always go through `putchar()`.

## Future work

See [TODO.md](TODO.md) for the current list of planned improvements to this
module (RGB color support, dynamic screen allocation, IRQ-driven keyboard
input).
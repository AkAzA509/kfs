# Display Subsystem

## Overview

The display subsystem is the **backend layer**: it knows how to draw a
single glyph at a physical screen position, and nothing else. It has no
concept of scrollback, virtual screens, or a logical write cursor, all
of that lives one layer up, in `console.c` (see [console.md](console.md)).

Two mutually exclusive video backends exist:

- **VGA text mode** writes directly to the `0xB8000` text buffer.
- **Linear framebuffer** writes RGB pixels to a memory region provided by
  GRUB via the Multiboot info structure, and renders glyphs from a PSF1/PSF2
  bitmap font.

The choice between the two is made **at runtime**, during boot, based on
what GRUB actually managed to set up.

```
kputchar / printf                 (console.c, generic orchestration: scrollback,
      │                             view offset, multi-screen, cursor math)
      ▼
t_display_driver (vtable)         (display_d: this document)
   /            \
vga.c        framebuffer.c        (hardware-specific implementations,
                                    know nothing about each other, nothing
                                    about scrollback, nothing about which
                                    virtual screen is active)
```

The dividing line is strict: **anything below this vtable operates only on
physical, currently-visible coordinates**. It never sees a logical line
number, never sees `head`/`view_offset`, never decides whether something
should scroll. `console.c` decides *what* to draw and *where on the visible
screen*; the backend only knows *how* to put it there.

## Why runtime dispatch instead of compile-time `#ifdef`

An earlier version of this code picked the backend with a build-time
`VIDEO_MODE` macro. This was dropped because:

- GRUB may fail to provide a framebuffer even if the kernel requests one
  (old hardware, some VM configurations, etc.). A build-time choice cannot
  react to that; a runtime check can fall back to VGA automatically.
- `#ifdef`s scattered across data structures and logic make the code harder
  to reason about and effectively produce two different kernels to
  maintain.

The Multiboot header always requests a framebuffer and always embeds the
font in the binary; the actual mode used is decided in `init_display()` by
inspecting `mbi->flags`.

## Core data structures

### `t_screen`, physical output descriptor (single instance: `g_screen`)

Describes the **hardware**, not any content. There is exactly one of these
regardless of how many virtual screens exist above it, the machine only
has one physical display.

| Field         | Meaning                                                              |
|---------------|-----------------------------------------------------------------------|
| `buf`         | Pointer to the physical output: `0xB8000` (VGA) or the framebuffer address (FB) |
| `back_buf`    | Single shared pixel back buffer, framebuffer mode only (see [The back buffer](#the-back-buffer)) |
| `width`, `height`, `pitch`, `bpp` | Geometry reported by GRUB (FB) or fixed 80x25 (VGA) |
| `total_rows`, `total_cols` | Visible grid size, derived from `width`/`height` and font metrics |
| `mode`        | `0` = framebuffer, `1` = VGA                                          |
| `cursor_col`, `cursor_row` | Last position where the cursor was actually drawn (`int`, `-1` = none). Must be initialized to `-1` explicitly at boot, it is **not** zero-safe, see [Pitfalls](#known-pitfalls). |

Note what is deliberately **not** here anymore: no `col`/`row`/`color`.
Those are per-virtual-screen logical state and belong to `t_screen_data`
in `console.c`'s domain, keeping them here caused several desync bugs where
the physical struct and the logical one disagreed about the cursor
position after a screen switch.

### `t_display_driver`, the vtable

```c
typedef struct s_display_driver {
	void (*putchar_at)(char c, u8_t color, size_t col, size_t row);
	void (*scroll)(void);          // physical scroll by one row, pinned-bottom case
	void (*clear)(void);           // wipe the physical/back buffer
	void (*cursor_update)(void);
	void (*flush_partial)(u32_t x, u32_t y, u32_t w, u32_t h); // nullable, FB only
	void (*flush_screen)(void);    // nullable, FB only
} t_display_driver;
```

`display_d` points to either `vga_driver` or `fb_driver`, selected once in
`init_display()`.

**`flush_partial`/`flush_screen` are empty for VGA on purpose**. VGA
writes straight to `0xB8000`, there's no back buffer to synchronize.

## Color system

Color is a backend-agnostic concept: a 4-bit foreground + 4-bit background
index into a fixed 16-color palette, packed into a single byte, the
format VGA text mode expects natively.

```c
typedef enum e_color { COLOR_BLACK, COLOR_BLUE, ..., COLOR_WHITE } t_color;

u8_t  make_color(t_color fg, t_color bg);   // fg | (bg << 4)
u32_t color_to_rgb(t_color color);          // palette lookup, FB only
```

- **VGA** uses the packed byte directly, no conversion needed.
- **Framebuffer** decodes each nibble and looks it up in `color_to_rgb()`.

**Pitfall to watch for:** any physical fill operation on the framebuffer
(`clear_physical_fb`, `scroll_physical_fb`'s newly-exposed row) must go
through `color_to_rgb((t_color)((color >> 4) & 0x0F))` to get the
background. Writing the raw packed attribute byte directly into a pixel
buffer, or hardcoding `0x000000`/`memset(..., 0, ...)`, silently ignores
whatever background color/theme is actually active and was the cause of
a visible "wrong background flashes in" bug during screen switches and
scrolling.

This intentionally caps the framebuffer to the same 16 colors as VGA, even
though the hardware could display millions, a deliberate simplicity
trade-off, see [Future work](#future-work).

## The back buffer

`g_screen.back_buf` exists purely to avoid tearing/flicker: glyphs are
rendered into it first, then copied to the physical framebuffer in a
separate, explicit sync step. It does **not** exist to hold scrollback,
that would require one per virtual screen and cost several MB for no
reason. Scrollback is entirely `console.c`'s responsibility, stored as
character cells, not pixels (see [console.md](console.md)).

Two primitives, kept deliberately separate:

- **`render_glyph_fb(c, color, col, row)`**, rasterizes one glyph into
  `back_buf` only. No sync to the physical buffer.
- **`flush_rect_fb(x, y, w, h)` / `flush_screen_fb()`**, the explicit sync
  step (`swap_rect` internally), called **once per logical operation**
  (one glyph, or one full redraw), never once per pixel or per row inside
  a loop. This is why a full-screen redraw does dozens of `putchar_at`
  calls followed by exactly one `flush_screen_fb()`, instead of a flush
  after every cell.

## Cursor drawing (framebuffer)

The cursor is drawn as a small overlay rectangle directly onto the
**physical** buffer, never into `back_buf`, `back_buf` must always
reflect real content only, so it can be used to cleanly erase the cursor
later.

`draw_cursor(cx, cy, color)`:

1. If a previous position is recorded (`g_screen.cursor_col >= 0`), erase
   it by copying that rectangle back from `back_buf` (which never had a
   cursor drawn into it, so this restores the real glyph underneath).
2. Compute the target rectangle from `font_info.width`/`font_info.height`
  , the kernel supports both PSF1 and PSF2 with different glyph dimensions.
3. Draw the new cursor overlay directly onto the physical buffer.
4. Record the new position.

Any function that does a full-screen `flush_screen_fb()` (a full physical
scroll, a full clear) invalidates the cursor cache
(`cursor_col = cursor_row = -1`), the previous position becomes
meaningless after the entire physical buffer has been overwritten from
`back_buf` in one shot.

`cx`/`cy` passed into `draw_cursor` must already be a **row relative to
what's currently visible**, never a raw logical line number. The caller
(`console.c`) is responsible for that conversion; this backend has no way to
know it's wrong if it isn't.

## VGA specifics

No back buffer exists in this mode; `g_screen.buf` points straight at
`0xB8000`, so drawing and being visible are the same write. The cursor is
a CRTC hardware register (`set_cursor`, via `outb` to ports `0x3D4`/
`0x3D5`), not a pixel overlay, no draw/erase dance needed, which is why
`flush_partial`/`flush_screen` are empty for this backend.

## Boot-time setup

`init_display(multiboot_info *mbi)`:

```c
if (mbi->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO && mbi->framebuffer_type == 1) {
	init_fb(mbi);
	display_d = &fb_driver;
} else {
	init_vga();
	display_d = &vga_driver;
}
g_screen.cursor_col = -1;
g_screen.cursor_row = -1;
```

The explicit `-1` initialization matters: `g_screen` is a global, zero-
initialized in BSS by default. Without this line, the very first
`cursor_update()` call (triggered by the boot log's first `\n`, before the
keyboard even takes over) sees `cursor_col == cursor_row == 0`, matches
whatever position it's asked to draw at, and returns early without ever
drawing anything, the cursor logic starts from a phantom state that
doesn't correspond to any real drawn position. The symptom was a visible
cursor artifact left behind on every line, which only self-corrected once
the first full-screen scroll forced a global resync.

## Public API

| Function                          | File            | Purpose |
|-----------------------------------|-----------------|---------|
| `init_display(mbi)`               | `init.c`        | Detect backend, set up `g_screen` |
| `putpixel_vga(c, color, x, y)`    | `vga.c`         | Draw one glyph at a physical VGA cell |
| `scroll_physical_vga(void)`       | `vga.c`         | Physical row scroll (VGA memory) |
| `clear_physical_vga(void)`        | `vga.c`         | Wipe `0xB8000` with the active background |
| `update_cursor_vga(void)`         | `vga.c`         | Move the CRTC hardware cursor |
| `render_glyph_fb(c, color, col, row)` | `framebuffer.c` | Rasterize one glyph into `back_buf` (no sync) |
| `flush_rect_fb` / `flush_screen_fb` | `framebuffer.c` | Explicit `back_buf` → physical sync |
| `scroll_physical_fb(void)`        | `framebuffer.c` | Physical pixel scroll on `back_buf` |
| `clear_physical_fb(void)`         | `framebuffer.c` | Wipe `back_buf` with the active background |
| `update_cursor_fb(void)`          | `framebuffer.c` | Erase/redraw the pixel cursor overlay |

Nothing outside `console.c` should call any of these directly, always go
through the generic API described in [console.md](console.md).

## Known pitfalls

A running list of mistakes made and fixed at this layer, kept as a
reminder for future backends or changes:

- **Never call a nullable vtable function unconditionally.** `flush_partial`/
  `flush_screen` are empty not `NULL` for VGA by design.
- **Never write a raw packed color attribute into a pixel buffer.** Always
  decode through `color_to_rgb()` first.

## Future work

See [TODO.md](TODO.md) for the current list of planned improvements to this
module.
# TODO

Planned improvements and known gaps, organized by module. See the
corresponding `<module>.md` file for context on why each item matters.

## Display

- [ ] Switch the color model to true 24-bit RGB (framebuffer currently
      capped at the 16-color VGA-compatible palette). VGA backend would
      approximate down to its 16 colors via nearest-color match instead of
      the other way around. Change is isolated to `display.h`/`display.c`.
- [ ] Dynamic screen allocation (`g_screens`, and eventually `back_buf`)
      once a physical frame allocator exists — currently fixed-size,
      statically allocated in `.bss` (`MAX_SCREENS = 4`).
- [ ] IRQ-driven keyboard input (ring buffer) to decouple scancode capture
      from character rendering. Currently polling `0x60` in a busy loop;
      fast typing can outrun the 8042 hardware buffer and drop/delay
      scancodes.

## Memory

- [ ] _Not documented yet._

## Toolchain

- [x] Move the toolchain methode and creation from the root readme to is file in
      docs/toolchain.md

## ROOT readme

- [x] Remove the toolchain part and update/folow the new config and handled case

## Misc

- [x] Create a bootloader.md doc
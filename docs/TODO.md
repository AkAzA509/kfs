# TODO

Planned improvements and known gaps, organized by module. See the
corresponding `<module>.md` file for context on why each item matters.

## Display

- [ ] Switch the color model to true 24-bit RGB (framebuffer currently
      capped at the 16-color VGA-compatible palette). VGA backend would
      approximate down to its 16 colors via nearest-color match instead of
      the other way around. Change is isolated to `display.h`/`display.c`.
- [ ] Dynamic screen allocation (`g_screens`, and eventually `back_buf`)
      once a physical frame allocator exists, currently fixed-size,
      statically allocated in `.bss` (`MAX_SCREENS = 4`).
- [ ] IRQ-driven keyboard input (ring buffer) to decouple scancode capture
      from character rendering. Currently polling `0x60` in a busy loop;
      fast typing can outrun the 8042 hardware buffer and drop/delay
      scancodes.
- [x] Handle the backspace key in the keyboard driver (improve the driver overall)
- [ ] Add a psf2 font handler to be capable of rendering both psf1 and psf2 fonts

## Memory

- [ ] _Not documented yet._

## Toolchain

- [x] Move the toolchain methode and creation from the root readme to is file in
      docs/toolchain.md

## ROOT readme

- [x] Remove the toolchain part and update/folow the new config and handled case

## GDT

- [ ] TSS descriptor + `ltr`, needed once ring 3 code and an IDT exist,
  so the CPU knows which kernel stack (`SS0`/`ESP0`) to switch to on interrupt from ring 3.
- [ ] `iret`-based jump into ring 3, once there is user code to jump to.
- [x] Complete the asm update_gdt() function

## Misc

- [x] Create a bootloader.md doc
- [x] rewrite the print API (kwrite, kputchar, kprint ...) its not well organise adn not enough clear to me
      and add a public api for printf and all is family that i can handle now, not the fd specific version.

## FPU handling
- [ ] Remove the fpu init from the bootloader and make a init file, for gdt and paging too
- [ ] No #MF handler for floating point exeption (divide per 0, overflow ...) IDT needed
- [ ] When we have a schedeler handle the context switching (FXSAVE/FXRSTOR)
- [ ] The FPU system is not active with SSE2, mean less precision/perf, can be enable later
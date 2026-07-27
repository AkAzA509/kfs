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
- [x] Add a psf2 font handler to be capable of rendering both psf1 and psf2 fonts
- [ ] Handle the unicode lookup table if the font support it

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
- [x] Remove the fpu init from the bootloader and make a init file, for gdt and paging too
- [ ] No #MF handler for floating point exeption (divide per 0, overflow ...) IDT needed
- [ ] When we have a schedeler handle the context switching (FXSAVE/FXRSTOR)
- [ ] The FPU system is not active with SSE2, mean less precision/perf, can be enable later

## TTY / Console

- [x] Line editor / input discipline: `input_boundary`, bounding backspace to
  the current prompt
- [ ] Had `^L` handling.
- [ ] Bring `screen_clear()` in line with the non-destructive design (reuse
  `screen_newline()` instead of `memset`).
- [ ] Revisit tab-stop expansion when it crosses a line boundary mid-loop.
- [ ] Scroll mess TO FIX
- [ ] when we switch screen the promt doesn't appear, unless i hit enter TO FIX

## Keyboard

- [ ] when the tty prompt has been implemented add a end/^a for deplacement in the command,
  and add arrow left/right cursor deplacement for modification 
code for leftarro: 0x000000e0 press
									 0x0000004b
									 0x000000e0 release
									 0x000000cb.

				rightarro: 0x000000e0 press
									 0x0000004d
									 0x000000e0 release
									 0x000000cd.

							^a : 0x0000001d ctrl press
									 0x0000001e a press
									 0x0000009e a release
									 0x0000009d ctrl release

							end: 0x000000e0 press
									 0x0000004f
									 0x000000e0 release
									 0x000000cf
)
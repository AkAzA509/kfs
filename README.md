# kfs

> A 32-bit x86 kernel built from scratch.

![Language](https://img.shields.io/badge/language-C-00599C?style=for-the-badge)
![Architecture](https://img.shields.io/badge/arch-i686--elf-8A8A8A?style=for-the-badge)
![License](https://img.shields.io/badge/license-GPLv3-3DA639?style=for-the-badge)
![Status](https://img.shields.io/badge/status-in%20progress-F2C94C?style=for-the-badge)
![CI](https://img.shields.io/github/actions/workflow/status/AkAzA509/kfs/test.yaml?branch=gdt&style=for-the-badge&label=CI)

`kfs` (Kernel From Scratch) is a custom x86 kernel project written as part of
42's systems programming curriculum. The goal is about building each subsystem
(GDT, framebuffer, drivers, memory management, ...) of a kernel, part by part.

## Project structure

```
kernel/
├── arch/         # architecture-specific code (x86)
├── core/         # architecture-independent kernel code
├── drivers/      # specific drivers file
├── fs/           # file/virtual system
├── include/      # header declarations
├── ressources/   # font file and other
└── syscall       # syscall implementation
libc/
├── include/     # public headers / libc-like reimplementation
└── **/          # implementation matching the headers above
docs/            # technical & developer documentation
grub.cfg         # GRUB boot configuration
```

## Status

| Subsystem | State |
|---|---|
| Bootloader (GRUB / Multiboot) | ✅ Done |
| VGA / linear framebuffer, double buffering | ✅ Done |
| PSF font rendering (PSF1 / PSF2) | ✅ Done |
| `printf` family (custom `vprint_core`) | ✅ Done |
| GDT (ring 0 / ring 3 segments) | ✅ Done |
| Keyboard driver, virtual terminals | ✅ Done |
| ACPI / QEMU shutdown | ✅ Done |
| IDT / interrupt handling | ⬜ In progress |
| Physical & virtual memory management | ⬜ Planned |
| filesystem | ⬜ Planned |
| Userspace / syscalls | ⬜ Planned |

## Features

**Boot & architecture**
- Boots via GRUB using a Multiboot-compliant kernel
- Meaty Skeleton architecture (see [OSDev](https://wiki.osdev.org/Meaty_Skeleton))
- GDT implementation with ring 0 / ring 3 segments

**Display**
- Text output through VGA text mode or a linear framebuffer, depending on
  what GRUB provides
- Double-buffered rendering
- Custom PSF1/PSF2 font parser with a normalized font abstraction

**Input & terminal**
- Keyboard driver (IRQ-based)
- Up to 4 virtual terminals (configurable in `init.h`), switch with F1-F4,
  exit with Escape
- Small tty with input history and keybinds (arrows, Page Up/Down, Home/End,
  Ctrl+A, Ctrl+L)
- Basic shell with utility and debug functions (reboot, halt, pstack, help ...)

**Core libc**
- `printf` family reimplemented from scratch, including bit-exact IEEE 754
  float formatting done in pure integer arithmetic
- Validated against host libc as ground truth (see `tests/`)

## Requirements
- An i386 cross-compiler toolchain — see [`docs/toolchain.md`](docs/toolchain.md)
- `nasm`
- `ld` (with `elf_i386` support)
- `grub-mkrescue` and `grub-file`
- `qemu-system-i386`

## Build & run
```sh
make                   # build the kernel binary (bin/kernel)
make up                # build the ISO and run it in QEMU
make dev               # run the raw kernel binary directly in QEMU (no ISO)
make debug             # build a debug ISO (-DDEBUG=1) and run it in QEMU with serial output
make clean             # remove object files
make fclean            # remove all build artifacts (binaries, ISOs)
make re                # fclean + generate compile_commands.json for clangd (via bear)
```

## Documentation

Detailed technical write-ups (multiboot header, GRUB, memory layout, GDT,
framebuffer, PSF fonts, etc.) live in [`docs/`](./docs/README.md).

A curated list of resources used while building this project (OSDev wiki,
Intel SDM, reference kernels, books) is available in
[`docs/ressources.md`](./docs/ressources.md).

## License

This project is licensed under the GNU General Public License v3.0 — see
[`LICENSE`](./LICENSE).

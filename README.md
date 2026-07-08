# kfs

## Overview
This project is a small kernel and bootloader setup for a custom x86 operating system, built from scratch as part of the **kfs** (Kernel From Scratch) project at 42. The repository contains the bootloader, the kernel entry point, and the build rules used to generate the final ISO image.

## Project structure
- `bootloader/` - assembly bootloader and multiboot header
- `includes/` - public header and libc like reimplementation
- `kernel/` - kernel entry point, terminal, and drivers (keyboard, VGA, framebuffer)
- `helpers/` - low-level utilities (memcpy, memset, kprint, ...)
- `docs/` - technical and developer documentation (multiboot, memory layout, etc.)
- `linker.ld`, `grub.cfg` - linking and GRUB boot configuration

## Features
- Boots via GRUB using a Multiboot-compliant kernel
- Text output through VGA text mode or linear framebuffer, depending on what the bootloader/GRUB provides
- Basic keyboard driver

## Requirements
- An i386 cross-compiler toolchain see [`docs/toolchain.md`](docs/toolchain.md)
- `nasm`
- `ld` (with `elf_i386` support)
- `grub-mkrescue` and `grub-file`
- `qemu-system-i386`

## Build & run
```sh
make          # build the kernel binary (bin/kernel)
make up       # build the ISO and run it in QEMU
make dev      # run the raw kernel binary directly in QEMU (no ISO)
make debug    # build a debug ISO (adds -DDEBUG=1) and run it in QEMU with serial output
make clean    # remove object files
make fclean   # remove all build artifacts (binaries, ISOs)
make re       # fclean + all
```

## Intern documentation
Detailed technical explanations (multiboot header, GRUB, memory layout, etc.) live in the [`docs/`](./docs/README.md) folder. This README only covers the project overview; refer to `docs/` for implementation details.

## Documentations and ressources
- Fundamental Concepts and Structure of the Linux Kernel (old version v2.4.20) [here](https://www.irif.fr/~carton/Enseignement/Architecture/Cours/Virtual/linux.pdf)
- Some real linux kernel docs [here](https://www.kernel.org/)
- The OSdev documentations and guide [here](https://wiki.osdev.org/Expanded_Main_Page)
- Writing a Tiny x86 Bootloader in assembly [here](https://www.joe-bergeron.com/posts/Writing%20a%20Tiny%20x86%20Bootloader/)
- Operating Systems: From 0 to 1 by tuhdo [here](https://github.com/tuhdo/os01/tree/master)
- The little book about OS development by Erik Helin, Adam Renberg [here](https://littleosbook.github.io/)
- Kernels 101 – Let’s write a Kernel by Arjun Sreedharan [here](https://arjunsreedharan.org/post/82710718100/kernels-101-lets-write-a-kernel)
- Kernels 201 - Let’s write a Kernel with keyboard and screen support [here](https://arjunsreedharan.org/post/99370248137/kernels-201-lets-write-a-kernel-with-keyboard)
- Writing My Own OS by Frank Rosner [here](https://dev.to/frosnerd/series/9585)
- NyanOS repo [here](https://github.com/yunusemreduran388-ux/NyanOS-v1)
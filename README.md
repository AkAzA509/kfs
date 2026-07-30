# kfs

## Overview
This project is a small kernel and bootloader setup for a custom x86 operating system, built from scratch as part of the **kfs** (Kernel From Scratch) project at 42. The repository contains the bootloader, the kernel entry point, a lib, and the build rules used to generate the final ISO image.

## Project structure
- `kernel/` - kernel code space
- `kernel/arch/` - architecture specific code
- `kernel/include` - all the kernel header space declaration
- `kernel/kernel` - rest of the kernel code not architecture specific
- `libc` - general lib shared with the kernel and the future user-space
- `libc/include/` - public header and libc like reimplementation
- `libc/**` - code directory linked to the header
- `docs/` - technical and developer documentation (bootloader, io, display, etc)
- `grub.cfg` - GRUB boot configuration

## Features
- Boots via GRUB using a Multiboot-compliant kernel
- Text output through VGA text mode or linear framebuffer, depending on what the bootloader/GRUB provides
- Keyboard driver
- Virtual terminal up to 4 (can be update in init.h)
- f1-f4 keybind for multiscreen and escape key to exit the kernel
- Basic tty with small history and some keybind (4 arrows, pg up/down, end, home, ctrl + a/l)
## Requirements
- An i386 cross-compiler toolchain see [`docs/toolchain.md`](docs/toolchain.md)
- `nasm`
- `ld` (with `elf_i386` support)
- `grub-mkrescue` and `grub-file`
- `qemu-system-i386`

## Build & run
```sh
make                  # build the kernel binary (bin/kernel)
make up               # build the ISO and run it in QEMU
make dev              # run the raw kernel binary directly in QEMU (no ISO)
make compile_commande # Use bear to generate a json compile rule for clangd autocompletion
make debug            # build a debug ISO (adds -DDEBUG=1 and some testing file) and run it in QEMU with serial output
make clean            # remove object files
make fclean           # remove all build artifacts (binaries, ISOs)
make re               # fclean + all
```

## Intern documentation
Detailed technical explanations (multiboot header, GRUB, memory layout, etc.) live in the [`docs/`](./docs/README.md) folder. This README only covers the project overview; refer to `docs/` for implementation details.

# Documentations and ressources

## Generale ressources and inspiration
- Fundamental Concepts and Structure of the Linux Kernel (old version v2.4.20) [here](https://www.irif.fr/~carton/Enseignement/Architecture/Cours/Virtual/linux.pdf)
- Some real linux kernel docs [here](https://www.kernel.org/)
- The OSdev documentations and guide [here](https://wiki.osdev.org/Expanded_Main_Page)
- Writing My Own OS by Frank Rosner [here](https://dev.to/frosnerd/series/9585)
- NyanOS repo [here](https://github.com/yunusemreduran388-ux/NyanOS-v1)
- Operating Systems: From 0 to 1 by tuhdo [here](https://github.com/tuhdo/os01/tree/master)
- The little book about OS development by Erik Helin, Adam Renberg [here](https://littleosbook.github.io/)

## Bootloader
- Writing a Tiny x86 Bootloader in assembly [here](https://www.joe-bergeron.com/posts/Writing%20a%20Tiny%20x86%20Bootloader/)
- GNU/GRUB multiboot doc [here](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
- Boot sequence doc from OSdev [here](https://wiki.osdev.org/Boot_Sequence)

## Keyboard and screen
- The little book about OS development by Erik Helin, Adam Renberg output section [here](https://littleosbook.github.io/#output)
- Kernels 101 – Let’s write a Kernel by Arjun Sreedharan [here](https://arjunsreedharan.org/post/82710718100/kernels-101-lets-write-a-kernel)
- Kernels 201 - Let’s write a Kernel with keyboard and screen support [here](https://arjunsreedharan.org/post/99370248137/kernels-201-lets-write-a-kernel-with-keyboard)
- GNU/GRUB multiboot doc [here](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
- OSdev Framebuffer implementation [linear fb here](https://wiki.osdev.org/Drawing_In_a_Linear_Framebuffer), [double buffering here](https://wiki.osdev.org/Double_Buffering)
- OSdev psf font [here](https://wiki.osdev.org/VGA_Fonts), [here](https://wiki.osdev.org/PC_Screen_Font)
- OSdev keybord ps/2 [here](https://wiki.osdev.org/PS/2_Keyboard)

## Code and file organisation
- Kernel architecture with Meaty skeleton, by OSdev[here](https://wiki.osdev.org/Meaty_Skeleton)
- Linux kernel architecture [here](https://www.kernel.org/)
- The Newlib project [here](https://sourceware.org/newlib/)

## GDT
- OSdev gdt, descriptors and segment [gdt here](https://wiki.osdev.org/Global_Descriptor_Table), [gdt2 here](https://wiki.osdev.org/GDT_Tutorial), [segment here](https://wiki.osdev.org/Segmentation), [descriptor here](https://wiki.osdev.org/Descriptors)
- Making an OS (x86) Chapter 6 - Entering Protected mode, GDT, Yt video [here](https://www.youtube.com/watch?v=Wh5nPn2U_1w)
- Intel® 64 and IA-32 Architectures Software Developer’s Manual Combined Volumes: 1, 2A, 2B, 2C, 2D, 3A, 3B, 3C, 3D, and 4 [here](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- The little book about OS development by Erik Helin, Adam Renberg segment section [here](https://littleosbook.github.io/#segmentation)
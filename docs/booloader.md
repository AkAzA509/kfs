# Bootloader ([`bootloader.s`](../bootloader/bootloader.s))

## Overview

This file is the very first code that runs before there is a stack, before
there is any C runtime, before the kernel exists as anything other than raw
bytes loaded into memory. Its only jobs are:

1. Carry a **Multiboot header** that GRUB recognizes, so it knows this file
   is a bootable kernel and how it would like to be loaded.
2. Set up the bare minimum the CPU needs to run C code: a stack.
3. Jump into `kernel_main`, written in C, passing along what GRUB found out
   about the machine.

Everything here runs in 32-bit protected mode, with interrupts disabled and
paging off, exactly the state Multiboot guarantees, nothing more.

## Boot flow

```
   ┌──────────┐      ┌──────────────────────┐      ┌───────────────────┐
   │  BIOS /  │      │        GRUB          │      │   this kernel     │
   │  UEFI    │ ──▶  │  (Multiboot loader)  │ ──▶  │      binary       │
   └──────────┘      └──────────────────────┘      └─────────────┬─────┘
                       1. scans first 8 KiB for                  │
                          MAGIC (0x1BADB002)                     │
                       2. reads MBFLAGS, honors what             │
                          it can (align modules,                 │
                          memory map, framebuffer                │
                          request)                               │
                       3. loads the kernel into RAM,             │
                          jumps to _start                        │
                                                                 ▼
                                                        ┌─────────────────────┐
                                                        │       _start        │
                                                        │  (this file, asm)   │
                                                        └─────────┬───────────┘
                                                                  │  1. set up stack
                                                                  │  2. clear eflags
                                                                  │  3. push ebx, eax
                                                                  ▼
                                                        ┌───────────────────┐
                                                        │    kernel_main    │
                                                        │        (C)        │
                                                        └───────────────────┘
```

At the moment `_start` gets control, two CPU registers carry everything GRUB
learned:

- **`eax`** the Multiboot magic number (`0x2BADB002`), confirming a
  Multiboot-compliant loader actually did the loading.
- **`ebx`** a pointer to the **Multiboot info structure**, GRUB's report
  on the machine: memory map, boot device, and if requested and
  available, framebuffer address/width/height/pitch/bpp. This is exactly
  the `mbi` pointer consumed by `init_display()` (see [`display.md`](display.md#Boot-time--setup)).

Both are pushed onto the stack before calling `kernel_main`, so the C side
receives them as its two arguments.

## The Multiboot header

```nasm
MBALIGN   equ 1 << 0   ; align loaded modules on page boundaries
MEMINFO   equ 1 << 1   ; provide a memory map
VIDMOD    equ 1 << 2   ; ask for a framebuffer / video mode
MBFLAGS   equ MBALIGN | MEMINFO | VIDMOD
MAGIC     equ 0x1BADB002
CHECKSUM  equ -(MAGIC + MBFLAGS)
```

`MAGIC + MBFLAGS + CHECKSUM` must sum to exactly `0`. This is how GRUB
validates the header isn't garbage before trusting it. The header itself
must live within the first 8 KiB of the file, 32-bit aligned hence its
own `.multiboot` section, placed first in the link order.

```
offset  field            value here      meaning
──────  ───────────────  ──────────────  ──────────────────────────────────
  0     magic            0x1BADB002      "this is a Multiboot kernel"
  4     flags            MBALIGN|        what this kernel is asking for
                          MEMINFO|VIDMOD
  8     checksum         -(magic+flags)  validity check, must sum to 0
 12     header_addr      0               unused (flags bit 16 not set)
 16     load_addr        0               unused
 20     load_end_addr    0               unused
 24     bss_end_addr     0               unused
 28     entry_addr       0               unused
 32     mode_type        1               0 = linear framebuffer, 1 = EGA text
 36     width            1680            requested framebuffer width
 40     height           1000            requested framebuffer height
 44     depth            32              requested bits per pixel
```

The last four fields (`mode_type`/`width`/`height`/`depth`) only exist
because `VIDMOD` is set in `MBFLAGS`, without that flag, GRUB would ignore
them entirely. Even with the flag set, this is a **request, not a
guarantee**: GRUB will try to honor it, but may fall back to whatever the
hardware/emulator actually supports. That's exactly why `init_display()`
never trusts these numbers directly, it re-reads the *actual* geometry
GRUB reports back at runtime via the Multiboot info structure, and picks
the VGA or framebuffer driver based on what was really granted, not on what
was asked for. See `display.md` for that runtime negotiation.

## Section layout of the compiled binary

```
┌─────────────────────────────────────────────────────────────┐
│ .multiboot   (must be within the first 8 KiB, 4-byte aligned)│
│   Multiboot header: magic, flags, checksum, video request     │
├─────────────────────────────────────────────────────────────┤
│ .bss         (uninitialized, zero-filled at load, 16-aligned) │
│   stack_bottom .. stack_top   (16 KiB kernel stack)            │
├─────────────────────────────────────────────────────────────┤
│ .rodata      (read-only data)                                 │
│   font_data[]   the raw PSF1 font file, embedded via incbin  │
├─────────────────────────────────────────────────────────────┤
│ .text        (executable code)                                │
│   _start:  the real entry point                               │
└─────────────────────────────────────────────────────────────┘
```

### Why the stack lives in `.bss`

`.bss` holds memory that's zeroed at load time but never given actual
content in the file on disk, the loader just reserves the space. A stack
doesn't need any initial content, so putting it in `.bss` (via `resb`,
"reserve bytes") keeps the kernel binary itself small: 16 KiB of stack
space costs 0 bytes in the file, only 16 KiB of RAM once loaded.

### Why the font is embedded via `incbin`

```nasm
section .rodata
global font_data
font_data:
	incbin "fonts/Lat15-VGA16.psf"
```

`incbin` drops the raw bytes of the PSF1 font file directly into the
kernel binary at assemble time, and `global font_data` exposes its start
address as a symbol the C side can reach with `extern unsigned char
font_data[];`. This is what lets `init_frambuffer()` (in `init.c`) read
`font_data[0..3]` for the PSF1 header and glyph bitmaps beyond that, with
no filesystem, no loader, nothing beyond "this data was baked into the
binary at build time." See `display.md` for how it's consumed.

## `_start`, step by step

```nasm
_start:
	mov esp, stack_top      ; 1. give the CPU a stack, C cannot run without one

	push 0
	popf                     ; 2. zero out eflags, start from a known, clean state

	push ebx                 ; 3. framebuffer/Multiboot info pointer (2nd C arg)
	push eax                 ; 4. Multiboot magic number   (1st C arg)

	extern kernel_main
	call kernel_main          ; 5. hand off to C, this never normally returns

	cli                       ; 6. if it ever does return: disable interrupts
.hang:
	hlt                       ;    halt the CPU
	jmp .hang                 ;    and if a non-maskable interrupt wakes it up,
                              ;    go straight back to halting
```

A few details worth calling out:

- **Stack alignment.** The System V ABI requires the stack to be 16-byte
  aligned at the point of a `call`. `stack_top` is aligned via `align 16`
  in `.bss`, and pushing two 4-byte values (`ebx`, `eax`) before the `call`
  instruction (which itself pushes a 4-byte return address) keeps that
  alignment intact for `kernel_main`. Getting this wrong doesn't crash
  immediately, it silently produces undefined behavior, often much later
  and in an unrelated-looking place, which is exactly why it's worth being
  precise about here even though it "just works" until it doesn't.
- **`push 0` / `popf`** doesn't just clear flags, it's the cheapest way to
  put a known, predictable value into `eflags` without relying on whatever
  garbage the CPU or GRUB left there.
- **The halt loop matters even though `kernel_main` never returns in
  practice.** If it ever did (a bug, a deliberate `return` while
  debugging), falling off the end of `_start` into whatever bytes follow in
  memory would be far worse than a controlled, silent halt.

## What this file deliberately does *not* do

- No paging, no GDT/IDT setup, no floating point initialization, the
  comments in the file are explicit about this being intentional. Those
  belong to later kernel initialization stages in C (or later asm files),
  not to the Multiboot entry point itself.
- No decision-making about VGA vs. framebuffer beyond *requesting* a
  preference. The actual choice is made once, at runtime, in
  `init_display()`, see [`display.md`](display.md#Boot-time--setup).
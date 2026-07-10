# I/O Ports ([io.c](../kernel/io.c))

## Overview

How the kernel talks to hardware devices through the x86 I/O address
space, and the technical behavior of the `in`/`out` instruction family
used to do so.

## Why it exists

x86 CPUs expose two distinct address spaces: regular memory, and a
separate **I/O address space** of 64 KB, numbered `0x0000` to `0xFFFF`.
Devices such as the keyboard controller, the PIC, the PIT timer, the RTC,
and (in an emulator) the ACPI chipset are not mapped into memory, they
are only reachable by writing to or reading from a specific port number
in this separate space. Regular `mov` instructions cannot touch this
space at all; only the dedicated `in`/`out` instruction family can.

This split is a legacy of the original PC architecture, but it is still
the standard way to talk to a large class of devices, and QEMU faithfully
emulates it, including for devices that only exist virtually, like its
ACPI power-management chipset.

## `outb` / `outw` / `outl`

These three functions wrap the `out` instruction, one per width: a byte
(8 bits), a word (16 bits), and a long (32 bits). Each takes a port
number and a value, and writes that value onto the I/O bus at the given
port, nothing more. The width only determines how many bits are moved
in a single write and which sub-register the value comes from
internally; it doesn't change the underlying mechanism.

Electrically/logically, the write does not touch RAM at all. It is placed
directly on the I/O bus, and whichever device (real or emulated) has
registered itself as the listener for that port number is the one that
reacts to it. In an emulator like QEMU, "reacting" means the hypervisor
intercepts the instruction during CPU emulation and runs whatever
callback it associated with that port, there is no real hardware bus,
but the CPU-facing behavior is preserved.

The port number itself is always a 16-bit value, regardless of the
value's own width or the CPU's operating mode, because the I/O address
space is only 64 KB no matter whether the CPU is running in 16, 32, or
64-bit mode.

## `inb` / `inw` / `inl`

These are the mirror of the `out` functions, one per width again. Each
takes a port number and returns the value currently held by whatever
device answers on that port, instead of writing one. This is how the
kernel polls hardware state, for example, reading a byte a device has
placed in its output buffer, or reading a status register to know
whether it's safe to read or write next.

Which width to call depends entirely on what the target device expects;
using the wrong one either reads/writes more or fewer bits than the
device's register actually holds, and produces garbage or partial data.

## Privilege and access control

Executing `in`/`out` is not unconditionally allowed at every privilege
level. The CPU checks the current I/O privilege level (a field in the
flags register) and, for less-privileged code, an I/O permission bitmap
associated with the running task. Code running at the highest privilege
level (ring 0, where the kernel runs) is always allowed unrestricted
access to every port, with no bitmap check performed. This matters if the
kernel later grows a user mode: any user-level code that needs to touch
hardware directly would need this access explicitly granted, rather than
inheriting it automatically.

## Why this matters for the kernel

Almost every piece of legacy PC hardware the kernel talks to directly: 
keyboard, PIC, PIT, RTC, and QEMU's virtual ACPI device, is only
reachable through this port-based interface, not through memory-mapped
registers. Any driver or subsystem that touches such a device will
ultimately reduce to a handful of `in`/`out` calls on specific, well-known
port numbers. Understanding this mechanism once, at this level, avoids
having to re-explain the same read/write conventions in every subsystem's
own doc.
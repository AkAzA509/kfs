# I/O Ports ([io.c](../kernel/core/io.c))

## Overview

How the kernel talks to hardware devices through the x86 I/O address
space, and what the `in`/`out` instruction family actually does at the
hardware boundary.

## Mental model

An I/O port is a numbered endpoint exposed by a device for control or
status exchange. Unlike regular memory, ports do not live in the RAM
address space; they are accessed through dedicated CPU instructions.

The term "register" is broader: a device often exposes one or more
registers through its ports, and the CPU itself also has registers. Here
we care about the device-facing meaning: a port usually selects which
device register or action is being addressed.

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
number and a value, then writes that value to the I/O space at that
port, nothing more. The width only determines how many bits are moved
in a single write; it does not change the mechanism.

Electrically and logically, the write does not touch RAM. It is routed to
the I/O side of the machine, and whichever device (real or emulated)
listens on that port is the one that reacts. In QEMU, that reaction is
implemented by the emulator intercepting the instruction and calling the
device model associated with the port.

The port number itself is always a 16-bit value, regardless of the value
width or the CPU's operating mode, because the I/O address space is only
64 KB.

## `inb` / `inw` / `inl`

These are the mirror of the `out` functions, one per width again. Each
takes a port number and returns the value currently held by whatever
device answers on that port. This is how the kernel polls hardware state,
for example, reading a byte the keyboard controller placed in its output
buffer, or checking a status register before the next read or write.

Which width to call depends entirely on what the target device expects;
using the wrong one either reads or writes too many or too few bits, and
produces garbage or partial data.

## Privilege and access control

Executing `in`/`out` is not allowed at every privilege level. The CPU
checks the current I/O privilege level (a field in the flags register)
and, for less-privileged code, an I/O permission bitmap associated with
the running task. Code running at ring 0, where the kernel lives, is
allowed unrestricted access to every port. If the kernel later grows a
user mode, any user-level code that needs direct hardware access will
need that access granted explicitly.

## Why this matters for the kernel

Almost every piece of legacy PC hardware the kernel talks to directly,
including the keyboard, PIC, PIT, RTC, and QEMU's virtual ACPI device,
is only reachable through this port-based interface, not through
memory-mapped registers. Any driver that touches such a device will
ultimately reduce to a handful of `in`/`out` calls on specific, well-known
port numbers. Understanding the mechanism once avoids having to repeat
the same conventions in every subsystem's own doc.
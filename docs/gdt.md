# Global Descriptor Table ([gdt.c](../kernel/arch/i386/gdt.c))

## Overview

The GDT is a binary table, read directly by the CPU, that describes memory
segments and the privilege level required to use each of them. It is not
optional: entering and staying in 32-bit protected mode requires a valid
GDT loaded via `lgdt`, with `CS`/`SS`/`DS`/... holding valid selectors into
it, at all times.

This kernel uses the GDT in **flat model**: every segment covers the same
`0x00000000`-`0xFFFFFFFF` range. In this configuration segmentation does
**not** partition memory into separate zones, every segment sees the exact
same bytes. What differs between segments is only the **privilege level**
(kernel/ring 0 vs. user/ring 3) required to load them into a segment
register. Real memory isolation (stopping ring 3 from reading a specific
kernel address) is the job of paging, not covered by this module, see
[memory.md](memory.md).

```
CS, DS, SS, ES, FS, GS        (CPU segment registers, 6 of them)
        │  each holds a "selector" = index into the table below
        ▼
┌─────────────────────────────────────────────┐
│                     GDT                     │   7 x 8-byte entries,
│  [0] null                                   │   built once at boot,
│  [1] kernel code   (ring 0)                 │   never modified after
│  [2] kernel data   (ring 0)                 │   init_gdt() runs
│  [3] kernel stack  (ring 0)                 │
│  [4] user code     (ring 3)                 │
│  [5] user data     (ring 3)                 │
│  [6] user stack    (ring 3)                 │
└─────────────────────────────────────────────┘
        │  base = 0, limit = 4 GiB, for every single entry
        ▼
┌─────────────────────────────────────────────┐
│         physical memory, 0 to 4 GiB         │   same underlying zone
└─────────────────────────────────────────────┘   for all 7 entries
```

## Why flat model instead of real per-segment zones

An alternative design would give each segment its own non-overlapping
`base`/`limit` (e.g. kernel code at `0x00100000`, user code at
`0x00600000`...), making segmentation do actual memory partitioning. This
was **not** chosen, for reasons that go beyond this kernel's current scope
but are worth recording:

- Segment sizes would be fixed at descriptor-creation time. Any process
  needing more memory than its segment's limit has no clean way to grow
  without colliding with the next segment.
- Multiple simultaneous user processes would each need their own code/data
  descriptors (with the historical solution being a per-process LDT), which
  is both limited (GDT/LDT cap at 8192 entries) and expensive to swap on
  every context switch compared to reloading a single register (`CR3`).
- C assumes flat, universal pointers. A pointer's meaning would depend on
  which segment happens to be loaded when it's dereferenced, breaking the
  language's memory model.

Flat model keeps segmentation to the one thing it is still needed for in
protected mode: carrying **ring 0 vs. ring 3** as a hardware-enforced
property of `CS`/`SS`, everything finer-grained is deferred to paging.

## Core data structures

### `s_gdt_entry` one 8-byte descriptor

Intel's descriptor format predates 32-bit addressing and was extended
across CPU generations without ever being cleaned up, so `base` and
`limit` end up split into non-contiguous pieces instead of two plain
32-bit/20-bit fields:

| Field      | Size | Content                                                        |
|------------|------|-----------------------------------------------------------------|
| `limit_1`  | 16 bits | limit, bits 0-15                                              |
| `base_1`   | 16 bits | base, bits 0-15                                               |
| `base_2`   | 8 bits  | base, bits 16-23                                              |
| `access`   | 8 bits  | present / privilege / type, see [Access byte](#access-byte)   |
| `lim_attr` | 8 bits  | bits 0-3: limit, bits 16-19 · bits 4-7: flags (G, D/B, L, AVL) |
| `base_3`   | 8 bits  | base, bits 24-31                                              |

`__attribute__((packed))` is required, without it the compiler is free to
insert padding for alignment, which would break the exact 8-byte layout
the CPU expects.

### `s_gdt_addr` the GDTR operand

```c
struct s_gdt_addr {
	u16_t	limit;   // size of the table in bytes, minus 1
	u32_t	addr;    // linear address of the table
} __attribute__((packed));
```

Passed to `lgdt` (inside `update_gdt`, see [Loading the GDT into the
CPU](#loading-the-gdt-into-the-cpu)). This struct describes *where the
table is*, it is not itself an entry of the table.

## Access byte

Bits 1-2 change meaning depending on bit 3 (`E`, executable):

| Bit | Name  | Code segment (E=1)                          | Data segment (E=0)                          |
|-----|-------|----------------------------------------------|-----------------------------------------------|
| 7   | P     | Present, always 1 for a real segment          | same                                          |
| 6-5 | DPL   | Ring required to use this selector (0 or 3)   | same                                          |
| 4   | S     | 1 = code/data descriptor (as opposed to system, e.g. a future TSS) | same |
| 3   | E     | 1                                              | 0                                              |
| 2   | C/DC  | Conforming, kept at 0 (no privilege borrowing) | Direction, 0 = expand-up                     |
| 1   | R/W   | Readable, kept at 1                           | Writable, kept at 1                          |
| 0   | A     | Accessed, set to 0, the CPU updates it itself | same                                          |

The four access bytes actually used in this kernel:

| Value  | Segment       | P | DPL | S | E | C/DC | R/W | A |
|--------|---------------|---|-----|---|---|------|-----|---|
| `0x9A` | kernel code   | 1 | 00  | 1 | 1 | 0    | 1   | 0 |
| `0x92` | kernel data/stack | 1 | 00 | 1 | 0 | 0    | 1   | 0 |
| `0xFA` | user code     | 1 | 11  | 1 | 1 | 0    | 1   | 0 |
| `0xF2` | user data/stack   | 1 | 11 | 1 | 0 | 0    | 1   | 0 |

Kernel and user pairs differ only in `DPL`, code and data pairs only in
`E`, there is no dedicated "stack" type: a stack segment is a plain data
descriptor, reused as-is for the kernel stack and user stack entries.

## Building a descriptor: `create_gdt_entry`

```c
static void	create_gdt_entry(u8_t idx, u32_t base, u32_t limit, u8_t access, u8_t flags)
{
	gdt[idx].base_1 = base & 0xFFFF;
	gdt[idx].base_2 = (base >> 16) & 0xFF;
	gdt[idx].base_3 = (base >> 24) & 0xFF;

	gdt[idx].limit_1 = limit & 0xFFFF;
	gdt[idx].lim_attr = (limit >> 16) & 0xF;
	gdt[idx].lim_attr |= flags & 0xF0;

	gdt[idx].access = access;
}
```

`base` and `limit` arrive as plain 32/20-bit values and get sliced to
match the layout in [Core data structures](#core-data-structures): each
line either masks off the bits that belong in that field (`& 0xFFFF`,
`& 0xFF`) or shifts a higher slice down to bit 0 before masking it
(`>> 16 & 0xF`).

`flags` is passed as a single nibble occupying bits 4-7 (`0xC` for every
segment in this kernel: `G=1` page granularity, `D/B=1` 32-bit segment,
`L=0`, `AVL=0`). It is combined into `lim_attr` with `|=` rather than `=`,
because the low nibble of that byte was already written by the limit line
just above, `=` would overwrite it instead of merging.

## The seven entries

```c
create_gdt_entry(0, 0, 0,      0,    0);    // null descriptor
create_gdt_entry(1, 0, 0xFFFFF, 0x9A, 0xC); // kernel code
create_gdt_entry(2, 0, 0xFFFFF, 0x92, 0xC); // kernel data
create_gdt_entry(3, 0, 0xFFFFF, 0x92, 0xC); // kernel stack
create_gdt_entry(4, 0, 0xFFFFF, 0xFA, 0xC); // user code
create_gdt_entry(5, 0, 0xFFFFF, 0xF2, 0xC); // user data
create_gdt_entry(6, 0, 0xFFFFF, 0xF2, 0xC); // user stack
```

| Index | Selector | Purpose | base | limit | access |
|-------|----------|---------|------|-------|--------|
| 0 | `0x00` | mandatory null descriptor, never used by the CPU | - | - | - |
| 1 | `0x08` | kernel code | 0 | 4 GiB | `0x9A` |
| 2 | `0x10` | kernel data | 0 | 4 GiB | `0x92` |
| 3 | `0x18` | kernel stack | 0 | 4 GiB | `0x92` |
| 4 | `0x20` | user code | 0 | 4 GiB | `0xFA` |
| 5 | `0x28` | user data | 0 | 4 GiB | `0xF2` |
| 6 | `0x30` | user stack | 0 | 4 GiB | `0xF2` |

A selector is simply `index * 8` (the byte offset of that entry inside the
table), these are the exact values `update_gdt` needs to load into
`CS`/`DS`/`SS`/etc.

Kernel stack and user stack reuse the same access byte as kernel/user
data, `lim_attr` and `base` end up byte-for-byte identical to their data
counterparts. In flat model there is no separate "stack zone", the entry
only exists to give `SS` its own privilege-tagged selector, distinct from
`DS`, for the day `SS` needs to be reloaded independently (ring switch).

## Loading the GDT into the CPU

Two things a C function cannot express force part of this module into
assembly, in `update_gdt`:

- `lgdt` has no C equivalent, it is a raw instruction that loads the GDTR.
- `CS` cannot be changed with a plain `mov`, x86 requires a far jump that
  specifies the new selector and the new instruction pointer together.

```
init_gdt()                              [gdt.c]
  │  fills gdt[0..6], builds gdt_addr { limit, addr }
  ▼
update_gdt(&gdt)                        [assembly]
  │
  ├─ lgdt [gdt_addr]                    tells the CPU where the table is,
  │                                     nothing else happens yet
  │
  ├─ mov ax, 0x10                       kernel data selector
  ├─ mov ds, ax
  ├─ mov es, ax
  ├─ mov fs, ax
  ├─ mov gs, ax
  ├─ mov ax, 0x18
  ├─ mov ss, ax
  │
  ├─ jmp 0x08:done                      far jump: only way to change CS,
  │                                     loads selector 0x08 (kernel code)
  │
  ├─done:
  ├─ ret
  ▼
CS = 0x08, DS/ES/FS/GS = 0x10, SS = 0x18 GRUB's temporary GDT is now fully
                                         replaced, ESP itself never moves
```

Building the descriptor table does not, by itself, change anything, the
CPU keeps using GRUB's temporary GDT until `update_gdt` explicitly points
`GDTR` at the new one and reloads every segment register.

## Privilege checking: CPL / DPL / RPL

Every time a segment register is (re)loaded, and on every memory access
through it, the CPU compares:

- **CPL** current privilege level, the 2 low bits of whatever selector is
  currently in `CS`.
- **DPL** the privilege level baked into the target descriptor's access
  byte.
- **RPL** the 2 low bits of the selector being loaded (always 0 in this
  kernel, `TI`/`RPL` bits are not used).

```
max(CPL, RPL) <= DPL   →  allowed
max(CPL, RPL)  > DPL   →  General Protection Fault
```

With flat-model base/limit identical everywhere, this check is the
**only** thing segmentation still enforces: it decides whether a given
selector may be loaded at all from the current privilege level, not
whether a specific address may be touched. See [What the GDT does not
protect](#what-the-gdt-does-not-protect).

## What the GDT does not protect

Because every entry shares `base = 0, limit = 4 GiB`, the check above
never has anything to reject once a matching-privilege selector is
loaded. A ring 3 program running with `DS = 0x28` (user data, DPL 3) can
read or write **any** address in the 4 GiB range, including one the
kernel is actively using, `0x2000` for example, since nothing in the GDT
ties that specific address to ring 0.

Segmentation only gates *which selector a given CPL may load*, it has no
concept of per-address ownership. That granularity is paging's job: a
future `memory.md` module will mark individual 4 KiB pages as
supervisor-only, so a ring 3 access to such a page faults (`#PF`)
regardless of which segment selector was used to reach it. Until paging
is implemented, ring 0 and ring 3 share the entire address space with no
enforced isolation beyond whatever the current code chooses to do.

## Boot-time placement

The subject requires the table itself to live at physical address
`0x00000800`.

## Public API

| Function | File | Purpose |
|----------|------|---------|
| `init_gdt(void)` | `gdt.c` | Fill all 7 descriptors, call `update_gdt`, log to boot output |
| `update_gdt(u32_t gdt)` | assembly | `lgdt` + far jump + reload of every data/stack segment register |

Nothing outside `gdt.c` should need to touch `gdt[]` or `gdt_addr`
directly, both are `static`, `init_gdt()` is the only entry point.

## Future work

See [TODO.md](TODO.md) for the current list of planned improvements to
this module.
# How to use it?

Inside this `docs/` dir you will find all the code-related docs to
understand the kernel's behavior and design, one file per module/part.

```
docs
├── README.md         ← you are here
├── bootloader.md
├── display.md
├── memory.md
├── toolchain.md
├── TODO.md
└── ...
```

Each doc follows roughly the same shape: what the module does, why it's
built that way (not just what the code says), and diagrams where a picture
explains it faster than a paragraph.

## Index

| File             | Covers |
|-------------------|--------|
| `bootloader.md`   | The Multiboot entry point (`bootloader.s`): boot flow from GRUB to `kernel_main`, header layout, stack setup |
| `display.md`      | VGA/framebuffer terminal: driver dispatch, buffers, color, cursor, multi-screen switching |
| `memory.md`       | _Not written yet_ |
| `toolchain.md`    | Everything required to build the kernel: language, compiler, cross-toolchain, packages |
| `io.md`           | The io function out and in explanation (hardware level)
| `TODO.md`         | Known gaps and planned improvements, organized by module |

## Conventions

- One module = one file, named after the module (`displaying` →
  `display.md`).
- If a doc references another module, link it directly
  (`[TODO.md](TODO.md)`) instead of repeating the explanation.
- When a module's design changes, update its doc in the same change,
  stale docs are worse than no docs.

For a general presentation of the project (what it is, how to build and run
it), see the root [README.md](../README.md).
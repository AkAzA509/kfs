# kfs

## Overview
This project is a small kernel and bootloader setup for a custom x86 operating system. The repository contains the bootloader, the kernel entry point, and the build rules used to generate the final ISO image.

## Environment Setup
A cross-compilation toolchain is required. The following environment variables must be set before building:

```bash
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
export PATH="$HOME/opt/cross/bin:$PATH"
```

You also need the following tools and libraries installed on your system:

- GCC
- Make
- Bison
- Flex
- GMP
- MPFR
- MPC
- Texinfo
- ISL


Finally, you need the source archives for Binutils and GCC.

Download the needed source code into a suitable directory such as `$HOME/src`:

- You can download the desired Binutils release by visiting the [Binutils website](https://www.gnu.org/software/binutils/).

- You can download the desired GCC release by visiting the [GCC website](https://www.gnu.org/software/gcc/).

## Toolchain Build

### Binutils
```bash
# For Binutils
cd $HOME/src

mkdir build-binutils
cd build-binutils
../binutils-x.y.z/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror --enable-default-execstack=no
make
make install
```

- `--target=$TARGET` builds Binutils as a cross-toolchain for the target architecture defined by `TARGET`.
- `--prefix="$PREFIX"` installs the toolchain under the directory pointed to by `PREFIX`.
- `--with-sysroot` enables sysroot support and points the toolchain to an empty default root for target files.
- `--disable-nls` disables native language support, which reduces dependencies and keeps diagnostics in English.
- `--disable-werror` prevents warnings from being treated as errors during the build.
- `--enable-default-execstack=no` disables executable stacks by default for the generated objects and binaries.

### GCC
```bash
# For GCC cross compiling
cd $HOME/src

# The $PREFIX/bin dir _must_ be in the PATH. This cmd check that
which -- $TARGET-as || echo $TARGET-as is not in the PATH

mkdir build-gcc
cd build-gcc
../gcc-x.y.z/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers --enable-initfini-array
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
```

- `--target=$TARGET` configures GCC as a cross-compiler for the target architecture.
- `--prefix="$PREFIX"` installs GCC in the same toolchain directory as Binutils.
- `--disable-nls` disables native language support and keeps build output in English.
- `--enable-languages=c` builds only the C frontend, which is enough for this kernel project.
- `--without-headers` tells GCC not to depend on a target C library or runtime headers.
- `--enable-initfini-array` enables support for the modern `.init_array` and `.fini_array` initialization model.

### Compilation Flags
The kernel build uses the following compiler and linker flags:

- `-fno-builtin`: disables optimizations that replace standard library calls with compiler built-ins.
- `-fno-exceptions`: disables C++ exception handling support.
- `-fno-stack-protector`: disables stack protector and canary instrumentation.
- `-fno-rtti`: disables Run-Time Type Information generation for classes with virtual functions.
- `-nostdlib`: prevents the use of standard startup files and libraries during linking.
- `-nodefaultlibs`: prevents automatic linking against the default system libraries.
- `-T linker.ld` : The own kernel linking indication

## Multiboot

C'est quoi un header multiboot ?
Quand GRUB cherche ton kernel sur le disque, il lit les premiers 8Ko du fichier et cherche le nombre magique 0x1BADB002 for (multiboot 1). Quand il le trouve, il sait que c'est un kernel multiboot et il lit les 3 fields de 32-bit qui suivent :
```text
field 1 : le magic number  (pour que GRUB reconnaisse le kernel)
field 2 : les flags        (pour dire à GRUB ce qu'on veut)
field 3 : le checksum      (pour vérifier que c'est pas corrompu)
```

C'est quoi les flags ?
C'est un entier 32-bit où chaque bit est un interrupteur on/off :
```text
bit 0 = 1  →  aligne les modules sur des pages mémoire
bit 1 = 1  →  donne nous la memory map
bit 2 = 1  →  on veut choisir le mode vidéo (framebuffer !)
...
```
Quand le bit 2 est à 1
GRUB dit "ah il veut choisir le mode vidéo — donc juste après le checksum dans le header il doit y avoir des infos supplémentaires". Il s'attend à lire 8 fields de 32-bit supplémentaires dans l'ordre :
```text
field 4 : header_addr   (adresse du header, on met 0)
field 5 : load_addr     (où charger le kernel, on met 0)
field 6 : load_end_addr (fin du kernel, on met 0)
field 7 : bss_end_addr  (fin du bss, on met 0)
field 8 : entry_addr    (point d'entrée, on met 0)
field 9 : mode_type     (0=palette, 1=RGB, 2=texte)
field 10: width         (largeur en pixels)
field 11: height        (hauteur en pixels)
field 12: depth         (bits par pixel)
```

## Documentations and ressources
- Fundamental Concepts and Structure of the Linux Kernel (old version v2.4.20) [here](https://www.irif.fr/~carton/Enseignement/Architecture/Cours/Virtual/linux.pdf)
- Some real linux kernel docs [here](https://www.kernel.org/)
- The OSdev documentations and guide [here](https://wiki.osdev.org/Expanded_Main_Page)
- Writing a Tiny x86 Bootloader in assembly [here](https://www.joe-bergeron.com/posts/Writing%20a%20Tiny%20x86%20Bootloader/)
- Operating Systems: From 0 to 1 by tuhdo [here](https://github.com/tuhdo/os01/tree/master)
- The little book about OS development by Erik Helin, Adam Renberg [here](https://littleosbook.github.io/)
- Kernels 101 – Let’s write a Kernel by Arjun Sreedharan [here](https://arjunsreedharan.org/post/82710718100/kernels-101-lets-write-a-kernel)
- Kernels 201 - Let’s write a Kernel with keyboard and screen support [here](https://arjunsreedharan.org/post/99370248137/kernels-201-lets-write-a-kernel-with-keyboard)
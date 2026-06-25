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
# Toolchain build setup

## Overview
To build this kernel you need a cross-compiler that targets the i386
architecture (`i686-elf`). The steps below walk you through building one
from source.

> **Time estimate:** building Binutils + GCC from source takes roughly
> 20-40 minutes depending on your machine and number of CPU cores.

> **Prefer not to do this by hand?** A script that automates every step
> below is available at [`scripts/setup-toolchain.sh`](../scripts/setup-toolchain.sh).
> See the [Automated setup](#automated-setup) section at the bottom.

## Tested versions

This project has been built and tested against:

- **Binutils**: `2.46.1`
- **GCC**: `13.3.0`

Other versions may work, but Binutils/GCC compatibility isn't guaranteed
across every combination — if you hit build errors, try matching these
versions first before assuming your code is at fault.

## Install system dependencies

You need the following tools and libraries installed before building the
toolchain itself:

- GCC (your system's native compiler, used to build the cross-compiler)
- Make
- Bison
- Flex
- GMP
- MPFR
- MPC
- Texinfo
- ISL

**Debian / Ubuntu:**
```bash
sudo apt update
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
```

**Fedora:**
```bash
sudo dnf install gcc gcc-c++ make bison flex gmp-devel mpfr-devel libmpc-devel texinfo
```

**Arch Linux:**
```bash
sudo pacman -S base-devel gmp mpc mpfr texinfo
```

**macOS (Homebrew):**
```bash
brew install gmp mpfr libmpc texinfo
```

## Environment Setup

The following environment variables must be set before building:

```bash
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
```

> These `export` lines only apply to your current shell session. Add them
> to your `~/.bashrc` / `~/.zshrc` if you want them available every time
> you open a new terminal.

## Download sources

Download the source archives for Binutils and GCC into a suitable
directory such as `$HOME/src`:

```bash
mkdir -p $HOME/src && cd $HOME/src
wget https://ftp.gnu.org/gnu/binutils/binutils-2.46.1.tar.gz
wget https://ftp.gnu.org/gnu/gcc/gcc-13.3.0/gcc-13.3.0.tar.gz
tar -xzf binutils-2.46.1.tar.gz
tar -xzf gcc-13.3.0.tar.gz
```

(You can browse other releases on the [Binutils website](https://www.gnu.org/software/binutils/)
or the [GCC website](https://www.gnu.org/software/gcc/) if you want a
different version — see [Tested versions](#tested-versions) above first.)

## Toolchain Build

### Binutils
```bash
cd $HOME/src

mkdir build-binutils
cd build-binutils
../binutils-2.46.1/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror --enable-default-execstack=no
make
make install
```

- `--target=$TARGET` builds Binutils as a cross-toolchain for the target architecture defined by `TARGET`.
- `--prefix="$PREFIX"` installs the toolchain under the directory pointed to by `PREFIX`.
- `--with-sysroot` enables sysroot support and points the toolchain to an empty default root for target files.
- `--disable-nls` disables native language support, which reduces dependencies and keeps diagnostics in English.
- `--disable-werror` prevents warnings from being treated as errors during the build.
- `--enable-default-execstack=no` disables executable stacks by default for the generated objects and binary.

### GCC
```bash
cd $HOME/src

# The $PREFIX/bin dir _must_ be in the PATH. This cmd checks that.
which -- $TARGET-as || echo $TARGET-as is not in the PATH

mkdir build-gcc
cd build-gcc
../gcc-13.3.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers --enable-initfini-array
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

## Verify the installation

```bash
$TARGET-gcc --version
```

You should see output similar to:
```
i686-elf-gcc (GCC) 13.3.0
```

If this command isn't found, double-check that `$PREFIX/bin` is in your
`PATH` (see [Environment Setup](#environment-setup)).

## Troubleshooting

**`i686-elf-as: not found` / `which` check fails**
`$PREFIX/bin` isn't in your `PATH`, or the Binutils build didn't
`make install` successfully. Re-export the variables from
[Environment Setup](#environment-setup) and confirm `ls $PREFIX/bin`
shows `i686-elf-as`, `i686-elf-ld`, etc.

**GCC configure fails complaining about ISL / GMP / MPFR / MPC**
One of the required libraries is missing or too old. Re-run the
[system dependencies](#install-system-dependencies) install command for
your distro; on some systems you may need the `-dev`/`-devel` package
variant specifically (headers, not just the runtime library).

**`make all-gcc` fails partway through with an internal compiler error**
This is almost always a version mismatch between Binutils and GCC, or a
partially-completed previous build. Try `rm -rf build-gcc build-binutils`,
recreate them, and rebuild using the exact [tested versions](#tested-versions)
above.

**Build succeeds but `$TARGET-gcc --version` shows the wrong version**
You likely have a previous cross-compiler build under the same `$PREFIX`,
or your system's native GCC is shadowing it in `PATH`. Check `which
$TARGET-gcc` and confirm the path points into `$PREFIX/bin`.

## Compilation Flags
The kernel build uses the following compiler and linker flags:

- `-fno-builtin`: disables optimizations that replace standard library calls with compiler built-ins.
- `-fno-exceptions`: disables C++ exception handling support.
- `-fno-stack-protector`: disables stack protector and canary instrumentation.
- `-fno-rtti`: disables Run-Time Type Information generation for classes with virtual functions.
- `-nostdlib`: prevents the use of standard startup files and libraries during linking.
- `-nodefaultlibs`: prevents automatic linking against the default system libraries.
- `-T linker.ld`: the kernel's own linker script.

## Automated setup

Instead of running each step above by hand, you can use:

```bash
./scripts/setup-toolchain.sh
```

The script performs the dependency check, download, build, and
verification steps automatically. See the script's header comment for
options (custom `$PREFIX`, skipping the download step if archives are
already present, etc).
#!/bin/bash
#
# setup-toolchain.sh — builds the i686-elf cross-compiler needed for kfs.
#
# This automates the manual steps described in docs/toolchain.md:
#   1. check for required system dependencies
#   2. download Binutils + GCC sources (skipped if already present)
#   3. configure & build Binutils
#   4. configure & build GCC
#   5. verify the resulting cross-compiler works
#
# Usage:
#   ./scripts/setup-toolchain.sh
#
# Options (environment variables, all optional):
#   PREFIX        Install location for the toolchain (default: $HOME/opt/cross)
#   SRC_DIR       Where sources are downloaded/extracted (default: $HOME/src)
#   BINUTILS_VER  Binutils version to build (default: 2.46.1)
#   GCC_VER       GCC version to build (default: 13.3.0)
#   JOBS          Parallel make jobs (default: number of CPU cores)
#
# Example:
#   PREFIX=/opt/kfs-cross ./scripts/setup-toolchain.sh
#
set -euo pipefail

# ---- Configuration ---------------------------------------------------------

PREFIX="${PREFIX:-$HOME/opt/cross}"
SRC_DIR="${SRC_DIR:-$HOME/src}"
TARGET=i686-elf
BINUTILS_VER="${BINUTILS_VER:-2.46.1}"
GCC_VER="${GCC_VER:-13.3.0}"
JOBS="${JOBS:-$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)}"

export PREFIX
export TARGET
export PATH="$PREFIX/bin:$PATH"

BINUTILS_TARBALL="binutils-${BINUTILS_VER}.tar.gz"
GCC_TARBALL="gcc-${GCC_VER}.tar.gz"
BINUTILS_URL="https://ftp.gnu.org/gnu/binutils/${BINUTILS_TARBALL}"
GCC_URL="https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VER}/${GCC_TARBALL}"

log() { printf '\033[1;34m[toolchain]\033[0m %s\n' "$1"; }
ok() { printf '\033[1;32m[toolchain]\033[0m %s\n' "$1"; }
fail() { printf '\033[1;31m[toolchain]\033[0m %s\n' "$1" >&2; exit 1; }

# ---- Step 1: dependency check ----------------------------------------------

check_dependencies() {
	log "Checking required tools..."
	local missing=()
	for tool in gcc make bison flex wget tar; do
		command -v "$tool" >/dev/null 2>&1 || missing+=("$tool")
	done

	if [ ${#missing[@]} -ne 0 ]; then
	fail "Missing required tools: ${missing[*]}
Install them first, e.g.:
  Debian/Ubuntu: sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo wget
  Fedora:        sudo dnf install gcc gcc-c++ make bison flex gmp-devel mpfr-devel libmpc-devel texinfo wget
  Arch:          sudo pacman -S base-devel gmp mpc mpfr texinfo wget
  macOS:         brew install gmp mpfr libmpc texinfo wget"
	fi
	ok "All required tools found."
}

# ---- Step 2: download sources ----------------------------------------------

download_sources() {
	mkdir -p "$SRC_DIR"
	cd "$SRC_DIR"

	if [ ! -f "$BINUTILS_TARBALL" ]; then
		log "Downloading Binutils ${BINUTILS_VER}..."
		wget -q --show-progress "$BINUTILS_URL"
	else
		log "Binutils tarball already present, skipping download."
	fi

	if [ ! -f "$GCC_TARBALL" ]; then
		log "Downloading GCC ${GCC_VER}..."
		wget -q --show-progress "$GCC_URL"
	else
		log "GCC tarball already present, skipping download."
	fi

	[ -d "binutils-${BINUTILS_VER}" ] || tar -xzf "$BINUTILS_TARBALL"
	[ -d "gcc-${GCC_VER}" ] || tar -xzf "$GCC_TARBALL"
	ok "Sources ready in $SRC_DIR."
}

# ---- Step 3: build Binutils -------------------------------------------------

build_binutils() {
	if command -v "${TARGET}-ld" >/dev/null 2>&1; then
		log "${TARGET}-ld already installed, skipping Binutils build."
		return
	fi

	log "Building Binutils (this can take a while)..."
	cd "$SRC_DIR"
	mkdir -p build-binutils
	cd build-binutils
	../"binutils-${BINUTILS_VER}"/configure \
		--target="$TARGET" \
		--prefix="$PREFIX" \
		--with-sysroot \
		--disable-nls \
		--disable-werror \
		--enable-default-execstack=no
	make -j"$JOBS"
	make install
	ok "Binutils built and installed."
}

# ---- Step 4: build GCC ------------------------------------------------------

build_gcc() {
	if command -v "${TARGET}-gcc" >/dev/null 2>&1; then
		log "${TARGET}-gcc already installed, skipping GCC build."
		return
	fi

	command -v "${TARGET}-as" >/dev/null 2>&1 \
		|| fail "${TARGET}-as not found in PATH — Binutils build likely failed."

	log "Building GCC (this is the long part, ~15-30 minutes)..."
	cd "$SRC_DIR"
	mkdir -p build-gcc
	cd build-gcc
	../"gcc-${GCC_VER}"/configure \
		--target="$TARGET" \
		--prefix="$PREFIX" \
		--disable-nls \
		--enable-languages=c \
		--without-headers \
		--enable-initfini-array
	make -j"$JOBS" all-gcc
	make -j"$JOBS" all-target-libgcc
	make install-gcc
	make install-target-libgcc
	ok "GCC built and installed."
}

# ---- Step 5: verify ----------------------------------------------------------

verify() {
	log "Verifying installation..."
	if ! command -v "${TARGET}-gcc" >/dev/null 2>&1; then
		fail "${TARGET}-gcc not found after build. Check $PREFIX/bin manually."
	fi
	ok "$("${TARGET}-gcc" --version | head -n1)"
	ok "Toolchain ready. Add this to your shell rc file to persist it:
  export PREFIX=\"$PREFIX\"
  export TARGET=$TARGET
  export PATH=\"\$PREFIX/bin:\$PATH\""
}

# ---- Main --------------------------------------------------------------------

main() {
	log "Building i686-elf cross-compiler (Binutils $BINUTILS_VER, GCC $GCC_VER)"
	log "Install prefix: $PREFIX"
	check_dependencies
	download_sources
	build_binutils
	build_gcc
	verify
}

main "$@"
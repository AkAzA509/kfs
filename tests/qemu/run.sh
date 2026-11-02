#!/bin/sh

set -eu

image=$1
log_file=$2

rm -f "$log_file"
set +e
timeout 15s qemu-system-i386 \
	-cdrom "$image" \
	-serial "file:$log_file" \
	-display none \
	-no-reboot \
	-device isa-debug-exit,iobase=0x501,iosize=0x02
qemu_status=$?
set -e

if [ ! -f "$log_file" ]; then
	echo "QEMU produced no serial output" >&2
	exit 1
fi

cat "$log_file"

if grep -q '^TEST_RUN_COMPLETE PASS$' "$log_file"; then
	exit 0
fi

if grep -q '^TEST_FAIL ' "$log_file"; then
	echo "Kernel integration tests reported a failure" >&2
	exit 1
fi

if [ "$qemu_status" -eq 124 ]; then
	echo "Kernel integration tests timed out" >&2
else
	echo "Kernel integration tests stopped without completion (status $qemu_status)" >&2
fi
exit 1

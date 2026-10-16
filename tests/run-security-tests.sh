#!/bin/sh
set -u

qemu=$1
image=$2
log=$(mktemp)
trap 'rm -f "$log"' EXIT HUP INT TERM

set +e
timeout 10s "$qemu" \
    -m 16M \
    -drive "file=$image,format=raw,if=floppy" \
    -display none \
    -serial none \
    -monitor none \
    -no-reboot \
    -debugcon "file:$log" \
    -global isa-debugcon.iobase=0xe9 \
    -device isa-debug-exit,iobase=0xf4,iosize=0x04
qemu_status=$?
set -e

if [ "$qemu_status" -ne 33 ]; then
    printf 'FAIL: security test image exited with status %s.\n' "$qemu_status" >&2
    cat "$log" >&2
    exit 1
fi

if ! grep -Fq 'OS SECURITY TESTS PASS' "$log"; then
    printf 'FAIL: expected security-test evidence not found.\n' >&2
    cat "$log" >&2
    exit 1
fi

printf 'PASS: TSS, ring-3 isolation, page-fault recovery, and syscalls.\n'

#!/bin/sh

if ! ./build.sh; then
	exit 1
fi

cd kernel

HOST=i386
DEBUG_FLAGS="-gdb tcp::1234 -S"
FLAGS="-cdrom build-$HOST/pax_os.iso -serial stdio -no-reboot"

if [[ $1 == "debug" ]]; then
	FLAGS="$FLAGS $DEBUG_FLAGS"
fi

qemu-system-$HOST $FLAGS

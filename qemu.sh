#!/bin/sh

cd kernel

if ! meson compile -C build-i386; then
	exit 1
fi

HOST=i386
DEBUG_FLAGS="-s -S"
FLAGS="-cdrom build-i386/pax_os.iso -serial stdio"

if [[ $1 == "debug" ]]; then
	FLAGS="$FLAGS $DEBUG_FLAGS"
fi

qemu-system-$HOST $FLAGS

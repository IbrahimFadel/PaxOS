#!/bin/sh

cd kernel
meson compile -C build-i386

HOST=i386
DEBUG_FLAGS="-s -S"
FLAGS="-cdrom build-i386/pax_os.iso -serial stdio"

if [[ $1 == "debug" ]]; then
	FLAGS="$FLAGS $DEBUG_FLAGS"
fi

qemu-system-$HOST $FLAGS

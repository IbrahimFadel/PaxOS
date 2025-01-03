#!/bin/sh

HOST=i386
DEBUG_FLAGS="-s -S"
FLAGS="-cdrom pax_os.iso -serial stdio"

if [[ $1 == "debug" ]]; then
	FLAGS="$FLAGS $DEBUG_FLAGS"
fi

qemu-system-$HOST $FLAGS

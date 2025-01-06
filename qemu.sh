#!/bin/sh

if ! ./build.sh; then
	exit 1
fi

cd kernel

HOST=i386
DEBUG_FLAGS="-gdb tcp::1234 -S"
FLAGS="-cdrom build-$HOST/pax_os.iso -serial stdio"
# FLAGS="-machine virt -bios none -kernel build-$HOST/pax_os.elf -m 128M -smp 1 -nographic"

if [[ $1 == "debug" ]]; then
	FLAGS="$FLAGS $DEBUG_FLAGS"
fi

qemu-system-$HOST $FLAGS

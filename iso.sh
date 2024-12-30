#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/pax_os.kernel isodir/boot/pax_os.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "Pax OS" {
	multiboot2 /boot/pax_os.kernel
}
EOF
grub-mkrescue -o pax_os.iso isodir

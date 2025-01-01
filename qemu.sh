#!/bin/sh
set -e
. ./iso.sh

DEBUG_FLAGS="-s -S"
FLAGS="-cdrom pax_os.iso -serial stdio"

if [[ $1 == "debug" ]]; then
	FLAGS="$FLAGS $DEBUG_FLAGS"
fi

qemu-system-$(./target-triplet-to-arch.sh $HOST) $FLAGS

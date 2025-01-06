#!/bin/sh

cd kernel

meson install --no-rebuild --only-changed -C build-i386
meson compile -C build-i386

# Pax OS

Making a basic 32 bit OS targetting i386 (at least for now)

## Goals & Non-Goals

The goal of this project is to learn about OS design and implementation, not to build something useful, fast, or reliable. I don't really want to spend time making an efficient `libc` because that's not an OS concept, as long as it works, it works. If I have time later on, of course, I would love to do all that.

## Build Instructions

This project uses meson.

You will also need an `i386` cross compiler toolchain (binutils and gcc), nasm, grub, and python.

```
cd kernel
meson setup --cross-file ../i386-elf.txt build-i386
meson compile -C build-i386
```

The kernel depends on a custom `libc` implementation which needs to be installed into `sysroot` before building. It is likely that the first time compiling will generate an error due to `libc` not being in the `sysroot` directory. If that happens, build and install `libc` manually. Builds after that should be fine. This is just until I figure out how to get meson to install the subproject before building.

## Emulating

I've only tested the kernel with qemu, but I'm sure it would work with another emulator provided you go through installation and whatever configuration they need. If you built the cross compiler toolchain you should have an `i386` compatible `qemu`. Running `qemu.sh` should build the kernel, generate the iso, and open the emulator with the iso.

## Debugging

```sh
./qemu.sh debug # run qemu in debug mode
i386-elf-gdb kernel/build-i386/isodir/boot/pax_os.elf
```

the `.gdbinit` config should put you into a remote target on startup.

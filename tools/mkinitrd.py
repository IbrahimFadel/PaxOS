#!/usr/bin/env python3

import sys
import os
import struct

MAGIC = 0xC0FFEE
HEADER_FMT = "<II"
HEADER_SIZE = struct.calcsize(HEADER_FMT)
ENTRY_FMT = "<64sII"
ENTRY_SIZE = struct.calcsize(ENTRY_FMT)


def pack(output, inputs):
    files = []
    for path in inputs:
        with open(path, "rb") as f:
            data = f.read()
        name = os.path.basename(path).encode()[:63]
        files.append((name, data))

    data_start = HEADER_SIZE + ENTRY_SIZE * len(files)
    offset = data_start
    entries = []
    for name, data in files:
        entries.append((name, offset, len(data)))
        offset += len(data)

    with open(output, "wb") as out:
        out.write(struct.pack(HEADER_FMT, MAGIC, len(files)))

        for name, offset, size in entries:
            out.write(struct.pack(ENTRY_FMT, name, offset, size))

        for _, data in files:
            out.write(data)

    total = os.path.getsize(output)
    print(f"mkinitrd: wrote {output} ({len(files)} files, {total} bytes)")
    for name, offset, size in entries:
        print(f"  {name.decode():<32} offset=0x{offset:x} size={size}")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"usage: {sys.argv[0]} output.initrd file1.elf [file2.elf ...]")
        sys.exit(1)
    pack(sys.argv[1], sys.argv[2:])

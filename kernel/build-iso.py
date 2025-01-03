import sys
import os
import shutil
import subprocess

if __name__ == '__main__':
    [grub_cfg, kernel_elf] = sys.argv[1:3]
    if not os.path.exists("isodir"):
        os.mkdir("isodir")
    shutil.copy2(grub_cfg, "isodir/boot/grub")
    shutil.copy2(kernel_elf, "isodir/boot")
    print("copied grub files")
    subprocess.run(
        ["grub-mkrescue", "-o", "pax_os.iso", "isodir"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.STDOUT,
        check=True
    )
    print("generated iso")

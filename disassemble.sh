#!/bin/bash

objdump -M intel --line-numbers --source --show-all-symbols --disassemble-all kernel/build-i386/pax_os.elf > dis

#!/bin/bash

set -e

echo "=== TEST ELF_READER : empty ==="

arm-none-eabi-gcc -c files/empty.s -o empty.o

../../elf_reader/elf_reader -h -S -s -r empty.o
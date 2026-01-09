#!/bin/bash

set -e

echo "=== TEST ELF_READER : symbols ==="

arm-none-eabi-gcc -c files/symbols.s -o symbols.o

../../elf_reader/elf_reader -h -S -s -r symbols.o

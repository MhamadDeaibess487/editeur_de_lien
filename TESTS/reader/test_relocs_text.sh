#!/bin/bash

set -e

echo "=== TEST ELF_READER : relocs_text ==="

arm-none-eabi-gcc -c files/relocs_text.s -o relocs_text.o

../../elf_reader/elf_reader -h -S -s -r relocs_text.o
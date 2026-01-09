#!/bin/bash

set -e

echo "=== TEST ELF_READER : text_only ==="

arm-none-eabi-gcc -c files/text_only.s -o text_only.o

../../elf_reader/elf_reader -h -S -s -r text_only.o

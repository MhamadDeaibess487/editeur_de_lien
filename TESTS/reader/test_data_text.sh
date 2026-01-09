#!/bin/bash

set -e

echo "=== TEST ELF_READER : data_text ==="

arm-none-eabi-gcc -c files/data_text.s -o data_text.o

../../elf_reader/elf_reader -h -S -s -r data_text.o

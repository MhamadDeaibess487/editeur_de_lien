#!/bin/bash

set -e

echo "=== TEST ELF_READER : data_text ==="

../../elf_reader/elf_reader -h -S -x 4 -s -r exemple1.o

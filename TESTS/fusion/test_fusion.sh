#!/bin/bash

echo "=== FUSION LOGIC TEST ==="

# System reference (real ELF)
arm-none-eabi-ld -EB -r file1.o file2.o -o system_fused.o

echo "[1] readelf on system fused (reference)"
readelf -h -S -s -r system_fused.o

echo
echo "[2] our elf_fusion output (text)"
../../elf_fusion/elf_fusion file1.o file2.o

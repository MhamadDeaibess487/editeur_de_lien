#!/bin/bash

echo "=== ELF FUSION SIMPLE TEST ==="

# Référence
arm-none-eabi-ld -EB -r file1.o file2.o -o ref.o

# Ton programme
../../elf_fusion/elf_fusion file1.o file2.o out.o

echo
echo "=== readelf -h -S -s -r : reference ==="
readelf -h -S -s -r ref.o > ref.txt

echo "=== readelf -h -S -s -r : elf_fusion ==="
readelf -h -S -s -r out.o > out.txt

echo
echo "=== DIFF (readelf output) ==="
diff -u ref.txt out.txt || true

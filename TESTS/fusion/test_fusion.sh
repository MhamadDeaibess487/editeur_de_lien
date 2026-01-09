#!/bin/bash

echo "=== ELF FUSION TEST ==="

# 1. Référence officielle
arm-none-eabi-ld -EB -r file1.o file2.o -o ref.o
if [ $? -ne 0 ]; then
    echo "Error: arm-none-eabi-ld failed"
    exit 1
fi

# 2. Notre fusion
../../elf_fusion/elf_fusion file1.o file2.o out.o
if [ $? -ne 0 ]; then
    echo "Error: elf_fusion failed"
    exit 1
fi

# 3. Comparaison binaire
echo
echo "[1] Binary comparison (cmp)"
if cmp -s ref.o out.o; then
    echo "✔ Binary files are identical"
else
    echo "✘ Binary files differ"
fi

# 4. Comparaison structurelle
echo
echo "[2] Structural comparison (readelf)"

readelf -h -S -s -r ref.o > ref.txt
readelf -h -S -s -r out.o > out.txt

if diff -u ref.txt out.txt > diff.txt; then
    echo "✔ ELF structure identical (readelf)"
else
    echo "✘ Differences found (see diff.txt)"
fi

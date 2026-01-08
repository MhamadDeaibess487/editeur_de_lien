#!/bin/bash

echo "=== ELF WRITER FINAL TEST (ELF32 ARM) ==="

# Nettoyage
rm -f system_fused.o our_fused.o system_readelf.txt our_readelf.txt

# 1) Référence système
echo "[1] Generating reference ELF using arm-none-eabi-ld"
arm-none-eabi-ld -EB -r file1.o file2.o -o system_fused.o

if [ $? -ne 0 ]; then
    echo "❌ system linker failed"
    exit 1
fi

# 2) Notre outil (fusion + écriture)
echo "[2] Generating ELF using our elf_fusion + elf_writer"
../../elf_fusion/elf_fusion file1.o file2.o our_fused.o

if [ $? -ne 0 ]; then
    echo "❌ our elf fusion/writer failed"
    exit 1
fi

# 3) readelf sur la référence
echo "[3] readelf on system ELF"
readelf -h system_fused.o  > system_readelf.txt
readelf -S system_fused.o >> system_readelf.txt
readelf -s system_fused.o >> system_readelf.txt
readelf -r system_fused.o >> system_readelf.txt

# 4) readelf sur notre ELF
echo "[4] readelf on our ELF"
readelf -h our_fused.o  > our_readelf.txt
readelf -S our_fused.o >> our_readelf.txt
readelf -s our_fused.o >> our_readelf.txt
readelf -r our_fused.o >> our_readelf.txt

# 5) Comparaison
echo "[5] diff results"
diff -u system_readelf.txt our_readelf.txt

if [ $? -eq 0 ]; then
    echo "✅ SUCCESS: ELF writer output matches system linker"
else
    echo "⚠️  DIFFERENCES FOUND (inspect manually)"
fi

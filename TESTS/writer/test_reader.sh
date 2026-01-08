#!/bin/bash
set -e

ARM=arm-none-eabi
READER=../../elf_reader/elf_reader

mkdir -p obj ref out

for src in asm/*.s; do
    base=$(basename "$src" .s)

    echo "=== Testing $base ==="

    # Assemble
    $ARM-as "$src" -o "obj/$base.o"

    # Reference output
    readelf -h "obj/$base.o" > "ref/$base.sec"
    readelf -S "obj/$base.o" >> "ref/$base.sec"
    readelf -s "obj/$base.o" >> "ref/$base.sec"
    readelf -r "obj/$base.o" >> "ref/$base.sec"

    # Our reader output
    $READER -h "obj/$base.o" > "out/$base.sec"
    $READER -S "obj/$base.o" >> "out/$base.sec"
    $READER -s "obj/$base.o" >> "out/$base.sec"
    $READER -r "obj/$base.o" >> "out/$base.sec"

    # Compare
    diff "out/$base.sec" "ref/$base.sec"

    echo "✔ $base OK"
done

echo "All reader tests PASSED"

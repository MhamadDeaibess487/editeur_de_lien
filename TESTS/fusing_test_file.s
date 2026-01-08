#!/bin/bash

arm-none-eabi-ld -EB -r file1.o file2.o -o ../elf_reader/systemfilefused.o
./elf_fusion file1.o file2.o > ../elf_reader/ourfilefused.o

cd ../elf_reader

readelf -h systemfilefused.o > systemfilefused.sec
readelf -S systemfilefused.o >> systemfilefused.sec
readelf -s systemfilefused.o >> systemfilefused.sec
readelf -r systemfilefused.o >> systemfilefused.sec

./elf_reader -h ../TESTS/ourfilefused.o > ourfilefused.sec
./elf_reader -S ../TESTS/ourfilefused.o >> ourfilefused.sec
./elf_reader -s ../TESTS/ourfilefused.o >> ourfilefused.sec
./elf_reader -r ../TESTS/ourfilefused.o >> ourfilefused.sec
diff ourfilefused.sec systemfilefused.sec
#!/bin/bash

# this test compiles a simple C file and compares the output of readelf and our elf_reader, 
#ultimately it will fail because our elf_reader does not yet support all features of ELF files generated from C code

arm-none-eabi-gcc -c a.c

readelf -h a.o > systemread.sec
./elf_reader -h ../TESTS/a.o > ourread.sec

readelf -S a.o >> systemread.sec
./elf_reader -S ../TESTS/a.o >> ourread.sec

readelf -s a.o >> systemread.sec
./elf_reader -s ../TESTS/a.o >> ourread.sec

readelf -r a.o >> systemread.sec
./elf_reader -r ../TESTS/a.o >> ourread.sec


diff ours.sec ref.sec



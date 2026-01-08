#!/bin/bash

# this test compiles a simple assembly file and compares the output of readelf and our elf_reader
# it will successfully pass as our elf_reader supports all features used in this simple assembly file

readelf -h exemple2.o > systemread.sec
./elf_reader -h ../TESTS/exemple2.o > ourread.sec

readelf -S exemple2.o >> systemread.sec
./elf_reader -S ../TESTS/exemple2.o >> ourread.sec

readelf -s exemple2.o >> systemread.sec
./elf_reader -s ../TESTS/exemple2.o >> ourread.sec

readelf -r exemple2.o >> systemread.sec
./elf_reader -r ../TESTS/exemple2.o >> ourread.sec


diff ours.sec ref.sec



CC=clang -g
CFLAGS=-Wall -Werror 

.PHONY:clean all

all: elf_writer

elf_writer: elf_writer.o
	$(CC) $(CFLAGS) $^ -o $@
 
#elf_writer.o: elf_writer.c elf_writer.h
#	$(CC) $(CFLAGS) -c elf_writer.c -o elf_writer.o



clean:
	rm -f elf_writer elf_writer.o
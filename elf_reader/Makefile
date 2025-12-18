CC=clang
CFLAGS=-Wall -Werror

.PHONY:clean all

all: elf_reader

elf_reader: header_reader.o header_section_reader.o util.o elf_reader.o section_reader.o
	$(CC) $(CFLAGS) $^ -o $@
 







clean: 
	rm -f *.o
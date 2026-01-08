CC=clang -g
CFLAGS=-Wall -Werror 

.PHONY:clean all

all: elf_reader

elf_reader: header_reader.o header_section_reader.o util.o elf_reader.o section_reader.o symbole_table.o relocation_reader.o
	$(CC) $(CFLAGS) $^ -o $@
 







clean:
	rm -f header_reader.o header_section_reader.o util.o elf_reader.o section_reader.o relocation_reader.o symbole_table.o elf_reader

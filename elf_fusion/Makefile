CC = clang -Wall -g 

all: elf_fusion

######################################################################
#                       Règles de compilation                        #
######################################################################

%.o: %.c
	$(CC) -c $<

header_reader.o: ../elf_reader/header_reader.c ../elf_reader/header_reader.h ../elf_reader/elf.h ../elf_reader/util.h
	$(CC) -c ../elf_reader/header_reader.c -o $@

header_section_reader.o: ../elf_reader/header_section_reader.c ../elf_reader/header_section_reader.h ../elf_reader/elf.h ../elf_reader/util.h
	$(CC) -c ../elf_reader/header_section_reader.c -o $@

section_reader.o: ../elf_reader/section_reader.c ../elf_reader/elf.h ../elf_reader/header_section_reader.h ../elf_reader/util.h
	$(CC) -c ../elf_reader/section_reader.c -o $@

symbol_reader.o: ../elf_reader/symbole_table.c ../elf_reader/elf.h ../elf_reader/header_section_reader.h ../elf_reader/util.h
	$(CC) -c ../elf_reader/symbole_table.c -o $@

relocation_reader.o: ../elf_reader/relocation_reader.c ../elf_reader/elf.h ../elf_reader/header_section_reader.h ../elf_reader/util.h
	$(CC) -c ../elf_reader/relocation_reader.c -o $@

elf_fusion.o: elf_fusion.c elf_reader_headers.h section_fusion.h symbole_fusion.h

section_fusion.o: section_fusion.c section_fusion.h ../elf_reader/elf.h ../elf_reader/header_section_reader.h

symbole_fussion.o: symbole_fusion.c symbole_fusion.h ../elf_reader/elf.h ../elf_reader/header_section_reader.h ../elf_reader/symbole_table.h

relocation_fusion.o: relocation_fusion.c relocation_fusion.h ../elf_reader/elf.h ../elf_reader/header_section_reader.h ../elf_reader/relocation_reader.h 

elf_writer.o: elf_writer.c 

util.o: ../elf_reader/util.c ../elf_reader/util.h ../elf_reader/elf.h
	$(CC) -c ../elf_reader/util.c -o $@

######################################################################
#                       Règles d'édition de liens                    #
######################################################################

elf_fusion: elf_fusion.o header_reader.o header_section_reader.o section_reader.o symbol_reader.o relocation_reader.o section_fusion.o util.o symbole_fusion.o relocation_fusion.o elf_writer.o
	$(CC) $^ -o $@

clean:
#	rm -f elf_fusion *.o 
	rm -f elf_fusion elf_fusion.o header_reader.o header_section_reader.o section_reader.o symbol_reader.o relocation_reader.o section_fusion.o util.o symbole_fusion.o relocation_fusion.o elf_writer.o

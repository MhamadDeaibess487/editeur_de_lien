#ifndef ELF_WRITER_H
#define ELF_WRITER_H
#include "../elf_reader/elf.h"
#include <stdio.h>
#include "../elf_fusion/section_fusion.h"

int write_elf_header(FILE *stream, Elf32_Ehdr *hdr);
int write_elf_section_header(FILE *stream, Elf32_Shdr *shdr, Elf32_Off e_shoff, Elf32_Half e_shnum);
int write_elf_shstrtab(FILE *stream, char* shstrtab, Elf32_Off offset, Elf32_Half size);
//int write_section(FILE *stream, Elf32_Shdr *shdr, void *section_data, int section_index)

#endif
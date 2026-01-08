#ifndef _RELOCATION_READER_H_
#define _RELOCATION_READER_H_
#include <stdio.h>

#include "elf.h"


//Read the rel or rela sections and put them into the 2 tables                          this                and this
int read_relocations(FILE *stream, Elf32_Ehdr *hdr, Elf32_Shdr *Shdr, char *shstrtab, Elf32_Rela **rela, Elf32_Rel **rel);

//Read the file for relocation sections and display them
void print_relocations(FILE *stream, Elf32_Shdr *Shdr,Elf32_Ehdr *hdr, char *shstrtab, Elf32_Sym* symtab, char* strtab);

void print_one_reloc_line(Elf32_Rel r, Elf32_Shdr *Shdr, Elf32_Ehdr *hdr,
                                 char *shstrtab, Elf32_Sym *symtab, char *strtab);
#endif
#ifndef _SYMBOLE_TABLE_H_
#define _SYMBOLE_TABLE_H_
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "elf.h"
#include <assert.h>
#include "util.h"
#include "header_section_reader.h"



//Read the symbol table and the associated strtab then pass it back 
Elf32_Sym* read_symbole_table(FILE *stream,Elf32_Shdr *Shdr,Elf32_Ehdr *hdr, char *shstrtab, char** strtab);

void swap_Elf32_Sym(Elf32_Sym *s);

//Read and print the symbol table
void print_symbole_table(Elf32_Shdr *Shdr,Elf32_Ehdr *hdr, char *shstrtab, Elf32_Sym* symtab, char* strtab);


#endif
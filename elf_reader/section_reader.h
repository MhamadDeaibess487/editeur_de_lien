#ifndef __SECTION_READER__
#define __SECTION_READER__

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "elf.h"
#include <assert.h>
#include "util.h"

//read the sections 
//void readSection(FILE *stream, Elf32_Ehdr *hdr, Elf32_Shdr **Shdr, Elf32_Sec *S,int i);


void print_section(FILE* stream, int section_index, Elf32_Shdr * Shdr);

#endif
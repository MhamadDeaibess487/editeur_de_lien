#ifndef __HEADER_SECTION_HEADER__
#define __HEADER_SECTION_HEADER__

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "elf.h"
#include <assert.h>
#include "util.h"

//read an array of section headers
void readShtable(FILE *stream, Elf32_Ehdr *hdr, Elf32_Shdr **Shdr);

//display said array
void printShtable(Elf32_Shdr* shtable, Elf32_Ehdr* hdr,FILE *stream);

#endif
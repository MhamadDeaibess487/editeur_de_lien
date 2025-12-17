#ifndef __ELF_HEADER_READER__
#define __ELF_HEADER_READER__

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "units_elf32.h"
#include "elf.h"

Elf32_Ehdr* ELF_header_reader(FILE *stream);

void print_header(Elf32_Ehdr *header);

#endif
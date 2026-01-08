#ifndef __SYMBOL_FUSION__
#define __SYMBOL_FUSION__
#include "elf_reader_headers.h"
#include "../elf_reader/symbole_table.h"
#include "section_fusion.h"



//function that maps one single symbol according to the section fusion context
void map_one_symbol(Elf32_Sym *s, SectionFusionContext *fusion);

//function that remaps an index according to the section fusion context
Elf32_Half fusion_remap_index(SectionFusionContext *fusion_context, int source);


//function that fuses 2 symbole tables according to the section fusion context 
Elf32_Sym *fusion_symbole(Elf32_Sym *symtab1,Elf32_Sym *symtab2,Elf32_Ehdr hdr1, Elf32_Ehdr hdr2, Elf32_Shdr **section_header_table1, Elf32_Shdr *section_header_table2, char* strtab1, char* strtab2,char ** new_strtab,SectionFusionContext *fusion_context,int** sym_map1, int** sym_map2);
#endif
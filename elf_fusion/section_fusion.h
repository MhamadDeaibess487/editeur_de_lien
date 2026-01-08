#ifndef __SECTION_FUSION__
#define __SECTION_FUSION__

#include <stdio.h>
#include "elf_reader_headers.h"

typedef struct {
    int src_index;        // original section index (file 2)
    int dst_index;        // section index in output ELF
    uint32_t add_offset;  // offset added due to concatenation 
} SectionMapEntry;

typedef struct {
    SectionMapEntry *map;
    size_t map_count;
    uint32_t *merged_sizes;   // indexed by output section index
    size_t merged_count;
} SectionFusionContext;

SectionFusionContext *fusion_init(size_t max_sections);

void fusion_register_file1_sections(Elf32_Ehdr *hdr1, Elf32_Shdr *sht1, SectionFusionContext *fu);

void fusion_process_file2_sections(Elf32_Ehdr *hdr1, Elf32_Shdr *sht1, char *shstr1, Elf32_Ehdr *hdr2, Elf32_Shdr *sht2, char *shstr2, SectionFusionContext *fu);


SectionFusionContext *fusion_build_sections(FILE *f1, FILE *f2,Elf32_Ehdr *hdr1, Elf32_Ehdr *hdr2,Elf32_Shdr **sht1, Elf32_Shdr *sht2, char **shstr1, char *shstr2);


#endif



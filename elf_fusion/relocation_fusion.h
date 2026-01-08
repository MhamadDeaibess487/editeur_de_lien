#ifndef __RELOCATION_FUSION__
#define __RELOCATION_FUSION__
#include "elf_reader_headers.h"
#include "../elf_reader/relocation_reader.h"
#include "section_fusion.h"

typedef struct {
    Elf32_Rel *rel;
    int des_index;
    int size;

} Rel_table;

typedef struct {
    Elf32_Rela *rela;
    int des_index;
    int size;

} Rela_table;
Elf32_Half fusion_context_source(SectionFusionContext fusion_context, int dest);

uint32_t get_add_offset(SectionFusionContext *fu, int src_sec);

Elf32_Rel read_rel(FILE *f, Elf32_Shdr *sh, int idx);

Elf32_Rela read_rela(FILE *f, Elf32_Shdr *sh, int idx);

Elf32_Rel *fusion_relocation_rel_sections(FILE *f1, FILE *f2, int section_rel1_index, int section_rel2_index, Elf32_Shdr *sht1, Elf32_Shdr *sht2, SectionFusionContext *fusion,int*sym_map1, int *sym_map2, int *out_count);

Elf32_Rela *fusion_relocation_rela_sections(FILE *f1, FILE *f2, int section_rela1_index, int section_rela2_index, Elf32_Shdr *sht1, Elf32_Shdr *sht2, SectionFusionContext *fusion,Elf32_Sym *fused_symtab,int*sym_map1, int *sym_map2, int *out_count);

Rel_table* fusion_relocation_rel (FILE *f1, FILE *f2, Elf32_Ehdr *hdr1, Elf32_Word hdr1_shnum_old, Elf32_Ehdr *hdr2, Elf32_Shdr *sht1, Elf32_Shdr *sht2, SectionFusionContext *fusion, Elf32_Sym *fused_symtab,int*sym_map1, int *sym_map2,int* rel_tab_size);

Rela_table* fusion_relocation_rela (FILE *f1, FILE *f2, Elf32_Ehdr *hdr1, Elf32_Word hdr1_shnum_old, Elf32_Ehdr *hdr2, Elf32_Shdr *sht1, Elf32_Shdr *sht2, SectionFusionContext *fusion, Elf32_Sym *fused_symtab,int*sym_map1, int *sym_map2,int* rela_tab_size);

#endif
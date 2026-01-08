#include <stdio.h>
#include "relocation_fusion.h"

//function that reads one relocation entry without addend

//function that handles the addend


/*if(Shdr1[i].sh_type == SHT_RELA){
                    Elf32_Half sym_index = ELF32_R_SYM(new_reltab[index_new_reltab].r_info);
                    Elf32_Sym sym = fused_symtab[sym_index];  

                    if(ELF32_ST_TYPE(fused_symtab[sym_index].st_info) == STT_SECTION){
                        if(type== R_ARM_ABS32 ||
                        type== R_ARM_ABS16 ||
                        type== R_ARM_ABS8 ||
                        type== R_ARM_ABS12 ||
                        type== R_ARM_ABS32_NOI ){
                            //new_reltab[index_new_reltab].r_addend += 
                            
                        }else if(type== R_ARM_CALL ||
                        type== R_ARM_JUMP24 ){
                            //new_reltab[index_new_reltab].r_addend += 
                        }
                    }
                }*/

uint32_t get_add_offset(SectionFusionContext *fu, int src_sec)
{
    for (size_t i = 0; i < fu->map_count; i++) {
        if (fu->map[i].src_index == src_sec)
            return fu->map[i].add_offset;
    }
    return 0;
}

Elf32_Half fusion_context_source(SectionFusionContext fusion_context, int dest){
    for (size_t i = 0; i < fusion_context.map_count; i++) {
        if (fusion_context.map[i].dst_index == dest)
            return fusion_context.map[i].src_index;
    }
    return 0;
}

Elf32_Rel read_rel(FILE *f, Elf32_Shdr *sh, int idx)
{
    Elf32_Rel r;
    fseek(f, sh->sh_offset + idx * sh->sh_entsize, SEEK_SET);
    fread(&r, sizeof(Elf32_Rel), 1, f);

    if (!is_big_endian()) {
        r.r_offset = reverse_4(r.r_offset);
        r.r_info   = reverse_4(r.r_info);
    }
    return r;
}

Elf32_Rela read_rela(FILE *f, Elf32_Shdr *sh, int idx)
{
    Elf32_Rela r;
    fseek(f, sh->sh_offset + idx * sh->sh_entsize, SEEK_SET);
    fread(&r, sizeof(Elf32_Rela), 1, f);

    if (!is_big_endian()) {
        r.r_offset = reverse_4(r.r_offset);
        r.r_info   = reverse_4(r.r_info);
        r.r_addend = reverse_4(r.r_addend);
    }
    return r;
}


Elf32_Rel *fusion_relocation_rel_sections(FILE *f1, FILE *f2, int section_rel1_index, int section_rel2_index, Elf32_Shdr *sht1, Elf32_Shdr *sht2, SectionFusionContext *fusion,int*sym_map1, int *sym_map2, int *out_count){
    size_t total = 0;
    if (sht1[section_rel1_index].sh_type == SHT_REL){
    total += sht1[section_rel1_index].sh_size / sht1[section_rel1_index].sh_entsize;}

    if (sht2[section_rel2_index].sh_type == SHT_REL){
    total += sht2[section_rel2_index].sh_size / sht2[section_rel2_index].sh_entsize;}

    Elf32_Rel *out = malloc(total * sizeof(Elf32_Rel));
    size_t idx = 0;

        if (sht1[section_rel1_index].sh_type == SHT_REL){
            printf("id 1 is rel \n ");
            int n = sht1[section_rel1_index].sh_size / sht1[section_rel1_index].sh_entsize;
            if (sht2[section_rel2_index].sh_type == SHT_REL){
                n-= sht2[section_rel2_index].sh_size / sht2[section_rel2_index].sh_entsize;
            }
            printf("found %d rel in 1\n",n);
            for (int j = 0; j < n; j++) {
                out[idx++] = read_rel(f1, &sht1[section_rel1_index], j);
            }
        }
        if (sht2[section_rel2_index].sh_type == SHT_REL){
            printf("id 2 is rel\n");
            int src_sec = sht2[section_rel2_index].sh_info;
            uint32_t add_offset = get_add_offset(fusion, src_sec);

            int n = sht2[section_rel2_index].sh_size / sht2[section_rel2_index].sh_entsize;
            for (int j = 0; j < n; j++) {
                Elf32_Rel r = read_rel(f2, &sht2[section_rel2_index], j);

                r.r_offset += add_offset;

                uint32_t sym  = ELF32_R_SYM(r.r_info);
                uint32_t type = ELF32_R_TYPE(r.r_info);

                if (sym_map2[sym] < 0) {
                    fprintf(stderr, "Unmapped symbol %u in relocation\n", sym);
                    exit(1);
                }

                r.r_info = ELF32_R_INFO(sym_map2[sym], type);

                out[idx++] = r;
            }
        }
    
    printf("total rel found %zu\n",idx);
    *out_count = idx;
    return out;
}


Elf32_Rela *fusion_relocation_rela_sections(FILE *f1, FILE *f2, int section_rela1_index, int section_rela2_index, Elf32_Shdr *sht1, Elf32_Shdr *sht2, SectionFusionContext *fusion,Elf32_Sym *fused_symtab,int*sym_map1, int *sym_map2, int *out_count){
    size_t total = 0;
    if (sht1[section_rela1_index].sh_type == SHT_RELA){
    total += sht1[section_rela1_index].sh_size / sht1[section_rela1_index].sh_entsize;}

    if (sht2[section_rela2_index].sh_type == SHT_RELA){
    total += sht2[section_rela2_index].sh_size / sht2[section_rela2_index].sh_entsize;}

    Elf32_Rela *out = malloc(total * sizeof(Elf32_Rela));
    size_t idx = 0;

    /* File 1: copy as-is */
    if (sht1[section_rela1_index].sh_type == SHT_RELA){
            printf("id 1 is rela \n ");
            int n = sht1[section_rela1_index].sh_size / sht1[section_rela1_index].sh_entsize;
            if (sht2[section_rela2_index].sh_type == SHT_RELA){
                n-= sht2[section_rela2_index].sh_size / sht2[section_rela2_index].sh_entsize;
            }
            printf("found %d rela in 1\n",n);
            for (int j = 0; j < n; j++) {
                out[idx++] = read_rela(f1, &sht1[section_rela1_index], j);
            }
        }

    /* File 2: remap */
    if (sht2[section_rela2_index].sh_type == SHT_REL){
            printf("id 2 is rela\n");

        int src_sec = sht2[section_rela2_index].sh_info;
        uint32_t add_offset = 0;
        add_offset = get_add_offset(fusion, src_sec);

        int n = sht2[section_rela2_index].sh_size / sht2[section_rela2_index].sh_entsize;
        for (int j = 0; j < n; j++) {
            Elf32_Rela r = read_rela(f2, &sht2[section_rela2_index], j);

            /* fix offset */
            r.r_offset += add_offset;

            /* fix symbol */
            uint32_t old_sym = ELF32_R_SYM(r.r_info);
            uint32_t type    = ELF32_R_TYPE(r.r_info);
            uint32_t new_sym = sym_map2[old_sym];

            r.r_info = ELF32_R_INFO(new_sym, type);

            /* fix addend if SECTION symbol */
            Elf32_Sym *s = &fused_symtab[new_sym];
            if (ELF32_ST_TYPE(s->st_info) == STT_SECTION){
                if(type== R_ARM_CALL || type== R_ARM_JUMP24 ){
                r.r_addend += add_offset;
                r.r_addend /= 4;
                }else {
                    r.r_addend+=add_offset;
                }
            }
            out[idx++] = r;
        }
    
    
    
    }
    printf("total rela found %zu\n",idx);
    *out_count = idx;
    return out;

}

Rel_table* fusion_relocation_rel (FILE *f1, FILE *f2, Elf32_Ehdr *hdr1, Elf32_Word hdr1_shnum_old, Elf32_Ehdr *hdr2, Elf32_Shdr *sht1, Elf32_Shdr *sht2, SectionFusionContext *fusion, Elf32_Sym *fused_symtab,int*sym_map1, int *sym_map2, int* rel_tab_size){
    uint32_t total_REL_sections = 0;
    //counting REL sections
    for (int search_for_REL = 0; search_for_REL<hdr1->e_shnum;search_for_REL++ ){
        if (sht1[search_for_REL].sh_type == SHT_REL){
            total_REL_sections  +=1;
        }
    }
    Rel_table* rel_sections_tab = malloc(sizeof(Rel_table)*total_REL_sections);
    uint32_t rel_table_max_index=0;
    uint32_t rel_section_index1, rel_section_index2;

    for (int search_for_REL = 0; search_for_REL<hdr1->e_shnum;search_for_REL++ ){
        if (sht1[search_for_REL].sh_type == SHT_REL){
            rel_section_index1 = search_for_REL;
            
            rel_section_index2 = fusion_context_source(*fusion, rel_section_index1);
            if (search_for_REL>=hdr1_shnum_old){
                rel_section_index1 = 0;
            }
            printf("id1 %d, id2 %d\n",rel_section_index1,rel_section_index2);
            rel_sections_tab[rel_table_max_index].des_index = rel_section_index1;
            rel_sections_tab[rel_table_max_index].rel = fusion_relocation_rel_sections(f1,f2,rel_section_index1, rel_section_index2, sht1,
                                                                                sht2, fusion, sym_map1, sym_map2, &(rel_sections_tab[rel_table_max_index].size));
            rel_table_max_index +=1;
        }

    }
    *rel_tab_size = rel_table_max_index;
    return rel_sections_tab;
};

Rela_table* fusion_relocation_rela (FILE *f1, FILE *f2, Elf32_Ehdr *hdr1, Elf32_Word hdr1_shnum_old, Elf32_Ehdr *hdr2, Elf32_Shdr *sht1, Elf32_Shdr *sht2, SectionFusionContext *fusion, Elf32_Sym *fused_symtab,int*sym_map1, int *sym_map2, int* rela_tab_size){
    uint32_t total_RELA_sections = 0;
    //counting REL sections
    for (int search_for_RELA = 0; search_for_RELA<hdr1->e_shnum;search_for_RELA++ ){
        if (sht1[search_for_RELA].sh_type == SHT_RELA){
            total_RELA_sections  +=1;
        }
    }
    Rela_table* rela_sections_tab = malloc(sizeof(Rela_table)*total_RELA_sections);
    uint32_t rela_table_max_index=0;
    uint32_t rela_section_index1, rela_section_index2;

    for (int search_for_RELA = 0; search_for_RELA<hdr1->e_shnum;search_for_RELA++ ){
        if (sht1[search_for_RELA].sh_type == SHT_RELA){
            rela_section_index1 = search_for_RELA;
            
            rela_section_index2 = fusion_context_source(*fusion, rela_section_index1);
            if (search_for_RELA>=hdr1_shnum_old){
                rela_section_index1 = 0;
            }
            printf("id1 %d, id2 %d\n",rela_section_index1,rela_section_index2);
            rela_sections_tab[rela_table_max_index].des_index = rela_section_index1;
            rela_sections_tab[rela_table_max_index].rela = fusion_relocation_rela_sections(f1,f2,rela_section_index1, rela_section_index2, sht1,
                                                                                sht2, fusion,fused_symtab, sym_map1, sym_map2, &(rela_sections_tab[rela_table_max_index].size));
            rela_table_max_index +=1;
        }

    }
    *rela_tab_size = rela_table_max_index;
    return rela_sections_tab;
};
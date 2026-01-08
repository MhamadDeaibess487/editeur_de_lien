#include "section_fusion.h"
#include <stdlib.h>
#include <string.h>

SectionFusionContext *fusion_init(size_t max_sections){
    SectionFusionContext *fu = malloc(sizeof(SectionFusionContext));
    if (!fu)
        return NULL;

    fu->map = calloc(max_sections, sizeof(SectionMapEntry)); //or malloc + memset but calloc is prolly simpler 
    if (!fu->map) {
        free(fu);
        return NULL;
    }

    fu->merged_sizes = calloc(max_sections, sizeof(uint32_t));
    if (!fu->merged_sizes) {
        free(fu->map);
        free(fu);
        return NULL;
    }

    fu->map_count = 0;
    fu->merged_count = 0;

    return fu;
}

//now, we loop over all sections of file 1, for each section: the “merged section size” is just its original size
//We declare: “the output ELF currently has the same number of sections as file 1"
void fusion_register_file1_sections(Elf32_Ehdr *hdr1, Elf32_Shdr *sht1, SectionFusionContext *fu){
    size_t i;

    for (i = 0; i < hdr1->e_shnum; i++) {
        fu->merged_sizes[i] = sht1[i].sh_size;
    }

    fu->merged_count = hdr1->e_shnum;
}


void fusion_process_file2_sections(Elf32_Ehdr *hdr1, Elf32_Shdr *sht1, char *shstr1, Elf32_Ehdr *hdr2, Elf32_Shdr *sht2, char *shstr2, SectionFusionContext *fu){
    size_t i, j;

    for (i = 0; i < hdr2->e_shnum; i++) {

        if ((sht2[i].sh_type != SHT_PROGBITS && sht2[i].sh_type != SHT_NOBITS) && sht2[i].sh_type != SHT_ARM_ATTRIBUTES&& sht2[i].sh_type != SHT_REL && sht2[i].sh_type != SHT_RELA)
            continue;

        char *name2 = shstr2 + sht2[i].sh_name;

        int found = 0;

        // Search for a section with the same name in file 1 
        for (j = 0; j < hdr1->e_shnum; j++) {

            char *name1 = shstr1 + sht1[j].sh_name;

            if (strcmp(name1, name2) == 0 && sht1[j].sh_type == sht2[i].sh_type) {
                // Merge with section j 
                fu->map[fu->map_count].src_index  = i;
                fu->map[fu->map_count].dst_index  = j;
                fu->map[fu->map_count].add_offset = fu->merged_sizes[j];

                fu->merged_sizes[j] += sht2[i].sh_size;
                fu->map_count++;

                found = 1;
                break;
            }
        }

        if (!found) {
            //Create a new section 
            size_t new_index = fu->merged_count;

            fu->map[fu->map_count].src_index  = i;
            fu->map[fu->map_count].dst_index  = new_index;
            fu->map[fu->map_count].add_offset = 0;

            fu->merged_sizes[new_index] = sht2[i].sh_size;

            fu->merged_count++;
            fu->map_count++;
        }
    }
}

//just gluing the sub fonctions: 

SectionFusionContext *fusion_build_sections(FILE *f1, FILE *f2,Elf32_Ehdr *hdr1, Elf32_Ehdr *hdr2,Elf32_Shdr **sht1, Elf32_Shdr *sht2, char **shstr1, char *shstr2){
    SectionFusionContext *fu;

    // Maximum possible sections = sum of both
    fu = fusion_init(hdr1->e_shnum + hdr2->e_shnum);
    if (!fu){
        return NULL;
    }
    fusion_register_file1_sections(hdr1, *sht1, fu);
    fusion_process_file2_sections(hdr1, *sht1, *shstr1,hdr2, sht2, shstr2,fu);
    (*sht1) = realloc(*sht1,hdr1->e_shentsize * fu->merged_count);

    size_t source;
    size_t dest;
    size_t extra_shstrtab_space = 0;
    int symtab_index = 0;

    //modifying sht1
    for (int iterate_fusion_maps = 0; iterate_fusion_maps < fu->map_count; iterate_fusion_maps++){
        dest = fu->map[iterate_fusion_maps].dst_index;
        source = fu->map[iterate_fusion_maps].src_index;
        if (dest < hdr1->e_shnum){
            (*sht1)[dest].sh_size = 
            (*sht1)[dest].sh_size + (sht2)[source].sh_size;
            printf("added %zu to %zu\n",source,dest);
        }
        else {
            (*sht1)[dest]= sht2[source];
            extra_shstrtab_space += strlen(shstr2+sht2[source].sh_name);
            if (sht2[source].sh_type == SHT_REL || sht2[source].sh_type == SHT_RELA){
                //searching for symtab
                for (int search_for_symtab =0; search_for_symtab < hdr1->e_shnum; search_for_symtab++){
                    if ((*sht1)[search_for_symtab].sh_type == SHT_SYMTAB){
                        symtab_index = search_for_symtab;
                        continue;
                    }
                }
                (*sht1)[dest].sh_link = symtab_index;
            }


        }
    }
    printf("total str size need to add: %zu\n",extra_shstrtab_space);
    printf("current str size %d\n",(*sht1)[hdr1->e_shstrndx].sh_size);
    size_t shstr1_len = (*sht1)[hdr1->e_shstrndx].sh_size;
    printf("shstr1 len: %zu\n",shstr1_len);
    (*shstr1) = realloc(*shstr1,(*sht1)[hdr1->e_shstrndx].sh_size + extra_shstrtab_space);
    (*sht1)[hdr1->e_shstrndx].sh_size = (*sht1)[hdr1->e_shstrndx].sh_size + extra_shstrtab_space+1;
   
    
    for (int iterate_fusion_maps = 0; iterate_fusion_maps < fu->map_count; iterate_fusion_maps++){
        dest = fu->map[iterate_fusion_maps].dst_index;
        source = fu->map[iterate_fusion_maps].src_index;
        if (dest >= hdr1->e_shnum){
            strcpy(*shstr1+shstr1_len, shstr2+ sht2[source].sh_name);
            printf("added string %s \n",shstr2+sht2[source].sh_name );
            (*sht1)[dest].sh_name = shstr1_len;
            shstr1_len += strlen(shstr2+ (sht2[source].sh_name)) +1;
        }
    }

    for (int i =0; i<shstr1_len; i++){
        if (*(*shstr1+i) == '\0'){
            printf("\\0");
        }
        else {
        printf("%c",*(*shstr1+i) );}
    }

    

    hdr1->e_shnum = fu->merged_count;
    return fu;
}
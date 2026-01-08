#include "symbole_fusion.h"
#include "section_fusion.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



Elf32_Half fusion_remap_index(SectionFusionContext *fusion_context, int source){
    printf("matching %d ",source);
    for (int iterate_source =0; iterate_source<fusion_context->map_count; iterate_source ++){
        if (fusion_context->map[iterate_source].src_index==source){
            printf("found %d\n",fusion_context->map[iterate_source].dst_index);
            return fusion_context->map[iterate_source].dst_index;
        }
    }
    printf ("not found!\n");
    return source;
}

Elf32_Addr fusion_remap_offset(SectionFusionContext *fusion_context, int source){
    printf("adding offset to %d ",source);
    for (int iterate_source =0; iterate_source<fusion_context->map_count; iterate_source ++){
        if (fusion_context->map[iterate_source].src_index==source){
            printf("found %d\n",fusion_context->map[iterate_source].add_offset);
            return fusion_context->map[iterate_source].add_offset;
        }
    }
    printf ("not found!\n");
    return 0;
}



Elf32_Sym *fusion_symbole(Elf32_Sym *symtab1,Elf32_Sym *symtab2,Elf32_Ehdr hdr1, Elf32_Ehdr hdr2, 
                            Elf32_Shdr **section_header_table1, Elf32_Shdr *section_header_table2, 
                            char* strtab1, char* strtab2,char ** new_strtab, SectionFusionContext *fusion_context, 
                            int **sym_map1, int **sym_map2){
    if (*sym_map1 !=NULL || *sym_map2 != NULL){
        fprintf(stderr,"need to pass NULL sym_map values");
        return NULL;
    }                            

    Elf32_Shdr sym_section_header1;
    Elf32_Shdr sym_section_header2;
    size_t strtab_section_index =0;
    size_t sym_section_index1 = 0;
    int nb_sym = 0;
    for (int iterate_sections_1 =0; iterate_sections_1<hdr1.e_shnum; iterate_sections_1++){
        if ((*section_header_table1)[iterate_sections_1].sh_type == SHT_SYMTAB){
            sym_section_header1 = (*section_header_table1)[iterate_sections_1];
            sym_section_index1 = iterate_sections_1;
            nb_sym+=(*section_header_table1)[iterate_sections_1].sh_size/(*section_header_table1)[iterate_sections_1].sh_entsize;
            strtab_section_index =(*section_header_table1)[iterate_sections_1].sh_link ;
        }       
    }

    for (int iterate_sections_2 =0; iterate_sections_2<hdr2.e_shnum; iterate_sections_2++){
        if (section_header_table2[iterate_sections_2].sh_type == SHT_SYMTAB){
            sym_section_header2 = section_header_table2[iterate_sections_2];
            nb_sym+=(section_header_table2)[iterate_sections_2].sh_size/(section_header_table2)[iterate_sections_2].sh_entsize;
        }
    }
    *sym_map1 = malloc(sizeof(int)*nb_sym);
    *sym_map2 = malloc(sizeof(int)*nb_sym);
    if (!*sym_map1 || !*sym_map2){
        fprintf(stderr, "error malloc symmap\n");
        return NULL;
    }

    size_t strtab1_length = (*section_header_table1)[sym_section_header1.sh_link].sh_size;
    size_t strtab2_length = section_header_table2[sym_section_header2.sh_link].sh_size;
    //new sym name table
    *new_strtab = malloc(sizeof(char)*(strtab1_length+strtab2_length));
    memcpy(*new_strtab,strtab1,strtab1_length);
    size_t new_strtab_length = strtab1_length;
    
    size_t nb_sym1 = sym_section_header1.sh_size / sym_section_header1.sh_entsize;
    size_t nb_sym2 = sym_section_header2.sh_size / sym_section_header2.sh_entsize;
    size_t new_sym_count = nb_sym1 + nb_sym2;
    //new sym table 
    Elf32_Sym *new_symtab;
    
    new_symtab = malloc(new_sym_count * sizeof(Elf32_Sym));
    if (!new_symtab)
        return NULL;    
    size_t index_new_sym_tab=0;
    //copy the local symbols from both tables 
    for(size_t i = 0; i < nb_sym1; i++){
        if(ELF32_ST_BIND(symtab1[i].st_info) == STB_LOCAL){
            new_symtab[index_new_sym_tab] = symtab1[i];
            (*sym_map1)[i]=index_new_sym_tab;
            index_new_sym_tab++;
        }
    }


    for(size_t j=1;j<nb_sym2;j++){
        if(ELF32_ST_BIND(symtab2[j].st_info) == STB_LOCAL){
            new_symtab[index_new_sym_tab] = symtab2[j];
            new_symtab[index_new_sym_tab].st_value += reverse_4(fusion_remap_offset(fusion_context,reverse_2(new_symtab[index_new_sym_tab].st_shndx)));
            new_symtab[index_new_sym_tab].st_shndx = reverse_2(fusion_remap_index(fusion_context,reverse_2(new_symtab[index_new_sym_tab].st_shndx)));
            new_symtab[index_new_sym_tab].st_name = reverse_4(new_strtab_length);
            (*sym_map2)[j]=index_new_sym_tab;
            
            index_new_sym_tab++;
            // printf(": %zu strtab2 + symtab2[j].st_name %s , new_strtab_length: %zu\n",j,strtab2 + reverse_4(symtab2[j].st_name),new_strtab_length);
            strcpy((*new_strtab) + new_strtab_length , strtab2 + reverse_4(symtab2[j].st_name));
            if (strlen(strtab2 + reverse_4(symtab2[j].st_name)) != 0){
                new_strtab_length = new_strtab_length + 1+ strlen(strtab2 + reverse_4(symtab2[j].st_name));
            }
            // printf("add symbol LOCAL 2, %zu\n",j);
            (*section_header_table1)[sym_section_index1].sh_size+= (*section_header_table1)[sym_section_index1].sh_entsize;
        }
        
    (*section_header_table1)[sym_section_index1].sh_info = index_new_sym_tab ;
    }
    //merge the global symbols
    size_t index_founded=0;
    for(size_t i = 0; i < nb_sym1; i++){
        if(ELF32_ST_BIND(symtab1[i].st_info) == STB_GLOBAL){
            char *s_name1 = strtab1 + reverse_4(symtab1[i].st_name);
            // printf("s_name1 : %s\n", s_name1);
            int found = 0;
            for(size_t j=0;j<nb_sym2;j++){
                if(ELF32_ST_BIND(symtab2[j].st_info) == STB_GLOBAL){
                    char *s_name2 = strtab2 + reverse_4(symtab2[j].st_name);
                    // printf("s_name2 : %s\n", s_name2);
                    if(strcmp(s_name1,s_name2) == 0){
                        found = 1;
                        index_founded = j;
                    }
                }
            }
            if(!found){
                new_symtab[index_new_sym_tab] = symtab1[i];
                (*sym_map1)[i]=index_new_sym_tab;
            
                index_new_sym_tab++;
            }
            else if(found){
                //defined in both tables
                if(symtab1[i].st_shndx != SHN_UNDEF && symtab2[index_founded].st_shndx != SHN_UNDEF){
                    fprintf(stderr, "Error: Redefinition of symbol %s\n", s_name1);
                    free(new_symtab);
                    return NULL;
                }
                //defined in symtab 1 and not 2 
                else if(symtab1[i].st_shndx != SHN_UNDEF && symtab2[index_founded].st_shndx == SHN_UNDEF){
                    new_symtab[index_new_sym_tab] = symtab1[i];
                   
                    (*sym_map1)[i]=index_new_sym_tab;
                    
                    index_new_sym_tab++;
                }
                //defined in symtab 2 and not 1
                else if(symtab1[i].st_shndx == SHN_UNDEF && symtab2[index_founded].st_shndx != SHN_UNDEF){
                    new_symtab[index_new_sym_tab] = symtab2[index_founded];
                    new_symtab[index_new_sym_tab].st_value += reverse_4(fusion_remap_offset(fusion_context,reverse_2(new_symtab[index_new_sym_tab].st_shndx)));
                    new_symtab[index_new_sym_tab].st_shndx = reverse_2(fusion_remap_index(fusion_context,reverse_2(new_symtab[index_new_sym_tab].st_shndx)));
                    new_symtab[index_new_sym_tab].st_name = reverse_4(new_strtab_length);
                    (*sym_map2)[index_founded]=index_new_sym_tab;
                    
                    strcpy(*new_strtab+new_strtab_length,strtab2+ reverse_4(symtab2[index_founded].st_name));
                    if (strlen(strtab2 + reverse_4(symtab2[index_founded].st_name))!=0){
                        new_strtab_length = new_strtab_length + 1 + strlen(strtab2+ reverse_4(symtab2[index_founded].st_name));
                    }
                    index_new_sym_tab++;
                    // printf("add symbol \n");
                    // (*section_header_table1)[sym_section_index1].sh_size+= (*section_header_table1)[sym_section_index1].sh_entsize;

                }
                //undefined in both tables
                else{
                    new_symtab[index_new_sym_tab] = symtab1[i];
                    (*sym_map1)[i]=index_new_sym_tab;
                    index_new_sym_tab++;
                }
            }
        }
    }

    for(size_t i = 0; i < nb_sym2; i++){
        if(ELF32_ST_BIND(symtab2[i].st_info) == STB_GLOBAL){
            char *s_name2 = strtab2 + reverse_4(symtab2[i].st_name);
            // printf("s_name2 : %s\n", s_name2);
            int found = 0;
            for(size_t j=0;j<nb_sym1;j++){
                if(ELF32_ST_BIND(symtab1[j].st_info) == STB_GLOBAL){
                    char *s_name1 = strtab1 + reverse_4(symtab1[j].st_name);
                    // printf("s_name1 : %s\n", s_name1);
                    if(strcmp(s_name1,s_name2) == 0){
                        found = 1;
                        index_founded = j;
                    }
                }
            }
            if(!found){
                new_symtab[index_new_sym_tab] = symtab2[i];
                    new_symtab[index_new_sym_tab].st_name = reverse_4(new_strtab_length);
                    new_symtab[index_new_sym_tab].st_value += reverse_4(fusion_remap_offset(fusion_context,reverse_2(new_symtab[index_new_sym_tab].st_shndx)));
                    new_symtab[index_new_sym_tab].st_shndx = reverse_2(fusion_remap_index(fusion_context,reverse_2(new_symtab[index_new_sym_tab].st_shndx)));
                    (*sym_map2)[i]=index_new_sym_tab;
                    strcpy(*new_strtab+new_strtab_length,strtab2+ reverse_4(symtab2[i].st_name));
                    if (strlen(strtab2 + reverse_4(symtab2[i].st_name))!=0){
                        new_strtab_length = new_strtab_length + 1 + strlen(strtab2+ reverse_4(symtab2[i].st_name));
                    }
                    index_new_sym_tab++;
                    // printf("add symbol (only for tab 2) \n");
                    (*section_header_table1)[sym_section_index1].sh_size+= (*section_header_table1)[sym_section_index1].sh_entsize;
                //index_new_sym_tab++;
            }
        }
    }
    printf("strtab_section_index:%zu, newlength: %zu\n",strtab_section_index,new_strtab_length);
    (*section_header_table1)[strtab_section_index].sh_size =  new_strtab_length;
    // printf("END OF LOOP\n");
    return new_symtab;

}
    


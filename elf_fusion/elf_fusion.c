#include <stdio.h>
#include <stdlib.h>
#include "elf_reader_headers.h"
#include "symbole_fusion.h"
#include "relocation_fusion.h"
#include "elf_writer.h"
#include <string.h>
/*
Usage:
   ./elf_fusion file1.o file2.o

 Step 6 only:
    - read both ELF files
   - build section fusion context
   - print fusion result (debug)

 Step 7:
    - read symbol tables from both files
    - fuse symbol tables according to section fusion context
    - print fused symbol table (debug)
*/


//Copy bytes from source to dest
void copy_byte(FILE* dest, FILE* source, uint32_t num){
    unsigned char byte_to_copy;
    for (uint32_t i=0; i<num; i++){
    if (!fread(&byte_to_copy, sizeof(unsigned char),1,source)){
        fprintf(stderr,"fail to copy, source, byte %u\n",i);
        return;
    }
    if (!fwrite(&byte_to_copy, sizeof(unsigned_char),1,dest)){
        fprintf(stderr, "fail to write, dest, byte %u\n",i);
        return;
    }
    }
}

int main(int argc, char **argv)
{
    if (argc <3) {
        fprintf(stderr, "Usage: %s <file1.o> <file2.o> <file3output (optional)\n", argv[0]);
        return 1;
    }

    FILE *f1, *f2;
    Elf32_Ehdr *hdr1, *hdr2;
    
    Elf32_Shdr *sht1, *sht2;
    char *shstr1, *shstr2;
    SectionFusionContext *fu;
    size_t i;

    f1 = fopen(argv[1], "rb");
    if (!f1) {
        perror("fopen file1");
        return 1;
    }
    f2 = fopen(argv[2], "rb");
    if (!f2) {
        perror("fopen file2");
        fclose(f1);
        return 1;
    }
    hdr1 = ELF_header_reader(f1);
    hdr2 = ELF_header_reader(f2);
    Elf32_Half old_hdr_shnum = hdr1->e_shnum;
    if (!hdr1 || !hdr2) {
        fprintf(stderr, "Error reading ELF headers\n");
        fclose(f1);
        fclose(f2);
        return 1;
    }

    // Read section header tables
    readShtable(f1, hdr1, &sht1);
    readShtable(f2, hdr2, &sht2);
    if (!sht1 || !sht2) {
        fprintf(stderr, "Error reading section headers\n");
        free(hdr1);
        free(hdr2);
        fclose(f1);
        fclose(f2);
        return 1;
    }

    // Read section string tables
    Elf32_Shdr *shstr_shdr1 = &sht1[hdr1->e_shstrndx];
    shstr1 = malloc(shstr_shdr1->sh_size);
    if (!shstr1) {
        fprintf(stderr, "Error allocating memory for section string table 1\n");
        free(hdr1);
        free(hdr2);
        free(sht1);
        free(sht2);
        fclose(f1);
        fclose(f2);
        return 1;
    }
    fseek(f1, shstr_shdr1->sh_offset, SEEK_SET);
    fread(shstr1, 1, shstr_shdr1->sh_size, f1);
   
    Elf32_Shdr *shstr_shdr2 = &sht2[hdr2->e_shstrndx];
    shstr2 = malloc(shstr_shdr2->sh_size);
    if (!shstr2) {
        fprintf(stderr, "Error allocating memory for section string table 2\n");
        free(hdr1);
        free(hdr2);
        free(sht1);
        free(sht2);
        free(shstr1);
        fclose(f1);
        fclose(f2);
        return 1;
    }
    fseek(f2, shstr_shdr2->sh_offset, SEEK_SET);
    fread(shstr2, 1, shstr_shdr2->sh_size, f2);

    fu = fusion_build_sections(f1, f2, hdr1, hdr2, &sht1, sht2, &shstr1, shstr2);
    if (!fu) {
        fprintf(stderr, "Error building section fusion context\n");
        free(hdr1);
        free(hdr2);
        free(sht1);
        free(sht2);
        free(shstr1);
        free(shstr2);
        fclose(f1);
        fclose(f2);
        return 1;
    }

    printf("=== Section Fusion Result ===\n");
    printf("Total merged sections: %zu\n", fu->merged_count);
    printf("Total mappings: %zu\n\n", fu->map_count);
    
    printf("Merged section sizes:\n");
    for (i = 0; i < fu->merged_count; i++) {
        printf("  Section %zu: %u bytes\n", i, fu->merged_sizes[i]);
    }
    
    printf("\nSection mappings (file2 -> output):\n");
    for (i = 0; i < fu->map_count; i++) {
        char *src_name = shstr2 + sht2[fu->map[i].src_index].sh_name;
        printf("  File2 section %d (%s) -> Output section %d (offset +%u)\n",
               fu->map[i].src_index, src_name, fu->map[i].dst_index, fu->map[i].add_offset);
    }

    printShtable_noname(sht1,hdr1);

    Elf32_Sym* symtab1 = NULL ;
    Elf32_Sym* symtab2 = NULL ; 
    char *sym_strtab1 = NULL;
    char *sym_strtab2 = NULL;
    char* new_strtab = NULL;
    symtab1 = read_symbole_table(f1, sht1,hdr1, shstr1, &sym_strtab1);
    print_symbole_table(sht1, hdr1, shstr1,symtab1,sym_strtab1);
    symtab2 = read_symbole_table(f2, sht2,hdr2, shstr2, &sym_strtab2);
    print_symbole_table(sht2, hdr2, shstr2,symtab2,sym_strtab2);
    int * sym_map1 = NULL;
    int * sym_map2 = NULL;
    Elf32_Sym * new_symtab = NULL;
    new_symtab = fusion_symbole(symtab1,symtab2,*hdr1,*hdr2,&sht1, sht2, sym_strtab1, sym_strtab2, &new_strtab,fu,&sym_map1,&sym_map2);
    
    
    print_symbole_table(sht1, hdr1, shstr1,new_symtab,new_strtab);


    Rel_table* rel_tab = NULL;
    int rel_tab_size = 0;
    rel_tab = fusion_relocation_rel(f1, f2, hdr1, old_hdr_shnum, hdr2, sht1, sht2, fu, new_symtab, sym_map1, sym_map2, &rel_tab_size);
    printf("iterating rel_tab %d rels\n",rel_tab_size);
    for (int iterate_rel_tab = 0; iterate_rel_tab<rel_tab_size;iterate_rel_tab++){
        printf("Rel_tab section dest %d containing %d rel\n",rel_tab[iterate_rel_tab].des_index, rel_tab[iterate_rel_tab].size);
        for (int iterate_sub_rel =0; iterate_sub_rel < rel_tab[iterate_rel_tab].size; iterate_sub_rel ++){
        print_one_reloc_line(rel_tab[iterate_rel_tab].rel[iterate_sub_rel], sht1, hdr1, shstr1, new_symtab, new_strtab);
    }
    }

    
    Rela_table* rela_tab = NULL;
    int rela_tab_size = 0;
    rela_tab = fusion_relocation_rela(f1, f2, hdr1, old_hdr_shnum, hdr2, sht1, sht2, fu, new_symtab, sym_map1, sym_map2, &rel_tab_size);
    printf("iterating rela_tab %d relas\n",rela_tab_size);
    for (int iterate_rela_tab = 0; iterate_rela_tab<rela_tab_size;iterate_rela_tab++){
        printf("Rela_tab section dest %d containing %d rela\n",rela_tab[iterate_rela_tab].des_index, rel_tab[iterate_rela_tab].size);
        for (int iterate_sub_rela =0; iterate_sub_rela < rel_tab[iterate_rela_tab].size; iterate_sub_rela ++){
        print_one_reloc_line(rel_tab[iterate_rela_tab].rel[iterate_sub_rela], sht1, hdr1, shstr1, new_symtab, new_strtab);
    }
    }

    //reorganizing shdr table
    Elf32_Off start_section_header_table = 0;
    
    
    Elf32_Word size_shstrtab = sht1[hdr1->e_shstrndx].sh_size;
    for (int iterate_section_headers = 1; iterate_section_headers <hdr1->e_shnum; iterate_section_headers++){
        (sht1)[iterate_section_headers].sh_offset = 0x34;
        for (int iterate_sum_headers = 0; iterate_sum_headers  < iterate_section_headers; iterate_sum_headers++){
            (sht1)[iterate_section_headers].sh_offset += (sht1)[iterate_sum_headers].sh_size;
        }
        start_section_header_table = 10 + (sht1)[iterate_section_headers].sh_offset +(sht1)[iterate_section_headers].sh_size ;
    }
    printShtable_noname(sht1,hdr1);


    Elf32_Half sections_number= hdr1->e_shnum;
    Elf32_Off start_shstrtab = sht1[hdr1->e_shstrndx].sh_offset;
    
    //writing the file
    if (argc<4){
        return 0;
    }

    FILE *file_write_elf =  fopen(argv[3],"wb");
    printf("section header table start: %d\n",start_section_header_table);
    hdr1->e_shoff = start_section_header_table;

    printf("eshnum :%d\n",hdr1->e_shnum);

    
    //header
    write_elf_header(file_write_elf, hdr1);

    //sections
    fseek(f1, 0 , SEEK_SET);
    Elf32_Ehdr *old_hdr1 = ELF_header_reader(f1);
    Elf32_Shdr *old_sht1 = NULL;
    readShtable(f1, old_hdr1, &old_sht1);

    int index_section_file1;
    int index_section_file2; 
    int index_rel_tab_to_section;
    int index_rela_tab_to_section;
    for (int iterate_write_sections=1; iterate_write_sections <sections_number; iterate_write_sections ++){
        printf("writing section %d, at %d\n",iterate_write_sections, (sht1[iterate_write_sections].sh_offset));
        switch (sht1[iterate_write_sections].sh_type){

        case SHT_SYMTAB:
            fseek(file_write_elf, sht1[iterate_write_sections].sh_offset, SEEK_SET);
            fwrite(new_symtab,sizeof(unsigned_char),sht1[iterate_write_sections].sh_size,file_write_elf);
            break;
        case SHT_STRTAB:
            if (strcmp(shstr1+sht1[iterate_write_sections].sh_name, ".strtab")==0){
                printf(".strtab here\n");
                fseek(file_write_elf, sht1[iterate_write_sections].sh_offset, SEEK_SET);
                write_elf_shstrtab(file_write_elf, new_strtab, sht1[iterate_write_sections].sh_offset, sht1[iterate_write_sections].sh_size);
            }
            break;
        case SHT_REL:
            printf("writing rels\n");
            fseek(file_write_elf, sht1[iterate_write_sections].sh_offset, SEEK_SET);
            for (int iterate_rel_tab = 0; iterate_rel_tab < rel_tab_size;  iterate_rel_tab ++){
                if (rel_tab[iterate_rel_tab].des_index == iterate_write_sections){
                    index_rel_tab_to_section = iterate_rel_tab;
                }
            for (int iterate_rels = 0; iterate_rels<rel_tab[index_rel_tab_to_section].size; iterate_rels++){
                rel_tab[index_rel_tab_to_section].rel->r_info = reverse_4(rel_tab[index_rel_tab_to_section].rel->r_info);
                rel_tab[index_rel_tab_to_section].rel->r_offset = reverse_4(rel_tab[index_rel_tab_to_section].rel->r_offset);
                fwrite(&rel_tab[index_rel_tab_to_section],sizeof(Elf32_Rel),1,file_write_elf);
                
            }   
            }
            break;
        case SHT_RELA:
            printf("writing relas\n");
            fseek(file_write_elf, sht1[iterate_write_sections].sh_offset, SEEK_SET);
            for (int iterate_rela_tab = 0; iterate_rela_tab < rela_tab_size;  iterate_rela_tab ++){
                if (rela_tab[iterate_rela_tab].des_index == iterate_write_sections){
                    index_rela_tab_to_section = iterate_rela_tab;
                }
            for (int iterate_relas = 0; iterate_relas<rela_tab[index_rela_tab_to_section].size; iterate_relas++){
                rela_tab[index_rela_tab_to_section].rela->r_info = reverse_4(rela_tab[index_rela_tab_to_section].rela->r_info);
                rela_tab[index_rela_tab_to_section].rela->r_offset = reverse_4(rela_tab[index_rela_tab_to_section].rela->r_offset);
                rela_tab[index_rela_tab_to_section].rela->r_addend = reverse_4(rela_tab[index_rela_tab_to_section].rela->r_addend);
                fwrite(&rela_tab[index_rela_tab_to_section],sizeof(Elf32_Rela),1,file_write_elf);
                
            }   
            }
            break;


        default:
            fseek(file_write_elf, sht1[iterate_write_sections].sh_offset, SEEK_SET);
            index_section_file1 =  iterate_write_sections;
            index_section_file2 = fusion_context_source(*fu,iterate_write_sections);
            if (index_section_file1 < old_hdr_shnum){
                fseek(f1, old_sht1[index_section_file1].sh_offset,SEEK_SET);
                copy_byte(file_write_elf,f1,old_sht1[index_section_file1].sh_size);
            }
            if (index_section_file2!=0){
                fseek(f2, sht2[index_section_file2].sh_offset, SEEK_SET);
                copy_byte(file_write_elf,f2,sht2[index_section_file2].sh_size);
            }
            break;
        
        }
        
    }

    //shstrab
    write_elf_shstrtab(file_write_elf, shstr1, start_shstrtab, size_shstrtab);
    printf("offset shstrtab:%d\n",start_shstrtab);



    
    //section header table
    write_elf_section_header(file_write_elf, sht1, start_section_header_table, sections_number);

    fclose(file_write_elf);


    free(fu->map);
    free(fu->merged_sizes);
    free(fu);
    free(hdr1);
    free(hdr2);
    free(sht1);
    free(sht2);
    free(shstr1);
    free(shstr2);;
    free(sym_map1);
    free(sym_map2);
   
    fclose(f1);
    fclose(f2);

    return 0;
}

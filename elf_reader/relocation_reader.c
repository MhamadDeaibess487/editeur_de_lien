#include "relocation_reader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "symbole_table.h"



Elf32_Rel read_one_relocation_no_addend(FILE *stream, Elf32_Ehdr *hdr, Elf32_Shdr *Shdr, int section_index, int entry_index){
    Elf32_Rel r;
    
        fseek(stream, Shdr[section_index].sh_offset + entry_index*Shdr[section_index].sh_entsize,SEEK_SET);
        fread(&r.r_offset, sizeof(uint32_t), 1, stream);
        fread(&r.r_info, sizeof(uint32_t), 1, stream);
    
        if (!is_big_endian()) {
            r.r_offset = reverse_4(r.r_offset);
            r.r_info = reverse_4(r.r_info);
        }
    
    return r;
}

//function that reads one relocation entry with addend
Elf32_Rela read_one_relocation_with_addend(FILE *stream, Elf32_Ehdr *hdr, Elf32_Shdr *Shdr, int section_index, int entry_index){
    Elf32_Rela rela;
    Elf32_Rela r;
        fseek(stream, Shdr[section_index].sh_offset + entry_index*Shdr[section_index].sh_entsize,SEEK_SET);
        fread(&rela.r_offset, sizeof(uint32_t), 1, stream);
        fread(&rela.r_info, sizeof(uint32_t), 1, stream);
        fread(&rela.r_addend, sizeof(int32_t), 1, stream);
        r.r_offset = rela.r_offset;
        r.r_info = rela.r_info;
        r.r_addend = rela.r_addend;
        if (!is_big_endian()) {
            r.r_addend = reverse_4(r.r_addend);
            r.r_offset = reverse_4(r.r_offset);
            r.r_info = reverse_4(r.r_info);
        }
    return r;
}




int g_nrel = 0;
int g_nrela = 0;

int read_relocations(FILE *stream, Elf32_Ehdr *hdr, Elf32_Shdr *Shdr, char *shstrtab, Elf32_Rela **rela, Elf32_Rel **rel){
    int total_rel = 0;
    int total_rela = 0;

    for (int i = 0; i < hdr->e_shnum; i++) {
        if (Shdr[i].sh_type == SHT_REL) {
            total_rel += (int)(Shdr[i].sh_size / Shdr[i].sh_entsize);
        } else if (Shdr[i].sh_type == SHT_RELA) {
            total_rela += (int)(Shdr[i].sh_size / Shdr[i].sh_entsize);
        }
    }
    g_nrel = total_rel;
    g_nrela = total_rela;

    if(rel == NULL){
        *rel = NULL;
        return 0;
    }
    if(rela == NULL){
        *rela = NULL;
        return 0;
    }

    if (total_rel > 0) {
        *rel = (Elf32_Rel *)malloc((size_t)total_rel * sizeof(Elf32_Rel));
        if (!*rel) {
            g_nrel = 0; g_nrela = 0;
            return 0;
        }
    }
    if (total_rela > 0) {
        *rela = (Elf32_Rela *)malloc((size_t)total_rela * sizeof(Elf32_Rela));
        if (!*rela) {
            free(*rel);
            *rel = NULL;
            g_nrel = 0; g_nrela = 0;
            return 0;
        }
    }

    int i_rela = 0;
    int i_rel = 0;
    for(int i = 0; i < hdr->e_shnum; i++){
        if(Shdr[i].sh_type == SHT_RELA || Shdr[i].sh_type==SHT_REL){
            int nb_reloc = Shdr[i].sh_size / Shdr[i].sh_entsize;
            for(int j = 0; j < nb_reloc; j++){
                fseek(stream, Shdr[i].sh_offset + j*Shdr[i].sh_entsize,SEEK_SET);
                if(Shdr[i].sh_type == SHT_RELA){
                    Elf32_Rela r;
                    fread(&r.r_offset, sizeof(uint32_t), 1, stream);
                    fread(&r.r_info, sizeof(uint32_t), 1, stream);
                    fread(&r.r_addend, sizeof(uint32_t), 1, stream);
                    if (!is_big_endian()) {
                        r.r_addend = reverse_4(r.r_addend);
                        r.r_offset = reverse_4(r.r_offset);
                        r.r_info = reverse_4(r.r_info);
                    }
                    (*rela)[i_rela++] = r;
                }
                else {
                    Elf32_Rel r;
                    fread(&r.r_offset, sizeof(uint32_t), 1, stream);
                    fread(&r.r_info, sizeof(uint32_t), 1, stream);
                    if (!is_big_endian()) {
                        r.r_offset = reverse_4(r.r_offset);
                        r.r_info = reverse_4(r.r_info);
                    }
                    (*rel)[i_rel++] = r;
                }
            }
        }
    }
    return total_rel;
}

void print_one_reloc_line(Elf32_Rel r, Elf32_Shdr *Shdr, Elf32_Ehdr *hdr,
                                 char *shstrtab, Elf32_Sym *symtab, char *strtab)
{
    printf("%08x  %08x  ", r.r_offset, r.r_info);

    uint32_t type = ELF32_R_TYPE(r.r_info);
    switch (type) {
        case R_ARM_ABS32:
            printf("R_ARM_ABS32    ");
            break;
        case R_ARM_CALL:
            printf("R_ARM_CALL     ");
            break;
        case R_ARM_JUMP24:
            printf("R_ARM_JUMP24   ");
            break;
        default:
            printf("%08d        ", type);
            break;
    }
    int index = ELF32_R_SYM(r.r_info);
    Elf32_Sym s = symtab[index];
    if (!is_big_endian()) {
        swap_Elf32_Sym(&s);
    }

    printf(" %08x ", s.st_value);

    if (ELF32_ST_TYPE(s.st_info) == STT_SECTION && s.st_shndx < hdr->e_shnum) {
        Elf32_Shdr *sec = &Shdr[s.st_shndx];
        printf("%s\n", shstrtab + sec->sh_name);
    } else {
        printf("%s\n", strtab + s.st_name);
    }
}

void print_relocations_from_memory(Elf32_Shdr *Shdr, Elf32_Ehdr *hdr, char *shstrtab, Elf32_Sym *symtab, char *strtab, Elf32_Rela *rela, int nrela,Elf32_Rel *rel, int nrel){
    int idx_rela = 0;
    int idx_rel = 0;

    for (int i = 0; i < hdr->e_shnum; i++) {
        if (Shdr[i].sh_type != SHT_RELA && Shdr[i].sh_type != SHT_REL)
            continue;

        int nb_reloc = (int)(Shdr[i].sh_size / Shdr[i].sh_entsize);
        uint32_t offset = Shdr[i].sh_offset;
        char *name = &shstrtab[Shdr[i].sh_name];

        printf("\nRelocation section %s at offset 0x%x contains %d entries:\n", name, offset, nb_reloc);
        printf("Offset     Info    Type            Sym.Value  Sym. Name\n");

        if (Shdr[i].sh_type == SHT_RELA) {
            for (int j = 0; j < nb_reloc; j++) {
                if (idx_rela >= nrela) return;
                Elf32_Rel r;
                r.r_offset = rela[idx_rela].r_offset;
                r.r_info   = rela[idx_rela].r_info;
                idx_rela++;
                print_one_reloc_line(r, Shdr, hdr, shstrtab, symtab, strtab);
            }
        } else {
            for (int j = 0; j < nb_reloc; j++) {
                if (idx_rel >= nrel) return;
                Elf32_Rel r = rel[idx_rel++];
                print_one_reloc_line(r, Shdr, hdr, shstrtab, symtab, strtab);
            }
        }
    }
}


void print_relocations(FILE *stream, Elf32_Shdr *Shdr, Elf32_Ehdr *hdr, char *shstrtab, Elf32_Sym *symtab, char *strtab){
    Elf32_Rela *rela = NULL;
    Elf32_Rel  *rel  = NULL;

    read_relocations(stream, hdr, Shdr, shstrtab, &rela, &rel);

    print_relocations_from_memory(Shdr, hdr, shstrtab, symtab, strtab,rela, g_nrela, rel, g_nrel);

    free(rela);
    free(rel);
}


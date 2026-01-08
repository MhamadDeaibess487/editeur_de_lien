#include "elf_writer.h"
#include <stdio.h>
#include <stdlib.h>

// void swap_Elf32_Shdr(Elf32_Shdr *s){
//     s->sh_name      = reverse_4(s->sh_name);
//     s->sh_type      = reverse_4(s->sh_type);
//     s->sh_flags     = reverse_4(s->sh_flags);
//     s->sh_addr      = reverse_4(s->sh_addr);
//     s->sh_offset    = reverse_4(s->sh_offset);
//     s->sh_size      = reverse_4(s->sh_size);
//     s->sh_link      = reverse_4(s->sh_link);
//     s->sh_info      = reverse_4(s->sh_info);
//     s->sh_addralign = reverse_4(s->sh_addralign);
//     s->sh_entsize   = reverse_4(s->sh_entsize);
// }

int write_elf_header(FILE *stream, Elf32_Ehdr *hdr){
    // returns 0 if there is a problem. 1 otherwise.
    hdr->e_type = reverse_2(hdr->e_type);
    hdr->e_machine = reverse_2(hdr->e_machine);
    hdr->e_version = reverse_4(hdr->e_version);
    hdr->e_entry = reverse_4(hdr->e_entry);
    hdr->e_phoff = reverse_4(hdr->e_phoff);
    hdr->e_shoff = reverse_4(hdr->e_shoff);
    hdr->e_flags = reverse_4(hdr->e_flags);
    hdr->e_ehsize = reverse_2(hdr->e_ehsize);
    hdr->e_phentsize = reverse_2(hdr->e_phentsize);
    hdr->e_phnum = reverse_2(hdr->e_phnum);
    hdr->e_shentsize = reverse_2(hdr->e_shentsize);
    hdr->e_shnum = reverse_2(hdr->e_shnum);
    hdr->e_shstrndx = reverse_2(hdr->e_shstrndx);
    if(stream == NULL){
        return 0;
    }

    if(fwrite(hdr, sizeof(Elf32_Ehdr), 1, stream) != 1){
        return 0;
    }
    return 1;
}

int write_elf_section_header(FILE *stream, Elf32_Shdr *shdr, Elf32_Off e_shoff, Elf32_Half e_shnum){
    // returns 1 if there is a problem. 1 otherwise.
    for (int iterat_sections = 0; iterat_sections<e_shnum; iterat_sections ++){
        swap_Elf32_Shdr(&(shdr[iterat_sections]));
    }
    
    if(stream == NULL || shdr == NULL || e_shnum == 0){
        return 0;
    }
    printf("eshoff:%d sizeof %lu eshnum %d\n",e_shoff,sizeof(Elf32_Shdr),e_shnum);
    if(fseek(stream, e_shoff, SEEK_SET) != 0){
        return 0;
    }
    

    return fwrite(shdr, sizeof(Elf32_Shdr), e_shnum, stream);
}


int write_elf_shstrtab(FILE *stream, char* shstrtab, Elf32_Off offset, Elf32_Half size){
    fseek(stream, offset, SEEK_SET);
    return fwrite(shstrtab,sizeof(char),size,stream);
}



/*
int write_section(FILE *stream, Elf32_Shdr *shdr, void *section_data, int section_index){
    if (!stream || !shdr)
        return 0;

    //sections sans données dans le fichier 
    if (shdr[section_index].sh_type == SHT_NULL ||
        shdr[section_index].sh_type == SHT_NOBITS)
        return 1;

    // aller à l’offset final 
    if (fseek(stream, shdr[section_index].sh_offset, SEEK_SET) != 0)
        return 0;

    // écrire le contenu fusionné 
    if (fwrite(section_data, 1, shdr[section_index].sh_size, stream) != shdr[section_index].sh_size)
        return 0;

    return 1;
}
*/

/*
int main(void)
{
    FILE *out = fopen("result.o", "wb");
    if (!out) {
        perror("fopen result.o");
        return 1;
    }

    // --- Hypothèse ---
    //   hdr1 et sht1 sont déjà fusionnés
    //   sh_offset et sh_size sont corrects
    //   les données des sections sont déjà en mémoire

    // 1. écrire le header ELF 
    if (!write_elf_header(out, hdr1)) {
        fprintf(stderr, "Error writing ELF header\n");
        return 1;
    }

    // 2. écrire les sections
    for (int i = 0; i < hdr1->e_shnum; i++) {

        // ignorer les sections sans contenu 
        if (sht1[i].sh_type == SHT_NULL ||
            sht1[i].sh_type == SHT_SYMTAB ||
            sht1[i].sh_type == SHT_STRTAB ||
            sht1[i].sh_type == SHT_REL ||
            sht1[i].sh_type == SHT_RELA)
            continue;

        // ici, section_data[i] = buffer fusionné 
        if (!write_section(out, &sht1[i], section_data[i], i)) {
            fprintf(stderr, "Error writing section %d\n", i);
            return 1;
        }
    }

    // 3. écrire la table des headers de sections 
    if (!write_elf_section_header(out,
                                  sht1,
                                  hdr1->e_shoff,
                                  hdr1->e_shnum)) {
        fprintf(stderr, "Error writing section header table\n");
        return 1;
    }

    fclose(out);
    printf("result.o written successfully\n");
    return 0;
}
*/
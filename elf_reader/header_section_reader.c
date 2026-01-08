

#include "header_section_reader.h"
#include <string.h>

void swap_Elf32_Shdr(Elf32_Shdr *s){
    s->sh_name      = reverse_4(s->sh_name);
    s->sh_type      = reverse_4(s->sh_type);
    s->sh_flags     = reverse_4(s->sh_flags);
    s->sh_addr      = reverse_4(s->sh_addr);
    s->sh_offset    = reverse_4(s->sh_offset);
    s->sh_size      = reverse_4(s->sh_size);
    s->sh_link      = reverse_4(s->sh_link);
    s->sh_info      = reverse_4(s->sh_info);
    s->sh_addralign = reverse_4(s->sh_addralign);
    s->sh_entsize   = reverse_4(s->sh_entsize);
}

void readShtable(FILE *stream, Elf32_Ehdr *hdr, Elf32_Shdr **Shdr){
    assert(stream);
    assert(hdr);
    Elf32_Off Sh_offset = hdr->e_shoff;
    fseek(stream, Sh_offset, SEEK_SET);
    Elf32_Half number_entries = hdr->e_shnum;
    Elf32_Half one_entry_size = hdr->e_shentsize;

    size_t shtaille = number_entries * one_entry_size;
    *Shdr = (Elf32_Shdr*)malloc(shtaille);
    assert(*Shdr);
    if (fread(*Shdr, one_entry_size, number_entries, stream) != number_entries) {
		return;
	}
    //////////// TESTER SI NOTRE ORDI EST BIG ENDIAN /////////////////
    if(is_big_endian() != hdr->e_ident[EI_DATA]){
        for(int i = 0; i < number_entries; i++){
            swap_Elf32_Shdr(&(*Shdr)[i]);
        }
    }

}

     /* Affichage minimal pour validation */
void printShtable(Elf32_Shdr* shtable, Elf32_Ehdr* hdr, FILE *stream){
    int str_tb_offset = shtable[hdr->e_shstrndx].sh_offset;
    
    char section_name[15];
    

    printf("Section Header Table:\n");
    printf("[Nr] Name          Type                   Addr        Offset      Size        |FLAGS: WR AL EX NR|  ES LK IF AL\n");
    for (int i = 0; i < hdr->e_shnum; i++) {
        printf("[%2d] ",i);

        fseek(stream,str_tb_offset+shtable[i].sh_name,SEEK_SET);
        fread(section_name,sizeof(char),14,stream);
        if (strlen(section_name) >12){
            printf("%.8s[..] ",section_name);
        }
        else {
        printf("%-12s ",section_name);
        }
        switch (shtable[i].sh_type){
            case SHT_NULL:
                printf("SHT_NULL                ");
                break;
            case SHT_PROGBITS:
                printf("SHT_PROGBITS            ");
                break;
            case SHT_SYMTAB:
                printf("SHT_SYMTAB              ");
                break;
            case SHT_STRTAB:
                printf("SHT_STRTAB              ");
                break;
            case SHT_RELA:
                printf("SHT_RELA                ");
                break;
            case SHT_HASH:
                printf("SHT_HASH                ");
                break;
            case SHT_DYNAMIC:
                printf("SHT_DYNAMIC             ");
                break;
            case SHT_NOTE:
                printf("SHT_NOTE                ");
                break;
            case SHT_NOBITS:
                printf("SHT_NOBITS              ");
                break;
            case SHT_REL:
                printf("SHT_REL                 ");
                break;
            case SHT_SHLIB:
                printf("SHT_SHLIB               ");
                break;
            case SHT_DYNSYM:
                printf("SHT_DYNSYM              ");
                break;
            case SHT_LOPROC:
                printf("SHT_LOPROC              ");
                break;
            case SHT_HIPROC:
                printf("SHT_HIPROC              ");
                break;
            case SHT_LOUSER:
                printf("SHT_LOUSER              ");
                break;
            case SHT_HIUSER:
                printf("SHT_HIUSER              ");
                break;
            case SHT_ARM_EXIDX:
                printf("SHT_ARM_EXIDX           ");
                break;
            case SHT_ARM_PREEMPTMAP:
                printf("SHT_ARM_PREEMPTMAP      ");
                break;
            case SHT_ARM_ATTRIBUTES:
                printf("SHT_ARM_ATTRIBUTES      ");
                break;
            case SHT_ARM_DEBUGOVERLAY:
                printf("SHT_ARM_DEBUGOVERLAY    ");
                break;
            case SHT_ARM_OVERLAYSECTION:
                printf("SHT_ARM_OVERLAYSECTION  ");
                break;
            default:
                printf("0x%08x      ",shtable[i].sh_type);
                break;
        }
        printf("0x%08x  0x%08x  0x%08x        ",
               shtable[i].sh_addr,
               shtable[i].sh_offset,
               shtable[i].sh_size);

        printf("  %d  ",(shtable[i].sh_flags&(1))!=0);
        printf("%d  ",(shtable[i].sh_flags&(1<<1))!=0);
        printf("%d  ",(shtable[i].sh_flags&(1<<2))!=0);
        printf("%d  ",(shtable[i].sh_flags&(uint32_t)(1<<29))!=0);
        //printf("%04d  ",(shtable[i].sh_flags&(uint32_t)(0xf0000000))>>28);
        printf(" %2d %2d %2d %2d",shtable[i].sh_entsize, shtable[i].sh_link, shtable[i].sh_info, shtable[i].sh_addralign);
        
            
        printf("\n");
    }
    
    
    

}

void printShtable_noname(Elf32_Shdr* shtable, Elf32_Ehdr* hdr){
    

    printf("Section Header Table:\n");
    printf("[Nr] Type                   Addr        Offset      Size        |FLAGS: WR AL EX NR|  ES LK IF AL\n");
    for (int i = 0; i < hdr->e_shnum; i++) {
        printf("[%2d] ",i);
        switch (shtable[i].sh_type){
            case SHT_NULL:
                printf("SHT_NULL                ");
                break;
            case SHT_PROGBITS:
                printf("SHT_PROGBITS            ");
                break;
            case SHT_SYMTAB:
                printf("SHT_SYMTAB              ");
                break;
            case SHT_STRTAB:
                printf("SHT_STRTAB              ");
                break;
            case SHT_RELA:
                printf("SHT_RELA                ");
                break;
            case SHT_HASH:
                printf("SHT_HASH                ");
                break;
            case SHT_DYNAMIC:
                printf("SHT_DYNAMIC             ");
                break;
            case SHT_NOTE:
                printf("SHT_NOTE                ");
                break;
            case SHT_NOBITS:
                printf("SHT_NOBITS              ");
                break;
            case SHT_REL:
                printf("SHT_REL                 ");
                break;
            case SHT_SHLIB:
                printf("SHT_SHLIB               ");
                break;
            case SHT_DYNSYM:
                printf("SHT_DYNSYM              ");
                break;
            case SHT_LOPROC:
                printf("SHT_LOPROC              ");
                break;
            case SHT_HIPROC:
                printf("SHT_HIPROC              ");
                break;
            case SHT_LOUSER:
                printf("SHT_LOUSER              ");
                break;
            case SHT_HIUSER:
                printf("SHT_HIUSER              ");
                break;
            case SHT_ARM_EXIDX:
                printf("SHT_ARM_EXIDX           ");
                break;
            case SHT_ARM_PREEMPTMAP:
                printf("SHT_ARM_PREEMPTMAP      ");
                break;
            case SHT_ARM_ATTRIBUTES:
                printf("SHT_ARM_ATTRIBUTES      ");
                break;
            case SHT_ARM_DEBUGOVERLAY:
                printf("SHT_ARM_DEBUGOVERLAY    ");
                break;
            case SHT_ARM_OVERLAYSECTION:
                printf("SHT_ARM_OVERLAYSECTION  ");
                break;
            default:
                printf("0x%08x      ",shtable[i].sh_type);
                break;
        }
        printf("0x%08x  0x%08x  0x%08x        ",
               shtable[i].sh_addr,
               shtable[i].sh_offset,
               shtable[i].sh_size);

        printf("  %d  ",(shtable[i].sh_flags&(1))!=0);
        printf("%d  ",(shtable[i].sh_flags&(1<<1))!=0);
        printf("%d  ",(shtable[i].sh_flags&(1<<2))!=0);
        printf("%d  ",(shtable[i].sh_flags&(uint32_t)(1<<29))!=0);
        //printf("%04d  ",(shtable[i].sh_flags&(uint32_t)(0xf0000000))>>28);
        printf(" %2d %2d %2d %2d",shtable[i].sh_entsize, shtable[i].sh_link, shtable[i].sh_info, shtable[i].sh_addralign);
        
            
        printf("\n");
    }
    
    
    

}
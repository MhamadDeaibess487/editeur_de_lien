#include "section_reader.h"



// void readSection(FILE *stream, Elf32_Ehdr *hdr, Elf32_Shdr **Shdr, Elf32_Sec *S,int i){//i is to get the specific section that we want
//         S->s_index=Shdr[i]->sh_name;
//         S->s_name= Shdr[hdr->e_shstrndx]
//         S->sh_type=Shdr[i]->sh_type;
//         S->sh_offset=Shdr[i]->sh_offset;
//         S->sh_entsize=Shdr[i]->sh_entsize;
//         S->sh_size=Shdr[i]->sh_size;   
// }

void print_section(FILE* stream, int section_index, Elf32_Shdr * Shdr){
    printf("\nDumping section: %d, type ", section_index);
    switch (Shdr[section_index].sh_type){
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
                printf("0x%08x      ",Shdr[section_index].sh_type);
                break;
        }
    printf("\n");
    printf("Offset: 0x%.8x\n",Shdr[section_index].sh_offset);
    fseek(stream, Shdr[section_index].sh_offset,SEEK_SET);
    unsigned char c;
    for (int i =0; i<Shdr[section_index].sh_size;i++){  
        fread(&c,sizeof(unsigned char),1,stream);
        if (i%16==0 ){
            printf("\n0x%08x    ",i);
        }
        if (i%4==0){
            printf(" ");
        }
        printf("%02x",c);
    }
    printf("\n");
}



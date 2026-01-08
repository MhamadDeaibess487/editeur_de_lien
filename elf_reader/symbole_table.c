#include "symbole_table.h"
#include <stdlib.h>
#include <stdio.h>


void swap_Elf32_Sym(Elf32_Sym *s)
{
    s->st_name  = reverse_4(s->st_name);
    s->st_value = reverse_4(s->st_value);
    s->st_size  = reverse_4(s->st_size);
    s->st_shndx = reverse_2(s->st_shndx);
}

Elf32_Sym* read_symbole_table(FILE *stream,Elf32_Shdr *Shdr,Elf32_Ehdr *hdr, char *shstrtab, char** strtab){
    for (int j = 0; j < hdr->e_shnum; j++) {
        
        if (Shdr[j].sh_type == SHT_SYMTAB) {

            int nb_symbols = Shdr[j].sh_size / Shdr[j].sh_entsize;

            //Reading symbol table
            printf("symtab malloc size: %d\n",Shdr[j].sh_size);
            Elf32_Sym *symtab = malloc(Shdr[j].sh_size);
            if (!symtab) {
                printf("\n malloc symtab \n");
                free(symtab);
                return NULL;
            }
            fseek(stream, Shdr[j].sh_offset, SEEK_SET);
            fread(symtab, Shdr[j].sh_entsize, nb_symbols, stream);

            // reading string table associated to the symbol table
            Elf32_Shdr str_sh = Shdr[Shdr[j].sh_link];
            
            *strtab = malloc(str_sh.sh_size);
            if (!*strtab) {
                printf("\n malloc strtab \n");
                free(symtab);
                free(*strtab);
                return NULL;
            }
            fseek(stream, str_sh.sh_offset, SEEK_SET);
            fread(*strtab, 1, str_sh.sh_size, stream);
            return symtab;
        }
    }
    return NULL;
        

}

void print_symbole_table(Elf32_Shdr *Shdr,Elf32_Ehdr *hdr, char *shstrtab, Elf32_Sym* symtab, char* strtab)
{
    Elf32_Sym s;
    for (int j = 0; j < hdr->e_shnum; j++) {
        
        if (Shdr[j].sh_type == SHT_SYMTAB) {
            int nb_symbols = Shdr[j].sh_size / Shdr[j].sh_entsize;

            printf("\nSymbol table '.symtab' contains %d entries:\n", nb_symbols);
            printf("Num:    Value  Size Type     Bind   Vis      Ndx Name\n");

            // Printing symbol table entries
            for (int i = 0; i < nb_symbols; i++) {
                 s = symtab[i];
                

                if (!is_big_endian()) {
                    swap_Elf32_Sym(&s);
                }

                printf("%3d: %08x %5d ",
                       i, s.st_value, s.st_size);

                //TYPE
                switch (ELF32_ST_TYPE(s.st_info)) {
                    case STT_NOTYPE:  printf("NOTYPE ");  break;
                    case STT_OBJECT:  printf("OBJECT ");  break;
                    case STT_FUNC:    printf("FUNC   ");  break;
                    case STT_SECTION: printf("SECTION");  break;
                    case STT_FILE:    printf("FILE   ");  break;
                    default:          printf("UNKNOWN");  break;
                }

                // Bind
                switch (ELF32_ST_BIND(s.st_info)) {
                    case STB_LOCAL:  printf(" LOCAL "); break;
                    case STB_GLOBAL: printf(" GLOBAL"); break;
                    case STB_WEAK:   printf(" WEAK  "); break;
                    default:         printf(" OTHER "); break;
                }

                // Visibility
                printf(" DEFAULT ");

                // NDX

                switch (s.st_shndx){
                    case (SHN_ABS):
                        printf(" ABS ");
                        break;
                    case (SHN_UNDEF):
                        printf(" UND ");
                        break;
                    case (SHN_COMMON):
                        printf(" CMN ");
                        break;
                    default:
                        printf(" %3d ", s.st_shndx);
                        break; 
                }
                //NOM
                if (ELF32_ST_TYPE(s.st_info) == STT_SECTION && s.st_shndx < hdr->e_shnum) {

                    Elf32_Shdr *sec = &Shdr[s.st_shndx];
                    printf("%s (%d)\n", shstrtab + sec->sh_name,sec->sh_name);
                }

                else
                    printf("%s\n", strtab + s.st_name);
            }

            //free(strtab);
            //free(symtab);
        }
    }
}


#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "debug.h"
#include "units_elf32.h"
#include "elf.h"
#include "header_reader.h"
#include "header_section_reader.h"  
#include "section_reader.h"
#include "symbole_table.h"
#include "relocation_reader.h"

    
void usage(char* filename){
    printf("%s <option(s) <file>\n", filename);
    printf("\n Options:\n");
    printf("    -h      :   display the ELF header\n");
    printf("    -S      :   display the section header table\n");
    printf("    -x <int>:   dump a section\n");
    printf ("    -s     :   display the symbol table\n");
    printf ("    -r     :   display the relocations\n");
    return;
}

int main(int argc, char *argv[]) {
    if (argc <3){
        usage(argv[0]);
        return 0;
    }
    FILE *f = fopen(argv[argc-1], "r");
    Elf32_Ehdr *hdr = ELF_header_reader(f);
    if (!hdr) {
        fprintf(stderr, "ELF file error: couldn't read file or is not ELF\n");
        return 1;
        fclose(f);
    }

    //Reading section header table 
    Elf32_Shdr *shtable = NULL;
    readShtable(f, hdr, &shtable);

    //Section header string section header
    Elf32_Shdr *shstr_shdr = &shtable[hdr->e_shstrndx];

    //Section header string section
    char *shstrtab = malloc(shstr_shdr->sh_size);
    fseek(f, shstr_shdr->sh_offset, SEEK_SET);
    fread(shstrtab, 1, shstr_shdr->sh_size, f);

    //table for string names of symbols
    char *sym_strtab = NULL;

    //table of symbols
    Elf32_Sym* symtab = NULL;

    //for testing
    // int total_rel = 0;
    // Elf32_Rel*  rel = NULL;
    // Elf32_Rela*  rela = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "hSx:srk")) != -1){
        switch(opt){
            case 'h':
                print_header(hdr);
                break;
            case 'S':
                printShtable(shtable, hdr,f);
                break;
            /*case 'x':
                print_section(f,atoi(optarg),shtable);
                break;
            */
            case 'x': {
                char *end;
                long idx = strtol(optarg, &end, 10);

                if (*end != '\0' || idx < 0 || idx >= hdr->e_shnum) {
                    fprintf(stderr,
                        "Error: section index %s out of range (0..%d)\n",
                        optarg, hdr->e_shnum - 1);
                    return 1;
                }

                print_section(f, (int)idx, shtable);
                break;
            }
            case 's':
                symtab = read_symbole_table(f, shtable,hdr,shstrtab, &sym_strtab);
                print_symbole_table(shtable, hdr, shstrtab,symtab, sym_strtab);
                break;
            case 'r':
                symtab = read_symbole_table(f, shtable,hdr,shstrtab, &sym_strtab);
                print_relocations(f, shtable, hdr, shstrtab,symtab, sym_strtab);
                break;
            case 'k': //for testing, to delete later
                // total_rel = read_relocations(f,hdr, shtable,shstrtab,&rela, &rel);
                // for (int relj = 0; relj<total_rel; relj++){
                //     printf("%08x  %08x  ",relj[rel].r_offset,relj[rel].r_info);
                
                // uint32_t type = ELF32_R_TYPE(relj[rel].r_info);

                // switch (type){
                
                // case R_ARM_ABS32:
                //     printf("R_ARM_ABS32    ");
                //     break;
                
                
                // case R_ARM_CALL:
                //     printf("R_ARM_CALL     ");
                //     break;
                
                // case R_ARM_JUMP24:
                //     printf("R_ARM_JUMP24   ");
                //     break;
                
                // default:
                //     printf("%08d        ",type);
                // }
                // printf("\n");
                // }
                break;
            case '?':
                usage(argv[1]);
                return 0;
            
        }
    }     
    free(shtable);
    free(hdr);
    
    free(sym_strtab);
    free(symtab);

    fclose(f);

    return 0;

    


}
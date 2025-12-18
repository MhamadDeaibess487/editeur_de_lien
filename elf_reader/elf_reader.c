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


void usage(char* filename){
    printf("%s <option(s) <file>\n", filename);
    printf("\n Options:\n");
    printf("    -h      :   display the ELF header\n");
    printf("    -S      :   display the section header table\n");
    printf("    -x <int>:   dump a section\n");
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

    Elf32_Shdr *shtable = NULL;
    readShtable(f, hdr, &shtable);
    

    int opt;
    while ((opt = getopt(argc, argv, "hSx:")) != -1){
        switch(opt){
            case 'h':
                print_header(hdr);
                break;
            case 'S':
                printShtable(shtable, hdr,f);
                break;
            case 'x':
                print_section(f,atoi(optarg),shtable);
                break;
            case '?':
                usage(argv[1]);
                return 0;
        }
    }     
    free(shtable);
    free(hdr);
    fclose(f);

    return 0;

    


}
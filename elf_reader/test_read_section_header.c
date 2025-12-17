#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "units_elf32.h"
#include "elf.h"
#include "header_reader.h"
#include <assert.h>
#include "header_section_reader.h"  
#include "section_reader.h"


int main(int argc, char *argv[]) {

    FILE *f = fopen(argv[1], "r");

    Elf32_Ehdr *hdr = ELF_header_reader(f);
    if (!hdr) {
        fprintf(stderr, "Erreur lecture ELF header\n");
        return 1;
        fclose(f);
    }
    print_header(hdr);
    

    /* Phase 2 : lecture de la table des sections */

    
    Elf32_Shdr *shtable = NULL;
    readShtable(f, hdr, &shtable);
    printShtable(shtable, hdr,f);
   
    /*Phase 3: reading the raw content of a section (argv[2]) DUMP*/
    f = fopen(argv[1], "r");
    print_section(f,atoi(argv[2]),shtable);

    free(shtable);
    free(hdr);
    fclose(f);

    return 0;

    


}
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "util.h"
#include "units_elf32.h"
#include "elf.h"
#define __BIGENDIAN__

//WILL ONLY READ CORRECTLY IF FILE IS 32 BITS
Elf32_Ehdr* ELF_header_reader(FILE *stream){

    Elf32_Ehdr* header=malloc(sizeof(Elf32_Ehdr));
    if(!header){
        free(header);
        return NULL;
    }
    fread((header->e_ident),sizeof(unsigned_char),EI_NIDENT,stream);

    //check if the header is ELF
    if((header->e_ident[EI_MAG0]!=127)||(header->e_ident[EI_MAG1]!=69)||(header->e_ident[EI_MAG2]!=76)||(header->e_ident[EI_MAG3]!=70)){
        free(header);
        printf("error \n");
        return NULL;
    }

    fread(&(header->e_type),sizeof(Elf32_Half),1,stream);
    fread(&(header->e_machine),sizeof(Elf32_Half),1,stream);
    fread(&(header->e_version),sizeof(Elf32_Word),1,stream);
    fread(&(header->e_entry),sizeof(Elf32_Addr),1,stream);
    fread(&(header->e_phoff),sizeof(Elf32_Off),1,stream);
    fread(&(header->e_shoff),sizeof(Elf32_Off),1,stream);
    fread(&(header->e_flags),sizeof(Elf32_Word),1,stream);
    fread(&(header->e_ehsize),sizeof(Elf32_Half),1,stream);
    fread(&(header->e_phentsize),sizeof(Elf32_Half),1,stream);
    fread(&(header->e_phnum),sizeof(Elf32_Half),1,stream);
    fread(&(header->e_shentsize),sizeof(Elf32_Half),1,stream);
    fread(&(header->e_shnum),sizeof(Elf32_Half),1,stream);
    fread(&(header->e_shstrndx),sizeof(Elf32_Half),1,stream);
    

    #ifdef __BIGENDIAN__
        header->e_type = reverse_2(header->e_type);
        header->e_machine = reverse_2(header->e_machine);
        header->e_version = reverse_4(header->e_version);
        header->e_entry = reverse_4(header->e_entry);
        header->e_phoff = reverse_4(header->e_phoff);
        header->e_shoff = reverse_4(header->e_shoff);
        header->e_flags = reverse_4(header->e_flags);
        header->e_ehsize = reverse_2(header->e_ehsize);
        header->e_phentsize = reverse_2(header->e_phentsize);
        header->e_phnum = reverse_2(header->e_phnum);
        header->e_shentsize = reverse_2(header->e_shentsize);
        header->e_shnum = reverse_2(header->e_shnum);
        header->e_shstrndx = reverse_2(header->e_shstrndx);
    #endif


    return header;
    }

void print_header(Elf32_Ehdr *header){
    
    printf("MAGIC NUMBERS:                      ");
    for (int i=0; i<EI_NIDENT;i++){
        printf("%02x ",header->e_ident[i]);
    }

    printf("\nCLASS:                              ");
    switch (header->e_ident[EI_CLASS]){
        case ELFCLASS32:
            printf("ELFCLASS32\n");
            break;
        case ELFCLASS64:
            printf("ELFCLASS64\n");
            break;
        default:
            printf("ELFCLASSNONE\n");
            break;
    }

    printf("DATA:                               ");
    switch (header->e_ident[EI_DATA]){
        case ELFDATA2LSB :
            printf("ELFDATA2LSB (Little endian)\n");
            break;
        case ELFDATA2MSB :
            printf("ELFDATA2MSB (2's complement, Big endian)\n");
            break;
        default :
            printf("ELFDATANONE (invalid)\n");
            break;
    }

    printf("VERSION:                            ");
    switch (header->e_version){
        case EV_CURRENT:
            printf("EV_CURRENT\n");
            break;
        default:
            printf("EV_NONE (invalid)\n");
            break;
    }

    printf("TYPE:                               ");
    switch (header->e_type){
        case ET_REL:
            printf("ET_REL (relocatable file)\n");
            break;
        case ET_EXEC:
            printf("ET_EXEC (executable file)\n");
            break;
        case ET_DYN:
            printf("ET_DYN (shared object file)\n");
            break;
        case ET_CORE:
            printf("ET_CORE (core file)\n");
            break;
        case ET_LOPROC:
            printf("ET_LOPROC\n");
            break;
        case ET_HIPROC:
            printf("ET_HIPROC\n");
            break;
        default: 
            printf("ET_NONE/unrecognizable\n");
            break;
    }

    printf("MACHINE:                            ");
    switch (header->e_machine){
        case EM_M32:
            printf("EM_M32 AT&T WE 32100");
            break;
        case EM_SPARC:
            printf("EM_SPARC SPARC");
            break;
        case EM_386:
            printf("EM_386 Intel Architecture");
            break;
        case EM_68K:
            printf("EM_68K Motorola 68000");
            break;
        case EM_88K:
            printf("EM_88K Motorola 88000");
            break;
        case EM_860:
            printf("EM_860 Intel 80860");
            break;
        case EM_MIPS:
            printf("EM_MIPS MIPS RS3000 Big-Endian");
            break;
        case EM_MIPS_RS4_BE:
            printf("EM_MIPS_RS4_BE MIPS RS4000 Big-Endian");
            break;
        case EM_ARM:
            printf("EM_ARM ARM");
            break;
        default:
            printf("ET_NONE/RESERVED");
            break;
    }
    printf("\n");

    printf("ENTRY ADDRESS:                      0x%08x\n",header->e_entry);
    printf("PROGRAM HEADER START:               0x%08x\n",header->e_phoff);
    printf("SECTION HEADER START:               0x%08x\n",header->e_shoff);
    printf("FLAGS:                              0x%08x\n",header->e_flags);
    printf("HEADER SIZE:                        %d",header->e_ehsize);
    switch(header->e_ehsize){
        case 52:
            printf(" (32 bits)\n");
            break;
        case 64:
            printf(" (64 bits)\n");
        default: 
            printf(" (NOT RECOGNIZED)\n");
    }
    printf("PROGRAM HEADERs SIZE:               %d\n",header->e_phentsize);
    printf("PROGRAM HEADERS NUMBER:             %d\n",header->e_phnum);
    printf("SECTION HEADERS SIZEZ:              %d\n",header->e_shentsize);
    printf("SECTION HEADERS NUMBERS:            %d\n",header->e_shnum);
    printf("SECTION HEADER STRING TABLE INDEX:  %d\n",header->e_shstrndx);
}

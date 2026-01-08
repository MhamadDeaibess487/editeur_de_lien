#ifndef __ELFSTRUCT__
#define __ELFSTRUCT__
#include "units_elf32.h"

//e_ident indexes
#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_PAD 7
#define EI_NIDENT 16

//e_ident[EI_CLASS]
#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELFCLASS64 2

//e_ident[EI_DATA]
#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

//e_type
#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff

//e_machine
#define EM_M32 1
#define EM_SPARC 2
#define EM_386 3
#define EM_68K 4
#define EM_88K 5
#define EM_860 7
#define EM_MIPS 8
#define EM_MIPS_RS4_BE 10
#define EM_ARM 40

//e_version
#define EV_NONE 0
#define EV_CURRENT 1

//Structure of ELF header
typedef struct {
    unsigned_char   e_ident[EI_NIDENT]; 
    Elf32_Half      e_type;
    Elf32_Half      e_machine;
    Elf32_Word      e_version;
    Elf32_Addr      e_entry;
    Elf32_Off       e_phoff;
    Elf32_Off       e_shoff;
    Elf32_Word      e_flags;
    Elf32_Half      e_ehsize;
    Elf32_Half      e_phentsize;
    Elf32_Half      e_phnum;
    Elf32_Half      e_shentsize;
    Elf32_Half      e_shnum;
    Elf32_Half      e_shstrndx;
} Elf32_Ehdr;

//sh_type
#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7fffffff
#define SHT_LOUSER 0x80000000
#define SHT_HIUSER 0xffffffff
#define SHT_ARM_EXIDX 0x70000001
#define SHT_ARM_PREEMPTMAP 0x70000002
#define SHT_ARM_ATTRIBUTES 0x70000003
#define SHT_ARM_DEBUGOVERLAY 0x70000004
#define SHT_ARM_OVERLAYSECTION 0x70000005

//sh_flags
#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKPROC 0xf0000000

//Section header structure
typedef struct {
   Elf32_Word sh_name;
   Elf32_Word sh_type;
   Elf32_Word sh_flags;
   Elf32_Addr sh_addr;
   Elf32_Off sh_offset;
   Elf32_Word sh_size;
   Elf32_Word sh_link;
   Elf32_Word sh_info;
   Elf32_Word sh_addralign;
   Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct{
    Elf32_Word s_index;
    char* s_name;
    Elf32_Word sh_type;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_entsize;
    
}Elf32_Sec; 



#define ELF32_ST_BIND(i) ((i)>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

//ELF32_ST_BIND
#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2
#define STB_LOPROC 13
#define STB_HIPROC 15

//ELF32_ST_TYPE
#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4
#define STT_LOPROC 13
#define STT_HIPROC 15

//shndx special values
#define SHN_UNDEF 0
#define SHN_LORESERVE 0xff00
#define SHN_LOPROC 0xff00
#define SHN_HIPROC 0xff1f
#define SHN_ABS 0xfff1
#define SHN_COMMON 0xfff2
#define SHN_HIRESERVE 0xffff

//symbol table entry
typedef struct {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;


//Relocation entry without addend
typedef struct {
 Elf32_Addr r_offset;
 Elf32_Word r_info;
} Elf32_Rel;

//Relocation entry with addend
typedef struct {
 Elf32_Addr r_offset;
 Elf32_Word r_info;
 Elf32_Sword r_addend;
} Elf32_Rela;

#define ELF32_R_SYM(i) ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))


//ARM Reloc types
#define R_ARM_NONE              0
#define R_ARM_PC24              1
#define R_ARM_ABS32             2
#define R_ARM_REL32             3
#define R_ARM_PC13              4
#define R_ARM_ABS16             5
#define R_ARM_ABS12             6
#define R_ARM_THM_ABS5          7
#define R_ARM_ABS8              8
#define R_ARM_SBREL32           9
#define R_ARM_THM_CALL          10
#define R_ARM_THM_PC8           11
#define R_ARM_BREL_ADJ          12
#define R_ARM_TLS_DESC          13
#define R_ARM_THM_SWI8          14
#define R_ARM_XPC25             15
#define R_ARM_THM_XPC22         16
#define R_ARM_TLS_DTPMOD32      17
#define R_ARM_TLS_DTPOFF32      18
#define R_ARM_TLS_TPOFF32       19
#define R_ARM_COPY              20
#define R_ARM_GLOB_DAT          21
#define R_ARM_JUMP_SLOT         22
#define R_ARM_RELATIVE          23
#define R_ARM_GOTOFF32          24
#define R_ARM_BASE_PREL         25
#define R_ARM_GOT_BREL          26
#define R_ARM_PLT32             27
#define R_ARM_CALL              28
#define R_ARM_JUMP24            29
#define R_ARM_THM_JUMP24        30
#define R_ARM_BASE_ABS          31
#define R_ARM_ALU_PCREL_7_0     32
#define R_ARM_ALU_PCREL_15_8    33
#define R_ARM_ALU_PCREL_23_15   34
#define R_ARM_LDR_SBREL_11_0    35
#define R_ARM_ALU_SBREL_19_12   36
#define R_ARM_ALU_SBREL_27_20   37
#define R_ARM_TARGET1           38
#define R_ARM_SBREL31           39
#define R_ARM_V4BX              40
#define R_ARM_TARGET2           41
#define R_ARM_PREL31            42
#define R_ARM_MOVW_ABS_NC       43
#define R_ARM_MOVT_ABS          44
#define R_ARM_MOVW_PREL_NC      45
#define R_ARM_MOVT_PREL         46
#define R_ARM_THM_MOVW_ABS_NC   47
#define R_ARM_THM_MOVT_ABS      48
#define R_ARM_THM_MOVW_PREL_NC  49
#define R_ARM_THM_MOVT_PREL     50
#define R_ARM_THM_JUMP19        51
#define R_ARM_THM_JUMP6         52
#define R_ARM_THM_ALU_PREL_11_0 53
#define R_ARM_THM_PC12          54
#define R_ARM_ABS32_NOI         55
#define R_ARM_REL32_NOI         56
#define R_ARM_ALU_PC_G0_NC      57
#define R_ARM_ALU_PC_G0         58
#define R_ARM_ALU_PC_G1_NC      59
#define R_ARM_ALU_PC_G1         60
#define R_ARM_ALU_PC_G2         61
#define R_ARM_LDR_PC_G1         62
#define R_ARM_LDR_PC_G2         63
#define R_ARM_LDRS_PC_G0        64
#define R_ARM_LDRS_PC_G1        65
#define R_ARM_LDRS_PC_G2        66
#define R_ARM_LDC_PC_G0         67
#define R_ARM_LDC_PC_G1         68
#define R_ARM_LDC_PC_G2         69
#define R_ARM_ALU_SB_G0_NC      70
#define R_ARM_ALU_SB_G0         71
#define R_ARM_ALU_SB_G1_NC      72
#define R_ARM_ALU_SB_G1         73
#define R_ARM_ALU_SB_G2         74
#define R_ARM_LDR_SB_G0         75
#define R_ARM_LDR_SB_G1         76
#define R_ARM_LDR_SB_G2         77
#define R_ARM_LDRS_SB_G0        78
#define R_ARM_LDRS_SB_G1        79
#define R_ARM_LDRS_SB_G2        80
#define R_ARM_LDC_SB_G0         81
#define R_ARM_LDC_SB_G1         82
#define R_ARM_LDC_SB_G2         83
#define R_ARM_MOVW_BREL_NC      84
#define R_ARM_MOVT_BREL         85
#define R_ARM_MOVW_BREL         86
#define R_ARM_THM_MOVW_BREL_NC  87
#define R_ARM_THM_MOVT_BREL     88
#define R_ARM_THM_MOVW_BREL     89
#define R_ARM_TLS_GOTDESC       90
#define R_ARM_TLS_CALL          91
#define R_ARM_TLS_DESCSEQ       92
#define R_ARM_THM_TLS_CALL      93
#define R_ARM_PLT32_ABS         94
#define R_ARM_GOT_ABS           95
#define R_ARM_GOT_PREL          96
#define R_ARM_GOT_BREL12        97
#define R_ARM_GOTOFF12          98
#define R_ARM_GOTRELAX          99
#define R_ARM_GNU_VTENTRY       100
#define R_ARM_GNU_VTINHERIT     101
#define R_ARM_THM_PC11          102
#define R_ARM_THM_PC9           103
#define R_ARM_TLS_GD32          104
#define R_ARM_TLS_LDM32         105
#define R_ARM_TLS_LDO32         106
#define R_ARM_TLS_IE32          107
#define R_ARM_TLS_LE32          108
#define R_ARM_TLS_LDO12         109
#define R_ARM_TLS_LE12          110
#define R_ARM_TLS_IE12GP        111

#endif
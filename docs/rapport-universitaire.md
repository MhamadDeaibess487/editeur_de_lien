# Prog5Projet — rapport universitaire (archive)

> Ce document conserve le rapport et le suivi d’origine, avec les crédits,
> les exemples et les conclusions de l’équipe. **Prog5Projet** est l’ancien
> titre du README de ce dépôt, désormais présenté comme « Éditeur de liens ELF32 ARM ».
> Pour compiler et essayer les programmes, consulter le [guide utilisateur](../README.md).
> Les affirmations de réussite des tests ci-dessous sont historiques ; elles ne
> constituent pas une validation actuelle. Voir les [vérifications effectuées](verification.md)
> et les [limitations actuelles](../README.md#limitations-et-travaux-incomplets).
>
> Les commandes et signatures sont conservées telles qu’elles ont été rédigées,
> y compris la seconde rubrique « Usage for elf_reader » qui concerne en réalité
> `elf_fusion`. Le fichier `elf.pdf` cité dans le texte n’est pas présent dans le
> dépôt ; cette référence est conservée sans lui attribuer une source supposée.





## Progress tracking:
- **15/12/2025**: Partially completed ELF header reader and section table reader
- **16/12/2025**: ELF header reader is complete
- **17/12/2025**: Section header table printing is complete
- **18/12/2025**: Added fully functional section dumper
- **20/12/2025**: Added fully functional symbol table reader
- **01/01/2026**: The entirety of phase 1 is completed
- **05/01/2026**: Barebone structure for step 6, barebones for step 7
- **07/01/2026**: Completing step 6 and 7, working on 8 and 9
- **08/01/2026** : Completion of ELF features

## Repartition of work:
#### Programming the main program:
- Step 1: Deaibess, Saab, Le
- Step 2: Jaber, Hijazi, Le
- Step 3: Le
- Step 4: Deaibess, Le, Jaber
- Step 5: Deaibess, Le, Jaber, Saab
- Step 6: Saab, Le
- Step 7: Deaibess, Le
- Step 8: Saab, Deaibess, Le
- Step 9: Jaber, Le
#### General testing:
-  Le, Hijazi
#### Writing test scripts
- Hijazi
#### Writing this report
- Le, Deaibess, Saab, Jaber, Hijazi
#### Added humour
- *Le*
---
### Current programs:
We use a mix of header files for definitions and .c files for functions (like any other sane human beings) 

The main program for phase 1 is located in `elf_reader.c` where arguments are taken. Below are brief descriptions of each files:
#### Header files:
-  `elf.h`: definitions for the ELF format, including but not limited to: `Elf32_Ehdr`
-  `units_elf32.h`: definitions of the types, for example `Elf32_Word`
-  `util.h`: given file, containing a function to determine if a file is big endian
#### .c files:
- `elf_reader.c` : main program for reading ELF
- `header_reader.c`: functions to read and print the ELF header
- `header_section_reader`: functions to read and print the Section headers table
- `section_reader.c`: functions to dump a section
- `symbole_table.c`: functions to read and print the symbol table
- `relocation_reader.c`:  functions to display relocations
- `util.c`: given file

### Usage for elf_reader
In the `/elf_reader` directory, compile with:
```
$make
```
This should compile `elf_reader`, whose usage is:
```
$./elf_reader <option(s)> <file_name>
```
The current options are :  
-    -h      :   display the ELF header
-    -S      :   display the section header table
-    -x <int>:   dump a section
-    -s      :   display the symbol table
-    -r      :   display  the relocation entries


Onto the second program `elf_fusion`, we reuse the old `.c` and created some more:
- `elf_reader_headers.h`: header file to include all previous `.h` *(includeception)*
- `elf_fusion.c`: main program, read two files and output one relocatable object file
- `section_fusion.c`: functions to merge 2 section header tables
- `symbole_fusion.c`: fusion of symbol table and its according string table
- `relocation_fusion.c`:  responsable for fusing relocation entries
- `elf_writing.c`: used for outputting and ELF file.

### Usage for elf_reader
In the `/elf_fusion` directory, compile with:
```
$make
```
This should compile `elf_fusion`, whose usage is:
```
$./elf_reader <file1> <file2> <outputfile (optional)>
```
If only two files are given, it will not write a result file.
## Phase 1


### Abstracts
There are three main types of object files  
- A relocatable file  
- An executable file  
- A shared object file  

An object file can be seen from two sides: linking view and execution view (figure 1-1).

![alt text](images/image-1.png)  

The linking view is used for linking relocatable files and for debugging, because it is based on sections.
The execution view is used by the OS loader and the dynamic linker to map segments of executables and shared objects into memory.
(sections and segments to be explained later)
### Step 1

#### A. What we are dealing with

`arm-none-eabi-readelf -h file1.o` gives the following header as output:
```
ELF Header:
  Magic:   7f 45 4c 46 01 02 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF32
  Data:                              2's complement, big endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              REL (Relocatable file)
  Machine:                           ARM
  Version:                           0x1
  Entry point address:               0x0
  Start of program headers:          0 (bytes into file)
  Start of section headers:          1376 (bytes into file)
  Flags:                             0x5000000, Version5 EABI
  Size of this header:               52 (bytes)
  Size of program headers:           0 (bytes)
  Number of program headers:         0
  Size of section headers:           40 (bytes)
  Number of section headers:         23
  Section header string table index: 22
```
The objective right now is to comprehend how it is represented in the binary form, thus studying `elf.pdf` is necessary. 

AN ELF Header lets us know how an elf file is organized (size,small or big endian endoding all that stuff detail in the elf.pdf)
The following contains the definition of an ELF header, cited from `elf.pdf`  

```
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
```
Further descriptions of each type is of course, within the documents and will not be included here. The purpose is to summarize key information so further relecture won't take as much time.


To note:  
-  The `e_machine` code for ARM-based machine is (40, 0x28), *since the scope of the project is limited to ARM, we will not discuss other types.*

-  `e_ehsize` dictates how many bits the object is based on, 52 bytes for 32-bits and 64 bytes for 64-bits.

#### B. C Implementation for header reader

**The approach**: It is certain that the header is at the beginning of the file, and all its components are well defined as per the documents. The plan is to read, according to the structures, all the bytes of the header, using `fread` or any equivalence.

We define the same structure type `ELF32_Ehdr` in C, and a function `ELF_header_reader()` who returns said struct by reading the file. 
#### C. Displaying the header

This section will briefly explain how each parameter is pulled from the header.  
- **MAGIC NUMBERS** Is the table `e_ident`, displayed in hexadecimal.
- **CLASS** and **DATA** are pulled from `e_ident` with the macro `EI_CLASS` and `EI_DATA` already defined.
- **VERSION** has 2 occurences. One in `e_indent[EI_VERSION]` who remains 1 (`EV_CURRENT`) as per documents consulted. One in `e_version`, which is displayed here.
- The rest of the parameter correspond to their tags defined in `Elf32_Ehdr`.

The final output is as below, do note that this version is **32 bits**, so while it is capable of identifying if a file is in 64 bits, further information displayed **WILL BE INCORRECT**.

```
MAGIC NUMBERS:                      7f 45 4c 46 01 02 01 00 00 00 00 00 00 00 00 00 
CLASS:                              ELFCLASS32
DATA:                               ELFDATA2MSB (2's complement, Big endian)
VERSION:                            EV_CURRENT
TYPE:                               ET_REL (relocatable file)
MACHINE:                            EM_ARM ARM
ENTRY ADDRESS:                      0x00000000
PROGRAM HEADER START:               0x00000000
SECTION HEADER START:               0x00000558
FLAGS:                              0x05000000
HEADER SIZE:                        52 (32 bits)
PROGRAM HEADERs SIZE:               0
PROGRAM HEADERS NUMBER:             0
SECTION HEADERS SIZEZ:              40
SECTION HEADERS NUMBERS:            23
SECTION HEADER STRING TABLE INDEX:  22
```
___
### Step 2: 

#### Section Header Table

After parsing the ELF header, the next step is to read the **Section Header Table**, which describes all sections present in the file.  
Using the fields `e_shoff`, `e_shnum`, and `e_shentsize` from `Elf32_Ehdr`, we can locate the table in the binary and iterate over each `Elf32_Shdr` entry.

#### A. The structure
Section definition from the pdf:
```
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
```

Each `Elf32_Shdr` provides metadata about a section (type, size, file offset, flags, etc.), but **does not store the section name directly**.  Instead, the `sh_name` field is an offset into the **section header string table** (`.shstrtab`). The index of `.shstrtab` is given by `e_shstrndx`. Once this section is located and read, section names can be resolved by interpreting each `sh_name` as an offset into `.shstrtab`. This phase allows us to identify and name all sections, such as `.text`, `.data`, `.bss`, `.symtab`, and prepares the ground for symbol and relocation analysis.

#### B. Reading the section header table in C
The approach here is quite simple as we try to "scoop in" the data containing each section header into a table using `fread`. To do that, we move the stream pointer to `e_shoff` provided by the ELF header, this is done using `fseek`.

#### C. Displaying said table in C

Displaying the table in terminal proves to be more complicated than reading it. As stated in sub-section A, the names of the sections are stored in a string section, which we need to move to using `e_shstrndx`. The subsequent steps involve pulling out the data from the section header table already read, such as the type, address and offset.

One thing to discuss is the FLAGS, in this project and as per documents given, we study 4 flags: 
- `SHF_WRITE` or `WR`
- `SHF_ALLOC` or `AL`
- `SHF_EXECINSTR` or `EX`
- `SHF_ARM_NOREAD` or `NR` (ARM specific)  

These are stored in the 0th, 1st, 2nd and 29th bits respectively in `sh_flags`

The rest of the data consists of `ES` for entry size, `LK` for section header table index link, `IF` for extra information and `AL` for address aligment constraint. The end result should ressemble that of `arm-eabi-gcc-readelf -S`, as it was used as the reference. Below is the example output on the same file used in the ELF header section.

```
Section Header Table:
[Nr] Name          Type                   Addr        Offset      Size        |FLAGS: WR AL EX NR|  ES LK IF AL
[ 0]              SHT_NULL                0x00000000  0x00000000  0x00000000          0  0  0  0    0  0  0  0
[ 1] .text        SHT_PROGBITS            0x00000000  0x00000034  0x00000000          0  1  1  0    0  0  0  1
[ 2] .data        SHT_PROGBITS            0x00000000  0x00000034  0x00000000          1  1  0  0    0  0  0  1
[ 3] .bss         SHT_NOBITS              0x00000000  0x00000034  0x00000000          1  1  0  0    0  0  0  1
[ 4] .text.st[..] SHT_PROGBITS            0x00000000  0x00000034  0x00000010          0  1  1  0    0  0  0  4
[ 5] .rel.tex[..] SHT_REL                 0x00000000  0x00000408  0x00000008          0  0  0  0    8 20  4  4
[ 6] .debug_info  SHT_PROGBITS            0x00000000  0x00000044  0x00000060          0  0  0  0    0  0  0  1
[ 7] .rel.deb[..] SHT_REL                 0x00000000  0x00000410  0x00000050          0  0  0  0    8 20  6  4
[ 8] .debug_a[..] SHT_PROGBITS            0x00000000  0x000000a4  0x0000005b          0  0  0  0    0  0  0  1
[ 9] .debug_a[..] SHT_PROGBITS            0x00000000  0x000000ff  0x00000020          0  0  0  0    0  0  0  1
[10] .rel.deb[..] SHT_REL                 0x00000000  0x00000460  0x00000010          0  0  0  0    8 20  9  4
[11] .debug_r[..] SHT_PROGBITS            0x00000000  0x0000011f  0x00000013          0  0  0  0    0  0  0  1
[12] .rel.deb[..] SHT_REL                 0x00000000  0x00000470  0x00000008          0  0  0  0    8 20 11  4
[13] .debug_line  SHT_PROGBITS            0x00000000  0x00000132  0x00000047          0  0  0  0    0  0  0  1
[14] .rel.deb[..] SHT_REL                 0x00000000  0x00000478  0x00000008          0  0  0  0    8 20 13  4
[15] .debug_str   SHT_PROGBITS            0x00000000  0x00000179  0x000000d8          0  0  0  0    1  0  0  1
[16] .comment     SHT_PROGBITS            0x00000000  0x00000251  0x00000027          0  0  0  0    1  0  0  1
[17] .debug_frame SHT_PROGBITS            0x00000000  0x00000278  0x00000028          0  0  0  0    0  0  0  4
[18] .rel.deb[..] SHT_REL                 0x00000000  0x00000480  0x00000010          0  0  0  0    8 20 17  4
[19] .ARM.att[..] SHT_ARM_ATTRIBUTES      0x00000000  0x000002a0  0x0000002a          0  0  0  0    0  0  0  1
[20] .symtab      SHT_SYMTAB              0x00000000  0x000002cc  0x00000120          0  0  0  0   16 21 16  4
[21] .strtab      SHT_STRTAB              0x00000000  0x000003ec  0x0000001a          0  0  0  0    0  0  0  1
[22] .shstrtab    SHT_STRTAB              0x00000000  0x00000490  0x000000c8          0  0  0  0    0  0  0  1
```

___
### Step 3:
In this step we are reading the raw content of a section. The offset of the section and its size is already in the table of section header read previously, so the task at hand is to simply read the content.

Once again we modeled our output to ressemble that of `arm-none-gcc-readelf`:
```
Dumping section: 22, type SHT_STRTAB              
Offset: 0x00000490

0x00000000     002e7379 6d746162 002e7374 72746162
0x00000010     002e7368 73747274 6162002e 74657874
0x00000020     002e6461 7461002e 62737300 2e72656c
0x00000030     2e746578 742e7374 61727475 70002e72
0x00000040     656c2e64 65627567 5f696e66 6f002e64
0x00000050     65627567 5f616262 72657600 2e72656c
0x00000060     2e646562 75675f61 72616e67 6573002e
0x00000070     72656c2e 64656275 675f726e 676c6973
0x00000080     7473002e 72656c2e 64656275 675f6c69
0x00000090     6e65002e 64656275 675f7374 72002e63
0x000000a0     6f6d6d65 6e74002e 72656c2e 64656275
0x000000b0     675f6672 616d6500 2e41524d 2e617474
0x000000c0     72696275 74657300
```

___
### Step 4 – Reading and Printing the Symbol Table

In this step, we implemented the reading and display of the ELF **symbol table** (`.symtab`), in a way comparable to the `readelf -s` command.



In an ELF file, the symbol table contains information about:
- functions
- variables
- sections
- undefined (external) symbols

Each symbol entry describes:
- the symbol value (address or offset)
- the symbol size
- its type (function, object, section, etc.)
- its binding (local, global, weak)
- the section in which it is defined
- its name

```
typedef struct {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;

```


#### Locating the Symbol Table

An ELF file does not contain a single fixed symbol table.  
Instead, symbol tables are stored as **sections** of type `SHT_SYMTAB`.

The procedure is therefore:
1. Iterate over all section headers
2. Select sections whose type is `SHT_SYMTAB`
3. Treat each such section as a symbol table


#### Reading the Symbol Entries

For a symbol table section:
- The number of symbols is computed as:

```
number_of_symbols = section_size / section_entry_size
```


- All symbol entries (`Elf32_Sym`) are read from the file using the section’s offset.

Because the ELF file used in this project is **big-endian** (ARM EABI) and the host machine is **little-endian**, the multi-byte fields of each symbol entry are converted to the host byte order before being processed.

---

#### Resolving Symbol Names

Symbol names are not stored directly inside the symbol table.

Instead:
- Each symbol table section has a field `sh_link`
- `sh_link` points to the associated string table section (`.strtab`)
- The field `st_name` is an offset into this string table

To retrieve a symbol name:
1. Load the `.strtab` section
2. Use `st_name` as an offset inside the string table


#### Special Case: Section Symbols (`STT_SECTION`)

Symbols of type `STT_SECTION` do not have names in `.strtab`.

For these symbols:
- `st_shndx` gives the index of the corresponding section
- The section name is retrieved from the **section header string table** (`.shstrtab`)
- `.shstrtab` is located using the `e_shstrndx` field of the ELF header

This allows section symbols such as `.text`, `.data`, and `.bss` to be displayed correctly.



At the end of this step, the program:
- Identifies the symbol table section(s)
- Reads and interprets all symbol entries
- Handles endianness correctly
- Resolves symbol names using the appropriate string tables
- Prints a complete symbol table matching the output of `readelf -s`


PS : run the elf_reader with option -s (not -S this is for section headers table) <file.o>

```
Symbol table '.symtab' contains 16 entries:
Num:    Value  Size Type     Bind   Vis      Ndx Name
  0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND 
  1: 00000000     0 SECTION LOCAL  DEFAULT    1 .text
  2: 00000000     0 SECTION LOCAL  DEFAULT    3 .data
  3: 00000000     0 SECTION LOCAL  DEFAULT    5 .bss
  4: 00000000     0 NOTYPE  LOCAL  DEFAULT    1 dead
  5: 00000000     0 NOTYPE  LOCAL  DEFAULT    1 $a
  6: 00000010     0 NOTYPE  LOCAL  DEFAULT    1 end
  7: 00000000     0 SECTION LOCAL  DEFAULT    8 .debug_info
  8: 00000000     0 SECTION LOCAL  DEFAULT   10 .debug_abbrev
  9: 00000000     0 SECTION LOCAL  DEFAULT    6 .debug_line
 10: 00000000     0 SECTION LOCAL  DEFAULT   13 .debug_str
 11: 00000000     0 SECTION LOCAL  DEFAULT   11 .debug_aranges
 12: 00000000     0 SECTION LOCAL  DEFAULT   14 .ARM.attributes
 13: 00000000     0 NOTYPE  GLOBAL DEFAULT  UND main
 14: 00000008     0 NOTYPE  GLOBAL DEFAULT    1 loop
 15: 00000000     0 NOTYPE  GLOBAL DEFAULT  UND other
```
___
### Step 5 — Relocation Sections

In this step, we read and display the **relocation entries** contained in an ELF object file, similarly to the command `readelf -r`.

Relocation sections (`SHT_REL` and `SHT_RELA`) indicate **locations in sections (.text, .data, etc.) that must be fixed by the linker** once symbol addresses are known. At this stage, relocations are **not applied**, only parsed and printed.

#### Implementation

- Iterates over all section headers to find relocation sections (`SHT_REL` and `SHT_RELA`)
- Computes the number of relocation entries using:
```

sh_size / sh_entsize

```
- Retrieves the relocation section name using the section header string table (`.shstrtab`)
- Reads each relocation entry:
- `r_offset` and `r_info`
- `r_addend` for `SHT_RELA`
- Handles endianness correctly
- Decodes ARM relocation types (e.g. `R_ARM_ABS32`, `R_ARM_CALL`, `R_ARM_JUMP24`)
- Prints relocation metadata (offset, info, type)

The implementation also retrieves a symbol table `symtab` and the string table associated (linked) with the `SHT_SYMTAB` section read in the previous step, for the purpose of displaying the symbols' value and names (with the same procedure as the previous step) 

#### Goal of this step

The objective is to **identify where relocations occur and what type they are**, preparing the data required for the final linking and relocation application phases.

___
## Phase 2 — Fusion (Linking)

The second phase of the project consists in **merging multiple ELF relocatable object files (`.o`) into a single ELF relocatable file**.  
At this stage, no code is executed: the goal is to **combine structures produced in Phase 1** in order to prepare the final linking process.

Phase 2 relies entirely on the data extracted previously:
- section header tables
- section contents
- symbol tables
- relocation tables

The fusion process is divided into **three dependent sub-steps**:
1. Section fusion
2. Symbol table fusion
3. Relocation table fusion

---

### Step 6 — Section Fusion

#### Objective
Merge sections of the same name (`.text`, `.data`, `.bss`, etc.) coming from different object files into unified sections. The scope of the project is to fuse the `SHT_PROGBITS` of the two objects files together by simple concatenation. However, we are also fusing `SHT_NOBITS` and `SHT_ARM_ATTRIBUTES` by simple concatenation as well.

The finished result should be an structure of type SectionFusionContext, the definition is as below:

```
typedef struct {
    int src_index;        // original section index (file 2)
    int dst_index;        // section index in output ELF
    uint32_t add_offset;  // offset added due to concatenation 
} SectionMapEntry;

typedef struct {
    SectionMapEntry *map;
    size_t map_count;
    uint32_t *merged_sizes;   // indexed by output section index
    size_t merged_count;
} SectionFusionContext;

```
The map array contains mapping of each section fused from the second input file to the first one, as well as the added offset value.

The side effects include modifying the Section headers table, to take account for new sections added and size change to existing sections due to concatenation. 

#### Inner workings
We scan the second section header table to look for sections of types wanted, then we search the first section header table for sections of the same names, if exist, then we change the according entries in the first section header table, if not, then we the new entries from the second table to the first one.

The real concatenation will be done in the ELF writing step, as it is not useful to have copies of each newly-formed section stored in the memory. We will simply write one section after another for concatenation.

The forming of a new section header string table (`shstrtab`) is based on a `realloc` of the first `shstrtab`: each newly added section adds its name's length to the new `shstrtab` size, we count the total size after the previous process is done, then reallocate the `shstrtab` and then copy the names of the new sections to the end of the newly allocated tab. Changes are made to the `sh_name` in its section header table entry are also made to accomodate newly referenced name index.

#### Main function used:
```
SectionFusionContext *fusion_build_sections(Elf32_Ehdr *hdr1, Elf32_Ehdr *hdr2,Elf32_Shdr **sht1, Elf32_Shdr *sht2, char **shstr1, char *shstr2);
```


Results: 
  - The return value is a pointer of type SectionFusionContext*, the exact details of this structure have been discussed above.
  - ELF header of the first file `hdr1` is modified to take into account the augmented sections number.
  - Section header table of the first file `sht1` is modified like discussed and is now the new section header table of the merged file. *For further disscusion, all changes made to the section header table is taken into account here (i.e new sections, merged sections, updated size, updated `sh_link`..)*
  - Section header string table of the first file `shstr1` is modified and is now the new section header string table of the merged file.


### Step 7 — Symbol Table Fusion

#### Objective
Create a **single global symbol table** from all input object files, copying all the `LOCAL` symbol from the two files, and merge all the `GLOBAL` symbols

#### Method
- We scan the first and second `symtab` for `LOCAL`, we copy all of them into a new `symtab`. 
- We then scan the first table for any `GLOBAL` symbols, and then compare to the second `symtab` and treat them according to rules.
- We then scan the second table of any `GLOBAL` that does not exist in the first one, and add them directly to the new `symtab`.

The the new `symtab` is forming with all the symbols necessary, now we have to change the `Ndx`, which is a section the symbol is linked to. We scan the fusion context done in step 6, and for each `Ndx` of each symbol of the second `symtab`, and map accordingly to the context.

#### Function used 
```
Elf32_Sym *fusion_symbole(Elf32_Sym *symtab1,Elf32_Sym *symtab2,Elf32_Ehdr hdr1, Elf32_Ehdr hdr2, Elf32_Shdr **section_header_table1, Elf32_Shdr *section_header_table2, char* strtab1, char* strtab2,char ** new_strtab, SectionFusionContext *fusion_context);
```
- The return value is a new `symtable` containing all the symbols merged and copied.
- The symbol string table of the first file `strtab1` is extended to accomodate new symbols' names.


---

### Step 8 — Relocation Table Fusion

#### Objective
Rebuild relocation tables so they reference the **correct offset and fused symbol table**.

#### Method 

To simplify the process, `SHT_REL`, `SHT_RELA` has been added as entries to `SectionFusionContext`, thus including mapping sources (file 2) and destinations (file 1).

We divide the task into 2 smaller nearly identical tasks: to treat all the `SHT_REL` and the `SHT_RELA`
```
  typedef struct {
      Elf32_Rel *rel;
      int des_index;
      int size;

  } Rel_table;
```

- We scan the modified section header table (this includes all merged sections and added sections, and is built upon the first section header table) for any `SHT_REL` section, then look for its mapping partner using the fusion context, make change to the partner, and put all the relocation entries of the two into an entry of `Rel_table`. `Reltable` contains an array of relocations, the destination, fused section index (to write the relocations entries later on).
- The same strategy is applied to `SHT_RELA`, adding the modifying of `r_addend`.

#### Function used
Here is the main function for treating `SHT_RELA`:
```
Rela_table* fusion_relocation_rela (FILE *f1, FILE *f2, Elf32_Ehdr *hdr1, Elf32_Word hdr1_shnum_old, Elf32_Ehdr *hdr2, Elf32_Shdr *sht1, Elf32_Shdr *sht2, SectionFusionContext *fusion, Elf32_Sym *fused_symtab,int*sym_map1, int *sym_map2, int* rela_tab_size)
```
- Return value is an array  of `Rela_table` having entries for all `SHT_RELA` table in the new section header table.
- Here, `hdr1_shnum_old` is the section number of the first file, to help draw the line between newly added sections and old sections in the new section header table.


### Step 9 — ELF Reconstruction

#### Objective
Reconstruct an ELF-relocatable file using all the data that has been treated.

#### The ELF-Header
This is by far the simplest step, we reverse-engineered (or rather, *reverse-reverse-engineered*) the ELF-Header reading function, and write the data of an `ELF_Hdr` object into a file.
#### The Section Headers Table and its string table
We discuss these components as they are simpler to implement. The section header table is also written using a reversed reading function, with the use of `fseek` to write at the end of the file. During conception, this was in fact written first to ensure progress.

The string table for sections' name is also written with a simple use of `fwrite` and its offset found in the section header table.

#### The sections:
- We have different type of sections, mainly **relocatation sections**, **symbols table**, **string table** and **other sections**, we iterate through the new section header table to decide what to write.
- The other sections consists of sections merged by simple concatenation and no changes is done to their raw data. Writing fused entries consists of firstly writing the fusing component of the first file, and then write the component of the second file. This applies to almost every section type.
- Relocation sections are pre-calculated in step 8, if we encounter a `SHT_REL` or `SHT_RELA`, we search the repsective `Rel_table` or `Rela_table` for an entry with the same destination index, and then write in the entire array of that index.
- The symbol table is pre-calculated and so is written directly using `fwrite`.
- The string table used a similar method to the sections name table.

#### The end result
We obtain a file, readable by our previous `./elf_reader` program and contains all the symbols, sections, relocations, fused or added.

It is also important to mention that we re-organized the sections order (written order) by their order in the section header table, as an example, consult the table below:

```
Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .text             PROGBITS        00000000 000034 00000c 00  AX  0   0  1
  [ 2] .data             PROGBITS        00000000 000040 000000 00  WA  0   0  1
  [ 3] .bss              NOBITS          00000000 000040 000000 00  WA  0   0  1
  [ 4] .text.startup     PROGBITS        00000000 000040 000010 00  AX  0   0  4
  [ 5] .rel.text.startup REL             00000000 000050 000008 08   I 20   4  4
  [ 6] .debug_info       PROGBITS        00000000 000058 00010c 00      0   0  1
  [ 7] .rel.debug_info   REL             00000000 000164 000110 08   I 20   6  4
  [ 8] .debug_abbrev     PROGBITS        00000000 000274 0000c8 00      0   0  1
  [ 9] .debug_aranges    PROGBITS        00000000 00033c 000040 00      0   0  1
  [10] .rel.debug_a[...] REL             00000000 00037c 000020 08   I 20   9  4
  [11] .debug_ranges     PROGBITS        00000000 00039c 000010 00      0   0  1
  [12] .rel.debug_ranges REL             00000000 0003ac 000010 08   I 20  11  4
  [13] .debug_line       PROGBITS        00000000 0003bc 00008f 00      0   0  1
  [14] .rel.debug_line   REL             00000000 00044b 000010 08   I 20  13  4
  [15] .debug_str        PROGBITS        00000000 00045b 0002a0 01  MS  0   0  1
  [16] .comment          PROGBITS        00000000 0006fb 000068 01  MS  0   0  1
  [17] .debug_frame      PROGBITS        00000000 000763 000048 00      0   0  4
  [18] .rel.debug_frame  REL             00000000 0007ab 000020 08   I 20  17  4
  [19] .ARM.attributes   ARM_ATTRIBUTES  00000000 0007cb 000054 00      0   0  1
  [20] .symtab           SYMTAB          00000000 00081f 000230 10     21  32  4
  [21] .strtab           STRTAB          00000000 000a4f 000039 00      0   0  1
  [22] .shstrtab         STRTAB          00000000 000a88 0000de 00      0   0  1
  [23] .rel.text         REL             00000000 000b66 000010 08   I 20   1  4
  [24] .rodata.str1.4    PROGBITS        00000000 000b76 00000c 01 AMS  0   0  4
```
We can see that the offset are incremental, this facilitates writing.

## End of Phase 2
This section here is being updated as of Thursday, 08/01/2026. 

### testing
Pour lancer un test, on exécute : ./test.sh

####Progression des tests
#####Phase 1 — elf_reader

Comparaison : readelf vs ./elf_reader

PASS/FAIL via diff

#####Phase 2 — elf_fusion

Vérification :

Mappage des sections

Offsets

Remappage des symboles

Comparaison avec : arm-none-eabi-ld -r -Map=map.txt

#####Phase 3 — elf_writer (validation binaire)

Fusion à l’aide de arm-none-eabi-ld -EB -r file1.o file2.o

Écriture du fichier fusionné à partir de ./elf_fusion en utilisant ./elf_writer

readelf -h ourfile.o et file.o

Comparaison des sorties de readelf

#####Phase 1 – Tests du lecteur ELF

Nous avons validé le lecteur ELF en comparant sa sortie avec celle de GNU readelf pour des fichiers objet ARM ELF32 générés à partir de sources assembleur contrôlées. Chaque test cible une fonctionnalité spécifique de l’ELF (vide, texte, sections, symboles, relocations). Les tests sont entièrement automatisés via des scripts shell et utilisent une comparaison basée sur diff pour garantir une équivalence stricte.

Exemple d’un test réussi :

=== Testing empty ===
✔ empty OK
=== Testing text_only ===
✔ text_only OK
=== Testing data_text ===
✔ data_text OK
=== Testing symbols ===
✔ symbols OK
=== Testing reloc_text ===
✔ reloc_text OK


Tous les tests du lecteur sont PASSÉS.
Si un test échoue → diff arrête immédiatement le script.

Nous avons détecté un bug lors des tests où le nombre de sections affiché était incorrect.
Cela provenait d’une mauvaise gestion de l’endianess du champ e_shnum, ce qui provoquait une lecture hors limites de la table des sections.

#####Phase 2 – Tests du fusionneur ELF

Ce script permet de valider la logique de fusion indépendamment de l’écriture binaire. Nous comparons la structure produite par notre outil avec celle du linker officiel afin de garantir la cohérence des calculs avant génération du fichier ELF final.


#####Phase 3 – Tests de l’écriture ELF

Ce test valide l’écriture finale du fichier ELF en comparant sa structure complète avec celle produite par le linker ARM officiel, à l’aide de readelf. Toute divergence est analysée structurellement et non textuellement.


### Known bugs
We are now discussing of a bug, explained simply by comparing the two output:
```
$ arm-none-eabi-readelf -s file3.o 

Symbol table '.symtab' contains 35 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 00000000     0 FILE    LOCAL  DEFAULT  ABS file1.c
     2: 00000000     0 SECTION LOCAL  DEFAULT    1 .text
     3: 00000000     0 SECTION LOCAL  DEFAULT    2 .data
     4: 00000000     0 SECTION LOCAL  DEFAULT    3 .bss
     5: 00000000     0 SECTION LOCAL  DEFAULT    4 .text.startup
     6: 00000000     0 NOTYPE  LOCAL  DEFAULT    4 $a
     7: 00000000     0 SECTION LOCAL  DEFAULT    6 .debug_info
     8: 00000000     0 SECTION LOCAL  DEFAULT    8 .debug_abbrev
     9: 00000000     0 SECTION LOCAL  DEFAULT    9 .debug_aranges
    10: 00000000     0 SECTION LOCAL  DEFAULT   11 .debug_ranges
    11: 00000000     0 SECTION LOCAL  DEFAULT   13 .debug_line
    12: 00000000     0 SECTION LOCAL  DEFAULT   15 .debug_str
    13: 00000000     0 SECTION LOCAL  DEFAULT   17 .debug_frame
    14: 00000000     0 SECTION LOCAL  DEFAULT   16 .comment
    15: 00000000     0 SECTION LOCAL  DEFAULT   19 .ARM.attributes
    16: 00000000     0 FILE    LOCAL  DEFAULT  ABS file2.c
    17: 00000000     0 SECTION LOCAL  DEFAULT    1 $d
    18: 00000000     0 SECTION LOCAL  DEFAULT    2 $d
    19: 00000000     0 SECTION LOCAL  DEFAULT    3 $d
    20: 00000000     0 SECTION LOCAL  DEFAULT   24 $d
    21: 00000000     0 NOTYPE  LOCAL  DEFAULT   24 $d
    22: 00000000     0 NOTYPE  LOCAL  DEFAULT    1 $a
    23: 00000008     0 NOTYPE  LOCAL  DEFAULT    1 $d
    24: 0000005d     0 SECTION LOCAL  DEFAULT    6 afficher
    25: 00000059     0 SECTION LOCAL  DEFAULT    8 afficher
    26: 00000020     0 SECTION LOCAL  DEFAULT    9 afficher
    27: 00000047     0 SECTION LOCAL  DEFAULT   13 afficher
    28: 000000fe     0 SECTION LOCAL  DEFAULT   15 afficher
    29: 00000028     0 SECTION LOCAL  DEFAULT   17 afficher
    30: 00000034     0 SECTION LOCAL  DEFAULT   16 afficher
    31: 0000002a     0 SECTION LOCAL  DEFAULT   19 afficher
    32: 00000000    16 FUNC    GLOBAL DEFAULT    4 main
    33: 00000000    12 FUNC    GLOBAL DEFAULT    1 afficher
    34: 00000000     0 NOTYPE  GLOBAL DEFAULT  UND puts
```
```
$ ../elf_reader/elf_reader -s file3.o 
symtab malloc size: 560

Symbol table '.symtab' contains 35 entries:
Num:    Value  Size Type     Bind   Vis      Ndx Name
  0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND 
  1: 00000000     0 FILE    LOCAL  DEFAULT  ABS file1.c
  2: 00000000     0 SECTION LOCAL  DEFAULT    1 .text (27)
  3: 00000000     0 SECTION LOCAL  DEFAULT    2 .data (33)
  4: 00000000     0 SECTION LOCAL  DEFAULT    3 .bss (39)
  5: 00000000     0 SECTION LOCAL  DEFAULT    4 .text.startup (48)
  6: 00000000     0 NOTYPE  LOCAL  DEFAULT    4 $a
  7: 00000000     0 SECTION LOCAL  DEFAULT    6 .debug_info (66)
  8: 00000000     0 SECTION LOCAL  DEFAULT    8 .debug_abbrev (78)
  9: 00000000     0 SECTION LOCAL  DEFAULT    9 .debug_aranges (96)
 10: 00000000     0 SECTION LOCAL  DEFAULT   11 .debug_ranges (115)
 11: 00000000     0 SECTION LOCAL  DEFAULT   13 .debug_line (133)
 12: 00000000     0 SECTION LOCAL  DEFAULT   15 .debug_str (145)
 13: 00000000     0 SECTION LOCAL  DEFAULT   17 .debug_frame (169)
 14: 00000000     0 SECTION LOCAL  DEFAULT   16 .comment (156)
 15: 00000000     0 SECTION LOCAL  DEFAULT   19 .ARM.attributes (182)
 16: 00000000     0 FILE    LOCAL  DEFAULT  ABS file2.c
 17: 00000000     0 SECTION LOCAL  DEFAULT    1 .text (27)
 18: 00000000     0 SECTION LOCAL  DEFAULT    2 .data (33)
 19: 00000000     0 SECTION LOCAL  DEFAULT    3 .bss (39)
 20: 00000000     0 SECTION LOCAL  DEFAULT   24 .rodata.str1.4 (208)
 21: 00000000     0 NOTYPE  LOCAL  DEFAULT   24 $d
 22: 00000000     0 NOTYPE  LOCAL  DEFAULT    1 $a
 23: 00000008     0 NOTYPE  LOCAL  DEFAULT    1 $d
 24: 0000005d     0 SECTION LOCAL  DEFAULT    6 .debug_info (66)
 25: 00000059     0 SECTION LOCAL  DEFAULT    8 .debug_abbrev (78)
 26: 00000020     0 SECTION LOCAL  DEFAULT    9 .debug_aranges (96)
 27: 00000047     0 SECTION LOCAL  DEFAULT   13 .debug_line (133)
 28: 000000fe     0 SECTION LOCAL  DEFAULT   15 .debug_str (145)
 29: 00000028     0 SECTION LOCAL  DEFAULT   17 .debug_frame (169)
 30: 00000034     0 SECTION LOCAL  DEFAULT   16 .comment (156)
 31: 0000002a     0 SECTION LOCAL  DEFAULT   19 .ARM.attributes (182)
 32: 00000000    16 FUNC    GLOBAL DEFAULT    4 main
 33: 00000000    12 FUNC    GLOBAL DEFAULT    1 afficher
 34: 00000000     0 NOTYPE  GLOBAL DEFAULT  UND puts
```
The `file3.o` shown here is the result of fusing `file1.o` and `file2.o` given for the project using our program. In this particular case, we are not discussing the similarity or the lackthereof with the already fused file `filefused.o` given, we can see that the difference is in the names of some symbols, for example `.rodata.str1.4`, which is correctly read by our program and not by `readelf`. As `readelf` is the standard reference for this project, we are now investigating the writing functions.

___

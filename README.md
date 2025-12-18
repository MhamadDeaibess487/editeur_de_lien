# Prog5Projet




## Progress tracking:
- **15/12/2025**: Partially completed ELF header reader and section table reader
- **16/12/2025**: ELF header reader is complete
- **17/12/2025**: Section header table printing is complete
- **18/12/2025**: Added fully functional section dumper

### Current program:
We use a mix of header files for definitions and .c files for functions (like any other sane human beings), the main program for phase 1 is located in `elf_reader.c` where arguments are taken. Below are brief descriptions of each files:
#### Header files:
-  `elf.h`: definitions for the ELF format, including but not limited to: `Elf32_Ehdr`
-  `units_elf32.h`: definitions of the types, for example `Elf32_Word`
-  `util.h`: given file, containing a function to determine if a file is big endian
#### .c files:
- `elf_reader.c` : main program for reading ELF
- `header_reader.c`: functions to read and print the ELF header
- `header_section_reader`: functions to read and print the Section headers table
- `section_reader.c`: functions to dump a section
- `util.c`: given file

## Phase 1

### Usage for elf_reader
As of right now, there is only one executable to be made, compile with 
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

### Abstracts
There are three main types of object files  
- A relocatable file  
- An executable file  
- A shared object file  

An object file can be seen from two sides: linking view and execution view (figure 1-1).

![alt text](image-1.png)  

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


#include "breakpoint.h"
#include "dbg.h"
#include "elf_parser.h"
#include "proc.h"


void f_usage(char *argv) { printf(WHITE "\tUsage: %s args\n", argv); }

int f_check(char *fileName) {
    if (!access(fileName, F_OK)) {
        return 0;
    } else {
        printf("\t%s not Found\n", fileName);
        return -1;
    }
    if (access(fileName, X_OK)) {
        printf("\t%s is not an Executable\n", fileName);
        return -1;
    }
}

int f_check_elf(int32_t fd, Elf64_Ehdr *header) {
    if (read(fd, header->e_ident, sizeof(Elf64_Ehdr)) == -1) {
        return -1;
    }
    if (header->e_ident[0] == ELFMAG0 && header->e_ident[1] == ELFMAG1 &&
        header->e_ident[2] == ELFMAG2 && header->e_ident[3] == ELFMAG3) {
        if (header->e_ident[4] == ELFCLASS64) {
            return 0;
        }
    }
    return -1;
}

void info_header(Elf64_Ehdr header) {
    printf(WHITE "ELF header:\n");
    printf(" Magic number:\x1B[0m\t\t");
    for (int i = 0; i < 16; i++) {
        printf(" %02x", header.e_ident[i]);
    }
    printf("\n");
    if (header.e_ident[EI_CLASS] == ELFCLASS64)
        printf(WHITE " Class:\x1B[0m \t\t x64 - ELF64\n");
    switch (header.e_ident[EI_DATA]) {
        case ELFDATANONE:
            printf(WHITE " Data:\x1B[0m \t\t\t Invalid data encoding\n");
            break;
        case ELFDATA2LSB:
            printf(WHITE
                   " Data:\x1B[0m \t\t\t 2’s complement values, least "
                   "significant byte\n");
            break;
        case ELFDATA2MSB:
            printf(WHITE
                   " Data:\x1B[0m \t\t\t  2’s complement values, most "
                   "significant byte\n");
            break;
        default:
            return;
    }
    switch (header.e_ident[EI_VERSION]) {
        case EV_NONE:
            printf(WHITE " Version:\x1B[0m \t\t 0, Invalid version\n");
            break;
        case EV_CURRENT:
            printf(WHITE " Version:\x1B[0m \t\t 1, Current version\n");
            break;
        default:
            return;
    }
    /* 	switch(header.e_ident[EI_OSABI]){
                    case ELFOSABI_GNU:
                            printf(" \x1B[37;1mOS:\x1B[0m \t GNU/Linux\n");
                            break;
                    case ELFOSABI_FREEBSD:
                            printf(" \x1B[37;1mOS:\x1B[0m \t FreeBSD\n");
                            break;
                    default:
                            break;
    } */
    printf(WHITE " Entry point:\x1B[0m \t\t 0x%lx\n", header.e_entry);
    switch (header.e_machine) {
        case EM_NONE:
            printf(WHITE " Machine:\x1B[0m \t An unknown machine");
            break;
        case EM_M32:
            printf(WHITE " Machine:\x1B[0m \t AT&T WE 32100");
            break;
        case EM_386:
            printf(WHITE " Machine:\x1B[0m \t Intel 80386");
            break;
        case EM_68K:
            printf(WHITE " Machine:\x1B[0m \t Motorola 68000");
            break;
        case EM_88K:
            printf(WHITE " Machine:\x1B[0m \t Motorola 88000");
            break;
        case EM_860:
            printf(WHITE " Machine:\x1B[0m \t Intel 80860");
            break;
        case EM_MIPS:
            printf(WHITE " Machine:\x1B[0m \t MIPS RS3000 (big-endian only)");
            break;
        case EM_PARISC:
            printf(WHITE " Machine:\x1B[0m \t HPPA");
            break;
        case EM_PPC:
            printf(WHITE " Machine:\x1B[0m \t PowerPC");
            break;
        default:
            break;
    }
    printf(WHITE " Flags:\x1B[0m \t\t 0x%x\n", header.e_flags);
    printf(WHITE " Start of program headers:\x1B[0m 0x%lx (bytes into file)\n",
           header.e_phoff);
    printf(WHITE " Start of section headers:\x1B[0m 0x%lx (bytes into file)\n",
           header.e_shoff);
    printf(WHITE " Header size:\x1B[0m \t\t 0x%x\n", header.e_ehsize);
}

void print_section(Elf64_Ehdr header, Elf64_Shdr *shdr, int fd) {
    struct stat sb;
    fstat(fd, &sb);
    char *offset;
    offset = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    shdr = (Elf64_Shdr *)(offset + header.e_shoff);
    char *sec_name = (char *)(offset + shdr[header.e_shstrndx].sh_offset);
    for (int i = 1; i < header.e_shnum; i++) {
        printf("%s\n", (sec_name + shdr[i].sh_name));
    }
}

void print_symbols(Elf64_Ehdr header, Elf64_Shdr *shdr, int fd) {
    (void)header;
    (void)shdr;
    (void)fd;
}
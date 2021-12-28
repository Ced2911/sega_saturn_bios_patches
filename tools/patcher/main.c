#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <elf.h>

//what kind of elfs are we dealinth with yo?
//yay polymorphism via macros!
//#define ELFN(x) Elf64_ ## x

char usage[] =
    "usage: %s sourceElf binFile result\n\
\n\
Note: binFile must be in flat binary format.\n\
";

void DumpHex(const void *data, size_t size)
{
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i)
    {
        printf("%02X ", ((unsigned char *)data)[i]);
        if (((unsigned char *)data)[i] >= ' ' && ((unsigned char *)data)[i] <= '~')
        {
            ascii[i % 16] = ((unsigned char *)data)[i];
        }
        else
        {
            ascii[i % 16] = '.';
        }
        if ((i + 1) % 8 == 0 || i + 1 == size)
        {
            printf(" ");
            if ((i + 1) % 16 == 0)
            {
                printf("|  %s \n", ascii);
            }
            else if (i + 1 == size)
            {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8)
                {
                    printf(" ");
                }
                for (j = (i + 1) % 16; j < 16; ++j)
                {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}

void copy_section(uint8_t *dst, uint32_t dst_offset, uint8_t *src, uint32_t src_offset, uint32_t src_size)
{
    memcpy(dst + dst_offset, src + src_offset, src_size);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf(usage, argv[0]);
        return 1;
    }

    void *sourceElf;
    off_t sourceElfSize;
    void *infected;
    off_t infectedSize;
    void *binFile;
    off_t binFileSize;
    int i, j;
    Elf32_Phdr *programHeader;
    Elf32_Shdr *sectionHeader;
    uint32_t start_offset = 0;

    if (readFile(argv[1], &sourceElf, &sourceElfSize) != 0)
    {
        fprintf(stderr, "Could not open file '%s'\n", argv[1]);
        return -1;
    }
    if (readFile(argv[2], &binFile, &binFileSize) != 0)
    {
        fprintf(stderr, "Could not open file '%s'\n", argv[2]);
        return -1;
    }

    start_offset = sizeof(Elf32_Ehdr);

    //set up the headers and tables for the sourceElf file
    Elf32_Ehdr *sourceElfElfHeader = sourceElf;
    Elf32_Phdr *sourceElfProgramHeaderTable = sourceElf + __builtin_bswap32(sourceElfElfHeader->e_phoff);
    Elf32_Shdr *sourceElfSectionHeaderTable = sourceElf + __builtin_bswap32(sourceElfElfHeader->e_shoff);

    //find the string table
    Elf32_Shdr *sourceElfStringTable = sourceElfSectionHeaderTable + __builtin_bswap16(sourceElfElfHeader->e_shstrndx);
    char *sourceElfStringTableValues = sourceElf + __builtin_bswap32(sourceElfStringTable->sh_offset);

    // find all patches addr
    for (i = 0, sectionHeader = sourceElfSectionHeaderTable;
         i < __builtin_bswap16(sourceElfElfHeader->e_shnum);
         i++, sectionHeader++)
    {
        uint16_t sh_name = __builtin_bswap32(sectionHeader->sh_name);
        char *section_name = sourceElfStringTableValues + sh_name;

        // use all sections like .patch_xxx_s
        if (strncmp(".patch", section_name, 6) == 0 /*&& (strstr(section_name, "_s") - section_name) == 7*/)
        {
            uint32_t section_offset = __builtin_bswap32(sectionHeader->sh_offset);
            uint32_t section_vaddr = __builtin_bswap32(sectionHeader->sh_addr);
            uint32_t section_size = __builtin_bswap32(sectionHeader->sh_size);
            uint32_t section_addralign = __builtin_bswap32(sectionHeader->sh_addralign);

            //printf("Apply %s (0x%x - 0x%x) => 0x%x\n", section_name, section_offset, section_size, section_vaddr);
            printf("Apply %s @ 0x%x\n", section_name, section_vaddr);
            DumpHex(sourceElf + section_offset, section_size);

            copy_section(binFile, section_vaddr, (uint8_t *)sourceElf, section_offset, section_size);
        }
    }

    // Save patched file
    writeFile(argv[3], binFile, binFileSize);
    printf("file saved to %s\n", argv[3]);

    return 0;
}

//assigns buffer and size to the size and location on the heap of the files contents
//returns 0 on success, -1 on failure
int readFile(char *path, void **buffer, off_t *size)
{
    //get the file size
    struct stat fileInfo;
    if (stat(path, &fileInfo) != 0)
    {
        return -1;
    }
    //allocate some space on the heap
    *size = fileInfo.st_size;
    if ((*buffer = malloc(fileInfo.st_size)) == 0)
    {
        return -1;
    }
    //read in the file
    FILE *fp;
    if ((fp = fopen(path, "r")) == 0)
    {
        free(*buffer);
        return -1;
    }
    fread(*buffer, 1, *size, fp);
    fclose(fp);

    return 0;
}

int writeFile(char *path, void *buffer, off_t size)
{
    FILE *fp;
    if ((fp = fopen(path, "wb")) == 0)
    {
        return -1;
    }
    fwrite(buffer, 1, size, fp);
    fclose(fp);
    return 0;
}

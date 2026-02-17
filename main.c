#include <stdio.h>
#include <string.h>
#include <sys/mman.h> 
#include "fs_utils.h"

#define FS_SIZE (1024 * 1024) 

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Uso: %s <nome_file_fs>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    printf("Mappando il file %s di dimensione %d byte...\n", filename, FS_SIZE);

    char* fs_memory = (char*) map_fs(filename, FS_SIZE);

    // test per vedere se funziona

    printf("Indirizzo memoria mappata: %p\n", (void*)fs_memory);
    
    if (fs_memory[0] == 0) {
        printf("File vuoto, Scrivo 'prova, ciao, funziona tutto'...\n");
        sprintf(fs_memory, "prova, ciao, funziona tutto");
    } else {
        printf("Trovati dati esistenti: %s\n", fs_memory);
    }

    munmap(fs_memory, FS_SIZE);

    return 0;
}
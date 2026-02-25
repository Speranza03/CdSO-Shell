#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "fs_utils.h"
#include "fs_core.h"

#define FS_SIZE (1024 * 1024)

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Uso: %s <nome_file_fs> <comando>\n", argv[0]);
        printf("Comandi disponibili: format\n");
        return 1;
    }

    const char* filename = argv[1];
    const char* cmd = argv[2];

    char* fs_memory = (char*) map_fs(filename, FS_SIZE);
    
    //test per vedere se funziona 

    if (strcmp(cmd, "format") == 0) {
        fs_format(fs_memory, FS_SIZE);
    }
    else if (strcmp(cmd, "mkdir") == 0) {
        if (argc < 4) {
            printf("Uso: mkdir <nome>\n");
        } else {
            fs_mkdir(fs_memory, 0, argv[3]);
        }
    }
    else if (strcmp(cmd, "ls") == 0) {
        fs_ls(fs_memory, 0);
    }
    else {
        printf("Comando sconosciuto: %s\n", cmd);
    }

    munmap(fs_memory, FS_SIZE);
    return 0;
}
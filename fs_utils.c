#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>  
#include <sys/mman.h> // su WSL2 qui mi da errore, provo dalla vm dell'università
/* 
l'errore è:
#include errors detected. Please update your includePath. 
Squiggles are disabled for this translation unit (D:\Complementi_di_Sistemi_Operativi\CdSO-Shell\fs_utils.c). 

trova il modo di risolverlo che senza mmap non puoi fare nulla

l'errore era usare vscode da windows e non da wsl, risolto installando l'estensione Remote-WSL di vscode, menomale
*/
#include "fs_utils.h"



void* map_fs(const char* filename, size_t size) {

    int fd = open(filename, O_RDWR | O_CREAT, 0666);

    if (fd == -1) {
        perror("Errore apertura file");
        exit(1);
    }


    if (ftruncate(fd, size) == -1) {
        perror("Errore ftruncate");
        close(fd);
        exit(1);
    }

    void* map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    if (map == MAP_FAILED) {
        perror("Errore mmap");
        close(fd);
        exit(1);
    }

    close(fd);

    return map;
}
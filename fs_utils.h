#ifndef FS_UTILS_H
#define FS_UTILS_H

#include <stddef.h>

// Funzione per mappare il file system
// Ritorna il puntatore all'inizio della memoria mappata
void* map_fs(const char* filename, size_t size);

#endif
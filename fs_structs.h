#ifndef FS_STRUCTS_H
#define FS_STRUCTS_H

#include <stdint.h>

#define BLOCK_SIZE 4096       //ogni blocco è di 4KB
#define MAX_FILES 128         //massimo numero di file/cartelle
#define MAX_FILENAME 32       //lunghezza massima nome file
#define MAGIC_NUMBER 0xF0F0   //per riconoscere se il file è formattato

typedef enum { //per distinguere tra directory e file
    FILE_TYPE = 0,
    DIR_TYPE = 1
} FileType;

typedef struct {
    uint8_t is_used;    //0=libero; 1=occupato;
    uint8_t type;
    char name[MAX_FILENAME];
    uint32_t size;
    int blocks[10];//puntatori ai blocchi dati,ho messo 10 così sono 40KB         
} Inode; //rappresenta un file o una cartella

typedef struct {
    uint16_t magic;     //MAGIC_NUMBER
    uint32_t num_blocks;    //totale blocchi dati
    uint32_t num_inodes;
    uint32_t free_blocks;
    //qua i vari offset
    uint64_t inode_map_offset;
    uint64_t block_map_offset;
    uint64_t inode_table_offset;
    uint64_t data_blocks_offset;
} Superblock; //sta all'inizio del file e descrive tutto

typedef struct {
    char name[MAX_FILENAME];
    int inode_index; // -1 = slot vuoto
} DirectoryEntry; //rappresenta una voce dentro una cartella

#endif
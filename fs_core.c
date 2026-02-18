#include "fs_core.h"
#include "fs_structs.h"
#include <string.h>
#include <stdio.h>

void fs_format(char* memory_ptr, size_t size) {
   //ordine: [Superblock] [InodeMap] [BlockMap] [InodeTable] [DataBlocks]

    Superblock* sb = (Superblock*) memory_ptr;

    sb->magic = MAGIC_NUMBER;
    sb->num_inodes = MAX_FILES;
    
    //calcolo degli offset

    size_t sb_size = sizeof(Superblock);
    size_t inode_map_size = MAX_FILES * sizeof(uint8_t);
    size_t headers_size = sb_size + inode_map_size + (MAX_FILES * sizeof(Inode));
    size_t available_data_space = size - headers_size;
    uint32_t num_blocks = available_data_space / (BLOCK_SIZE + 1);
    
    sb->num_blocks = num_blocks;
    sb->free_blocks = num_blocks; //perchè da appena formattata i blocchi sono tutti liberi

    //assegniazione degli offset
    sb->inode_map_offset = sb_size;
    sb->block_map_offset = sb->inode_map_offset + inode_map_size;
    sb->inode_table_offset = sb->block_map_offset + (num_blocks * sizeof(uint8_t));
    sb->data_blocks_offset = sb->inode_table_offset + (MAX_FILES * sizeof(Inode));

    //azzero tutto 
    uint8_t* inode_map = (uint8_t*) (memory_ptr + sb->inode_map_offset);
    memset(inode_map, 0, MAX_FILES);

    uint8_t* block_map = (uint8_t*) (memory_ptr + sb->block_map_offset);
    memset(block_map, 0, num_blocks);

    Inode* inode_table = (Inode*) (memory_ptr + sb->inode_table_offset);
    memset(inode_table, 0, MAX_FILES * sizeof(Inode));

    inode_map[0] = 1; //l'inode 0 è per la root
    inode_table[0].is_used = 1;
    inode_table[0].type = DIR_TYPE;
    strcpy(inode_table[0].name, "/");
    inode_table[0].size = 0;
    for(int i=0; i<10; i++) inode_table[0].blocks[i] = -1; //tutti i puntatori a blocchi sono liberi

    printf("FS Formattato!\n");
    printf("Inode Table Offset: %lu\n", sb->inode_table_offset);
    printf("Data Blocks Offset: %lu\n", sb->data_blocks_offset);
    printf("Blocchi disponibili: %d\n", sb->num_blocks);
}

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


int get_free_inode(char* memory_ptr){ //cerca un bit a 0 nelal Inode Map
    Superblock* sb = (Superblock*) memory_ptr;
    uint8_t* inode_map = (uint8_t*) (memory_ptr + sb->inode_map_offset);

    for (int i = 0; i < sb->num_inodes; i++){
        if (inode_map[i] == 0){
            inode_map[i] = 1;
            return i;
        }

    }
    return -1;
}

int get_free_block(char* memory_ptr){//cerca un bit a 0 nella Block Map
    Superblock* sb = (Superblock*) memory_ptr;
    uint8_t* block_map = (uint8_t*) (memory_ptr + sb->block_map_offset);

    for (int i = 0; i< sb->num_blocks; i++){
        if(block_map[i] == 0){
            block_map[i] = 1;
            sb->free_blocks--;
            return i;
        }
    }
    return -1;
}

void fs_mkdir(char* memory_ptr, int parent_inode_index, char* name){
    Superblock* sb= (Superblock*) memory_ptr;
    Inode* inode_table = (Inode*) (memory_ptr + sb->inode_table_offset);

    int new_inode_idx = get_free_inode(memory_ptr);
    if (new_inode_idx == -1){
        printf("Errore: Nessun Inode Libero.\n");
        return;
    }
    
    Inode* new_inode = &inode_table[new_inode_idx];
    new_inode->is_used = 1;
    new_inode->type = DIR_TYPE;
    strncpy(new_inode->name, name, MAX_FILENAME);
    new_inode->size  =0;

    for(int i = 0; i < 10; i++){
        new_inode->blocks[i] = -1;
    }

    Inode* parent_inode = &inode_table[parent_inode_index];

    if (parent_inode->blocks[0] == -1){
        int new_block = get_free_block(memory_ptr);
        if(new_block == -1){
            printf("Errore: Disco pieno.\n");
            return;
        }
        parent_inode->blocks[0] = new_block;

        char* block_ptr = memory_ptr + sb->data_blocks_offset +(new_block * BLOCK_SIZE);
        memset(block_ptr, 0, BLOCK_SIZE);

        DirectoryEntry* entries = (DirectoryEntry*) block_ptr;
        int max_entries = BLOCK_SIZE / sizeof(DirectoryEntry);
        for(int i = 0; i < max_entries; i++){
            entries[i].inode_index = -1;
        }
    }

    int parent_block_idx = parent_inode->blocks[0];
    char* block_ptr = memory_ptr + sb->data_blocks_offset + (parent_block_idx * BLOCK_SIZE);
    DirectoryEntry* entries = (DirectoryEntry*) block_ptr;

    int max_entries = BLOCK_SIZE / sizeof(DirectoryEntry);
    for (int i = 0; i < max_entries; i++){
        if (entries[i].inode_index == -1){
            entries[i].inode_index = new_inode_idx;
            strncpy(entries[i].name, name, MAX_FILENAME);
            printf("Creata directory '%s' (Inode %d) \n", name, new_inode_idx);
            return;
        }
    }
    printf("Errore: Directory padre piena \n");
}

void fs_ls (char* memory_ptr, int dir_inode_index){
    Superblock* sb= (Superblock*) memory_ptr;
    Inode* inode_table = (Inode*) (memory_ptr + sb->inode_table_offset);
    Inode* dir_inode = &inode_table[dir_inode_index];

    if (dir_inode->type != DIR_TYPE){
        printf("Errore: non è una directory.\n");
        return;
    }

    printf("Contenuto di %s:\n", dir_inode->name);

    if (dir_inode->blocks[0] == -1){
        printf("(vuoto)\n");
        return;
    }

    int block_idx = dir_inode->blocks[0];
    char* block_ptr = memory_ptr + sb->data_blocks_offset + (block_idx * BLOCK_SIZE);
    DirectoryEntry* entries = (DirectoryEntry*) block_ptr;

    int max_entries = BLOCK_SIZE / sizeof(DirectoryEntry);
    for (int i = 0; i < max_entries; i++){
        if (entries[i].inode_index != -1){
            Inode* target = &inode_table[entries[i].inode_index];
            printf("%s\t[%s] (Inode %d)\n", entries[i].name, (target->type == DIR_TYPE) ? "DIR" : "FILE", entries[i].inode_index);
        }
    }
}
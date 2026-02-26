#ifndef FS_CORE_H
#define FS_CORE_H

#include <stddef.h>

//fnunzione format
void fs_format(char* memory_ptr, size_t size);
//funzione per cercare Inode libero
int get_free_inode(char* memory_ptr);
//funzione per cercare Blocco libero
int get_free_block(char* memory_ptr);
//funzione ls
void fs_ls(char* memory_ptr, int dir_inode_index);
//funziojne mkdir
void fs_mkdir(char* memory_ptr, int parent_inode_index, char* name);
//funzione per cercare un file per nome
int get_inode_in_dir(char* memory_ptr, int parent_inode_index, char* name);
//funzione touch
void fs_touch(char* memory_ptr, int parent_inode_index, char* name);
//funzione append
void fs_append(char* memory_ptr, int parent_inode_index, char* filename, char* text);
//funzione cat
void fs_cat(char* memory_ptr, int parent_inode_index, char* filename);
//funzione rm
void fs_rm(char* memory_ptr, int parent_inode_index, char* name);

/*
mancano:
    cd
    close
    */
#endif
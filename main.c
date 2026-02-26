#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "fs_utils.h"
#include "fs_core.h"
#include "fs_structs.h"

#define FS_SIZE (1024 * 1024)
#define MAX_CMD_LEN 256


int current_inode_index = 0;

void print_prompt() {
    printf("shell_fs:%d> ", current_inode_index);
    fflush(stdout);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Uso: %s <nome_file_fs>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    char* fs_memory = (char*) map_fs(filename, FS_SIZE);
    
    Superblock* sb = (Superblock*) fs_memory;
    Inode* inode_table = (Inode*) (fs_memory + sb->inode_table_offset);

    char input[MAX_CMD_LEN];
    
    printf("Benvenuto nella Shell FS. Digita 'help' per i comandi.\n");

    while(1) {
        print_prompt();
        if (fgets(input, MAX_CMD_LEN, stdin) == NULL) break; 
        
        input[strcspn(input, "\n")] = 0;
        
        char* cmd = strtok(input, " ");
        if (!cmd) continue;

        char* arg1 = strtok(NULL, " ");
        char* arg2 = NULL; 
        
        if (strcmp(cmd, "append") == 0) {
            arg2 = strtok(NULL, "\n"); 
        } else {
            arg2 = strtok(NULL, " "); 
        }

        if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "close") == 0) {
            break;
        }
        else if (strcmp(cmd, "format") == 0) {
            fs_format(fs_memory, FS_SIZE);
            current_inode_index = 0; 
        }
        else if (strcmp(cmd, "mkdir") == 0) {
            if (!arg1) printf("Uso: mkdir <nome>\n");
            else fs_mkdir(fs_memory, current_inode_index, arg1);
        }
        else if (strcmp(cmd, "ls") == 0) {
            fs_ls(fs_memory, current_inode_index);
        }
        else if (strcmp(cmd, "touch") == 0) {
            if (!arg1) printf("Uso: touch <nome>\n");
            else fs_touch(fs_memory, current_inode_index, arg1);
        }
        else if (strcmp(cmd, "cat") == 0) {
            if (!arg1) printf("Uso: cat <nome>\n");
            else fs_cat(fs_memory, current_inode_index, arg1);
        }
        else if (strcmp(cmd, "append") == 0) {
            if (!arg1 || !arg2) printf("Uso: append <nome> <testo>\n");
            else fs_append(fs_memory, current_inode_index, arg1, arg2);
        }
        else if (strcmp(cmd, "rm") == 0) {
            if (!arg1) printf("Uso: rm <nome>\n");
            else fs_rm(fs_memory, current_inode_index, arg1);
        }
        else if (strcmp(cmd, "cd") == 0) {
            if (!arg1) {
                current_inode_index = 0;
            } else {
                int next_inode = get_inode_in_dir(fs_memory, current_inode_index, arg1);
                
                if (next_inode == -1) {
                    printf("Directory '%s' non trovata.\n", arg1);
                } else {
                    Superblock* curr_sb = (Superblock*) fs_memory;
                    Inode* curr_table = (Inode*) (fs_memory + curr_sb->inode_table_offset);

                    if (curr_table[next_inode].type != DIR_TYPE) {
                        printf("Errore: '%s' non è una directory (è un file).\n", arg1);
                    } else {
                        current_inode_index = next_inode;
                    }
                }
            }
        }
        else if (strcmp(cmd, "help") == 0) {
            printf("Comandi: format, mkdir, ls, cd, touch, cat, append, rm, close\n");
        }
        else {
            printf("Comando sconosciuto.\n");
        }
    }

    munmap(fs_memory, FS_SIZE);
    printf("Chiusura shell.\n");
    return 0;
}
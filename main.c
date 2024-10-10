#include "vma.h"

#define MAX_STRING_SIZE 512

int main(void)
{   
    arena_t *arena = malloc(sizeof(arena_t));
    DIE(arena == NULL, "Malloc failed");

    char command[MAX_STRING_SIZE];

    while(1) {
        fgets(command, MAX_STRING_SIZE, stdin);
        command[strlen(command) - 1] = '\0';

        if(strncmp(command, "ALLOC_ARENA", 11) == 0) {
            if (check_valid_2words_command(command, &arena->arena_size) != 0)
                arena = alloc_arena(arena->arena_size);
        } else if(strcmp(command, "DEALLOC_ARENA") == 0) {
            dealloc_arena(arena);
            break;
        } else if (strncmp(command, "ALLOC_BLOCK", 11) == 0) {
            uint64_t address, size;
            if (check_valid_3words_command(command, &address, &size) != 0) {
                alloc_block(arena, address, size);
            }
        } else if (strncmp(command, "FREE_BLOCK", 10) == 0) {
            uint64_t address;
            if (check_valid_2words_command(command, &address) != 0)
                free_block(arena, address);
        } else if (strncmp(command, "READ", 4) == 0) {
            uint64_t address, size;
            if (check_valid_3words_command(command, &address, &size)) 
                read(arena, address, size);
        } else if (strncmp(command, "WRITE", 5) == 0) {
            uint64_t address, size;
            int8_t *data = malloc(MAX_STRING_SIZE);
            DIE(data == NULL, "Malloc failed");

            if (check_valid_write_command(command, &address, &size, data) != 0) 
                write(arena, address, size, data);
        } else if (strcmp(command, "PMAP") == 0) {
            pmap(arena);
        } else {
            char *p = strtok(command, " ");
            while (p) {
                printf("Invalid command. Please try again.\n");
                p = strtok(NULL, " ");
            }
            break;
        }
    }

    free(arena);
    return 0;
}
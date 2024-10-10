#pragma once
#include <inttypes.h>
#include <stddef.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)


/* TODO : add your implementation for doubly-linked list */

typedef struct Node node_t;

struct Node{
   void* data;
   /* (block_t *)*/
    /* Pentru ca datele stocate sa poata avea orice tip, folosim un
                  pointer la void. */
   node_t *prev, *next;
};

typedef struct
{
   node_t* head;
   uint32_t data_size;
   uint32_t size;
} list_t;

typedef struct {
   uint64_t start_address; 
   size_t size;
   list_t* miniblock_list;
} block_t;

typedef struct {
   uint64_t start_address;
   size_t size;
   uint8_t perm;
   void* rw_buffer;
} miniblock_t;

typedef struct {
	uint64_t arena_size;
	list_t *alloc_list;
} arena_t;

uint8_t check_number(char *p);
uint8_t check_valid_2words_command(char *command, uint64_t *parameter);
uint8_t check_valid_3words_command(char *command, uint64_t *parameter2, uint64_t *parameter3);
uint8_t check_valid_write_command(char *command,  uint64_t *address, uint64_t *size, int8_t *data);

node_t *create_node(const void* new_data, uint32_t data_size);
list_t *create_list(uint32_t data_size);

node_t* remove_nth_node(list_t* list, uint32_t n);
void free_list(list_t** pp_list);

arena_t* alloc_arena(const uint64_t size);
void dealloc_arena(arena_t* arena);

node_t* get_nth_node(list_t* list, uint32_t n);
void add_nth_node(list_t* list, uint32_t n, const void* new_data);

void alloc_block(arena_t* arena, const uint64_t address, const uint64_t size);
void free_block(arena_t* arena, const uint64_t address);

void read(arena_t* arena, uint64_t address, uint64_t size);
void write(arena_t* arena, const uint64_t address,  const uint64_t size, int8_t *data);
void pmap(const arena_t* arena);
void mprotect(arena_t* arena, uint64_t address, int8_t *permission);

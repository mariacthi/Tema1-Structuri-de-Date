#include "vma.h"

uint8_t check_number(char *p) 
{
    /* check if the second word in the command is
    a number */
    uint8_t valid = 0;

    for (uint32_t i = 0; i < strlen(p); i++) {
        if (p[i] == ' ')
            break;
        
        if (!(p[i] >= '0' && p[i] <= '9')) {
            valid = 1;
            break;
        } 
    }
    return valid;
}

uint8_t check_valid_2words_command(char *command, uint64_t *parameter)
{
    uint8_t contor = 0, valid = 0;

    char *p = strtok(command, " ");
    while (p) {
        if (contor == 1) {
            if (check_number(p) == 0) {
                char *q = strtok(p, " ");
                *parameter = atol(q);
            } else
                valid = 1;
        } 
        contor++;
        p = strtok(NULL, " ");
    }

    if (contor == 2 && valid == 0)
        /* if the command only has two words and 
        the second one is a number */
        return 1;
    
    for (int i = 0; i < contor; i++)
        printf("Invalid command. Please try again.\n");
    
    return 0;
}

uint8_t check_valid_3words_command(char *command, uint64_t *parameter2, uint64_t *parameter3)
{
    uint8_t contor = 0, valid1 = 0, valid2 = 0;

    char *p = strtok(command, " ");
    while (p) {
        if (contor == 1) {
            if (check_number(p) == 0) {
                char *q = strtok(p, " ");
                *parameter2 = atol(q);
            } else {
                valid1 = 1;
            }
        } else if (contor == 2) {
            if (check_number(p) == 0) {
                char *q = strtok(p, " ");
                *parameter3 = atol(q);
            } else {
                valid2 = 1;
            }
        }
        contor++;
        p = strtok(NULL, " ");
    }

    if (contor == 3 && valid1 == 0 && valid2 == 0)
        /* if the command only has three words and 
        the last two are numbers */
        return 1;
    
    for (int i = 0; i < contor; i++)
        printf("Invalid command. Please try again.\n");
    
    return 0;
}

uint8_t check_valid_write_command(char *command,  uint64_t *address, uint64_t *size, int8_t *data)
{
    uint8_t contor = 0, valid1 = 0, valid2 = 0;

    char *p = strtok(command, " ");
    while (p) {
        if (contor == 1) {
            if (check_number(p) == 0) {
                char *q = strtok(p, " ");
                *address = atol(q);
            } else {
                valid1 = 1;
            }
        } else if (contor == 2) {
            if (check_number(p) == 0) {
                char *q = strtok(p, " ");
                *size = atol(q);
            } else {
                valid2 = 1;
            }
        }
        contor++;
        if (contor == 3)
            break;
        p = strtok(NULL, " ");
    }

    if (valid1 == 0 && valid2 == 0 && p != NULL) {
        /* if the command only has three words and 
        the last two are numbers */
        memcpy(data, p, strlen(p));
        return 1;
    }

    return 0;
}

node_t *create_node(const void* new_data, uint32_t data_size)
{
	node_t *node = malloc(sizeof(node_t));
	DIE(node == NULL, "Malloc failed");

	node->data = malloc(data_size);
	DIE(node->data == NULL, "Malloc failed");

	memcpy(node->data, new_data, data_size);

	return node;

}

list_t *create_list(uint32_t data_size)
{
	/* TODO */
    list_t *list;
    list = malloc(sizeof(list_t));
    DIE(list == NULL, "Malloc failed");

    list->head = NULL;
    list->data_size = data_size;
    list->size = 0;

    return list;
}

arena_t *alloc_arena(const uint64_t size)
{
    arena_t *arena = malloc(sizeof(arena_t));

    arena->arena_size = size;
    arena->alloc_list = create_list(sizeof(block_t));
    return arena;
}

node_t* remove_nth_node(list_t* list, uint32_t n)
{
	if (!list) {
        fprintf(stderr, "Can't open list");
        return NULL;
    }

	if (!list->size) {
        fprintf(stderr, "The list doesn't have elements");
        return NULL;
    }

	if (n >= list->size - 1)
		n = list->size - 1;

	node_t *removed_node;

	if (n == 0) {
		removed_node = list->head;
		list->head = removed_node->next;
	} else {
		removed_node = get_nth_node(list, n); 
	}

	removed_node->next->prev = removed_node->prev;
	removed_node->prev->next = removed_node->next;

	list->size--;
	if (!list->size)
		list->head = NULL;

    return removed_node;
}

void free_list(list_t** pp_list)
{
    node_t *node;

	if (!pp_list || !(*pp_list)) {
        fprintf(stderr, "Can't open list");
        return;
    }

	while((*pp_list)->size != 0) {
		node = remove_nth_node(*pp_list, 0);

		free(node->data);
		free(node);
	}
	
	free(*pp_list);
	*pp_list = NULL;
}

void dealloc_arena(arena_t *arena)
{
    node_t *node = arena->alloc_list->head;

    while (node) {
        block_t *block = (block_t *) node->data;

        if (block->miniblock_list->size != 0)
            free_list(&block->miniblock_list);

        node = node->next;
    }
    free_list(&arena->alloc_list);    
}

node_t* get_nth_node(list_t* list, uint32_t n)
{
    if (!list) {
        fprintf(stderr, "Can't open list");
        return NULL;
    }

    if (!list->size) {
        fprintf(stderr, "The list doesn't have elements");
        return NULL;
    }
    
	if (n >= list->size)
        n = n % list->size;

    node_t *node = list->head;

    for (unsigned int i = 0; i < n; i++) {
        node = node->next;
	}

	return node;
}

void add_nth_node(list_t* list, uint32_t n, const void* new_data)
{
	if (!list) {
        fprintf(stderr, "Can't open list");
        return;
    }

	if (n > list->size)
		n = list->size;

	node_t *new_node = create_node(new_data, list->data_size);

	if (list->size == 0) {
		new_node->next = new_node;
		new_node->prev = new_node;
		list->head = new_node;
	} else if (n == 0) {
		new_node->next = list->head;
		new_node->prev = list->head->prev;
		
		list->head->prev->next = new_node;
		list->head->prev = new_node;

		list->head = new_node;
	} else {
		node_t *previous_node = get_nth_node(list, n - 1);

		new_node->next = previous_node->next;
		previous_node->next->prev = new_node;

		new_node->prev = previous_node;
		previous_node->next = new_node;
	}

	list->size++;
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
    if (address > arena->arena_size) {
        printf("The allocated address is outside the size of arena\n");
        return;
    } else if (address + size > arena->arena_size) {
        printf("The end address is past the size of the arena\n");
        return;
    }

    node_t *node = arena->alloc_list->head;
    block_t *block = (block_t *)node->data;
    uint32_t pos = 0;
    
    miniblock_t *minib = malloc(sizeof(miniblock_t));
    DIE(minib == NULL, "Malloc failed");

    minib->perm = 6; // "RW-"
    minib->start_address = address;
    minib->size = size;

    while (node) {
        if (address <= block->start_address  && 
            address + size >= block->start_address + block->size) {
            printf("This zone was already allocated\n");
            free(minib);
            break;
        }
        
        if (node->next) {
            block_t *block2 = (block_t *)node->next->data;
            if (address == block->start_address + block->size + 1 &&
                address + size == block2->start_address - 1) {
                // uneste cele doua blocuri
                add_nth_node(block->miniblock_list, block->miniblock_list->size, minib);
                node_t* minib_node = block2->miniblock_list->head;

                while (minib_node) {
                    minib = (miniblock_t *)minib_node->data;
                    add_nth_node(block->miniblock_list, block->miniblock_list->size, minib);
                    minib_node = minib_node->next;
                }

                block->size += size + block2->size;
                free_list(&block2->miniblock_list);
                node->next = remove_nth_node(arena->alloc_list, pos + 1);
                free(node->next->data);
                free(node->next);
                break;
            }
        }

        if (address == block->start_address + block->size + 1) {
            // adauga miniblock la sfarsitul block-ului
            add_nth_node(block->miniblock_list, block->miniblock_list->size, minib);
            block->size += size;
            break;
        }

        if (address + size == block->start_address - 1) {
            // adauga miniblock la inceputul block-ului
            add_nth_node(block->miniblock_list, 0, minib);
            block->size += size;
            block->start_address = address;
            break;
        }
        
        if (node->next || pos == arena->alloc_list->size - 1) {
            block_t *block3 = (block_t *)node->next->data;
            if ((address > block->start_address + block->size + 1 &&
                address + size < block3->start_address - 1) ||
                pos == arena->alloc_list->size - 1) {
                // adauga block nou
                block_t *new_block = malloc(sizeof(block_t));
                DIE (new_block == NULL, "Malloc failed");

                new_block->size = size;
                new_block->start_address = address;
                new_block->miniblock_list = create_list(sizeof(miniblock_t));
                add_nth_node(new_block->miniblock_list, 0, minib);

                add_nth_node(arena->alloc_list, pos, new_block);
                break;
            }
        }
        pos++;
        node = node->next;
    }
}

void free_block(arena_t *arena, const uint64_t address)
{
    node_t *node = arena->alloc_list->head;
   
    uint32_t pos = 0;

    while (node) {
        block_t *block = (block_t *)node->data;
        node_t *minib_node = block->miniblock_list->head;

        uint32_t pos_miniblock = 0;
        uint8_t ok = 0;

        if (address >= block->start_address && address <= block->start_address + block->size) {
            while(minib_node) {
                miniblock_t *miniblock = (miniblock_t *)minib_node->data;

                if (address == miniblock->start_address) {
                    if (block->miniblock_list->size == 1) {
                    minib_node = remove_nth_node(block->miniblock_list, pos_miniblock);
                    free(minib_node);

                    free(block->miniblock_list);
                    node = remove_nth_node(arena->alloc_list, pos);
                    free(node->data);
                    free(node);
                    } else if (pos_miniblock == 0) {
                        minib_node = remove_nth_node(block->miniblock_list, pos_miniblock);
                        block->start_address += miniblock->size;
                        block->size -= miniblock->size;

                        free(minib_node->data);
                        free(minib_node);
                    } else if (pos_miniblock == block->miniblock_list->size - 1) {
                        minib_node = remove_nth_node(block->miniblock_list, pos_miniblock);
                        block->size -= miniblock->size;

                        free(minib_node->data);
                        free(minib_node);
                    } else {
                        //it s in the middle;
                        
                        minib_node->prev = NULL;
                        block->size -= ((miniblock_t *)minib_node->data)->size;

                        block_t *block2 = malloc(sizeof(block_t));
                        block2->miniblock_list = create_list(sizeof(miniblock_t));
                        
                        minib_node = minib_node->next;
                        block2->start_address = ((miniblock_t *)minib_node->data)->start_address;
                        uint32_t contor = 0;

                        while(minib_node) {
                            add_nth_node(block2->miniblock_list, contor, minib_node->data);
                            block2->size += ((miniblock_t *)minib_node->data)->size;
                            minib_node = minib_node->next;
                        }

                        block->size -= block2->size;
                        add_nth_node(arena->alloc_list, pos, block2);
                    }
                    ok = 1;
                    break;
                } 
                pos_miniblock++;
                minib_node = minib_node->next;
            }
        }
        if (ok == 1)
            return;
        
        node = node->next;
        pos++;
    }
    
    printf("Invalid address for free.\n");

}

void read(arena_t *arena, uint64_t address, uint64_t size)
{
    node_t *node = arena->alloc_list->head;
    
    int8_t *message = malloc(size);
    DIE (message == NULL, "Malloc failed");

    while (node) {
        block_t *block = (block_t *)node->data;
        node_t *minib_node = block->miniblock_list->head;

        if (address >= block->start_address && 
            address <= block->start_address + block->size) {
            if (size > block->size) {
                size = block->start_address + block->size - address;
                printf("Warning: size was bigger than the block size."); 
                printf(" Reading %ld characters.\n", size);
            }
            
            uint64_t read = 0;
            while(minib_node && read < size) {
                miniblock_t *miniblock = (miniblock_t *)minib_node->data;

                if (address >= miniblock->start_address && 
                    address <= miniblock->start_address + miniblock->size) {
                    uint64_t diff = miniblock->start_address - address;

                    for (uint64_t i = diff; read < size; i++) {
                        message[read++] = ((char *)miniblock->rw_buffer)[i];
                        if (i == miniblock->size - 1) {
                            minib_node = minib_node->next;
                            miniblock = (miniblock_t *)minib_node->data;
                            i = 0;
                        }
                    }
                }

                if (read == size - 1) {
                    printf("%s\n", message);
                    free(message);
                    return;
                }

                minib_node = minib_node->next;
            }
        }
        node = node->next;
    }
    
    printf("Invalid address for read.\n");
}

void write(arena_t *arena, const uint64_t address, const uint64_t size, int8_t *data)
{
    node_t *node = arena->alloc_list->head;
    uint64_t true_size = size;

    while (node) {
        block_t *block = (block_t *)node->data;
        node_t *minib_node = block->miniblock_list->head;

        if (address >= block->start_address && 
            address <= block->start_address + block->size) {
            if (size > block->size) {
                true_size = block->start_address + block->size - address;
                printf("Warning: size was bigger than the block size.");
                printf(" Writing %ld characters.\n", true_size);
            }
            
            uint64_t written = 0;
            while(minib_node && written < true_size) {
                miniblock_t *miniblock = (miniblock_t *)minib_node->data;

                if (address >= miniblock->start_address && 
                    address <= miniblock->start_address + miniblock->size) {
                    uint64_t diff = miniblock->start_address - address;

                    for (uint64_t i = diff; written < true_size; i++) {
                        ((char *)miniblock->rw_buffer)[i] = data[written++];
                        if (i == miniblock->size - 1) {
                            minib_node = minib_node->next;
                            miniblock = (miniblock_t *)minib_node->data;
                            i = 0;
                        }
                    }
                }

                if (written == true_size - 1)
                    return;

                minib_node = minib_node->next;
            }
        }
        node = node->next;
    }
    
    printf("Invalid address for write.\n");
}

void pmap(const arena_t *arena)
{
    printf("Total memory: 0x%lX bytes\n", arena->arena_size);
    node_t *node = arena->alloc_list->head;
    uint64_t occupied_mem = 0;
    uint32_t alloc_minib = 0;

    while (node) {
        block_t *block = (block_t *)node->data;

        occupied_mem += block->size;
        alloc_minib += block->miniblock_list->size;
    
        node = node->next;
    }

    printf("Free memory: 0x%lX bytes\n", arena->arena_size - occupied_mem);

    printf("Number of allocated blocks: %d\n", arena->alloc_list->size);
    printf("Number of allocated miniblocks: %d\n", alloc_minib);

    node = arena->alloc_list->head;

    uint32_t pos = 1;

    printf("\n");
    
    while (node) {
        block_t *block = (block_t *)node->data;

        printf("\nBlock %d begin\n", pos);
        printf("Zone: 0x%lX - 0x%lX\n", block->start_address, block->start_address + block->size);

        node_t *minib_node = block->miniblock_list->head;
        uint32_t pos_minib = 1;

        while (minib_node) {
            miniblock_t *miniblock = (miniblock_t *)minib_node->data;

            printf("Miniblock %d:\t\t0x%lX\t\t-\t\t0x%lX\t\t| ", pos_minib, miniblock->start_address,
                    miniblock->start_address + miniblock->size);
            for (int i = 0; i < 2; i++) {
                printf("%c", ((char *)miniblock->rw_buffer)[i]);
            }
            printf("\n");

            pos_minib++;
            minib_node = minib_node->next;
        }
        printf("Block %d end\n", pos);

        pos++;
        node = node->next;
    }
}

/*void mprotect(arena_t *arena, uint64_t address, int8_t *permission)

{

}
*/
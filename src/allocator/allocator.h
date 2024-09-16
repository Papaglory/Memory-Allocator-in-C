#include<stddef.h>

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "../linked_list/linked_list.h"

typedef struct {
    unsigned char* sub_heap;
    size_t size;

    LinkedList list;

} Allocator;

/*
* @brief Given the size of the desired sub heap, an allocator
* will be created that manages this sub heap.
*
* @param The size of the sub heap that will be allocated.
* @return Returns a pointer to the created Allocator.
*/
Allocator* create_allocator(size_t size);

/*
* $brief Destory the given term object and free
* it from the heap.
*
* @param Pointer to the object to be destroyed.
*/
void destroy_allocator(Allocator* term);

void* allocator_malloc(size_t size);

void* allocator_realloc(size_t size);

/*
* @brief Free up the memory corresponding to the pointer.
*
* @param Pointer to the object to be freed.
*/
void* allocator_free(void* ptr);

#endif

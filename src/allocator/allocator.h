#include<stddef.h>

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "../linked_list/linked_list.h"

typedef struct {
    // Pointer to the sub_heap
    void* sub_heap;

    // Size of the subheap
    size_t size;

    /*
    * List to keep track of what memory is free and used.
    *
    * The list starts with a single Node that represents
    * all the free memory. When memory is requested,
    * the Node will share the required memory by splitting
    * up into one Node for the allocated memory and one
    * for the remaining free memory.
    *
    * The payload of each Node will be a pointer to the
    * start of the memory block and a size_t to
    * indicate the size of the memory block.
    * There will also be a boolean variable to tell
    * whether the memory block is free or used.
    */
    LinkedList list;

} Allocator;

/*
* @brief Given the size of the desired sub heap, an allocator
* will be created that manages this sub heap. The allocator
* will utilize a portion of the sub heap for its own metadata.
*
* @param The size of the sub heap that will be allocated.
* @return Returns a pointer to the created Allocator.
*/
Allocator* create_allocator(size_t size);

/*
* @brief Given the input 'size', allocate memory on the
* sub heap.
*
* @param Determines the size of memory to allocate.
* @return Returns a pointer to the allocated memory.
*/
void* allocator_malloc(Allocator* alloc, size_t size);

/*
* @brief Free up the memory corresponding to the pointer.
*
* @param Pointer to the object to be freed.
*/
void* allocator_free(Allocator* alloc, void* ptr);

void* allocator_realloc(Allocator* alloc, size_t size);

/*
* $brief Destory the given term object and free
* it from the heap.
*
* @param Pointer to the object to be destroyed.
*/
void destroy_allocator(Allocator* alloc);

#endif

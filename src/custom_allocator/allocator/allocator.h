/**
 * @file allocator.h
 * @brief Custom Memory Allocator
 * This file defines a custom memory allocator (Allocator)
 * for managing a portion of the heap supplied by the
 * built-in C allocator. By making a call to malloc(heap_size)
 * where 'heap_size' is the size of the desired sub-heap,
 * an Allocator will be assigned to manage this memory through
 * custom allocator functions.
 *
 * @details
 * The Allocator object will reside within the heap
 * it is managing. This means that the end of the managed
 * heap is reserved for Allocator metadata i.e. an Allocator
 * object, a LinkedList and a single Node for initial user
 * memory pool. Furthermore, the managed heap will have a
 * dual structure. At the top (end of the managed memory)
 * there will be Allocator metadata, referred to as the
 * reserved pool. As more of the managed heap is used, the
 * reserved pool will grow downwards. Analogously, the
 * memory for actual use, called the user pool will
 * start at the bottom (start of the managed memory) and
 * grow upwards. This is inspired by the duality of
 * the stack and the heap for memory management.
 *
 * A LinkedList is utilized to keep track of what memory
 * within the managed heap is currently free and in use.
 * The list starts with a single Node that represents
 * all the initial free memory. When memory is requested,
 * the Node will share the required memory by splitting
 * up into one Node for the allocated memory and one
 * for the remaining free memory.
 *
 * The payload of each Node will be a pointer to the
 * start of the memory block and a size_t to
 * indicate the size of the memory block.
 * There will also be a boolean variable to tell
 * whether the memory block is free or in use.
 *
 * In order to now have to modify the function prototypes
 * for the LinkedList and Node by having to pass an
 * Allocator object to use the Allocator functions like
 * Allocator_malloc() or Allocator_free(), a static variable
 * 'current_alloc' is initialized in the allocator.c file.
 * This makes it so that the custom allocator functions
 * reference the Allocator object to use internally, rather
 * than having to pass it through as a function argument.
 * With this approach, there have been dedicated functions,
 * set() and release(), to assign Allocator objects to the
 * 'current_allocator' pointer variable.
 *
 * TERMINOLOGY:
 *
 * managed heap / sub-heap: The memory managed my
 * an Allocator object.
 *
 * Memory pool: All the memory within the managed heap.
 * Reserved pool: The memory within the managed heap dedicated
 *
 * to initialize an Allocator during its creation.
 * User pool: The memory within the managed heap dedicated for
 * use by users or clients.
 *
 * TODO ADD MORE?
 *
 */

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "../linked_list/linked_list.h"
#include<stddef.h>
#include <stdbool.h>

typedef struct {
    // Pointer to the start of the managed heap
    char* heap_start;

    // Pointer to the end of the managed heap
    char* heap_end;

    /*
     * Pointer to the lower border of the reserved pool.
     * As the reserved pool grows downwards, this is where
     * new metadata will be inserted.
     */
    char* reserved_pool_border;

    // Size of the managed heap
    size_t heap_size;

    // Size of the reserved pool
    size_t reserved_pool_size;

    LinkedList* list;

} Allocator;

/*
* @brief Given the size of the desired managed heap, an allocator
* will be created that manages this heap. The allocator
* will utilize a portion of the managed heap for its own metadata.
*
* @param The size of the sub heap that will be allocated.
* @return Returns a pointer to the created Allocator.
*/
Allocator* create_allocator(size_t size);

/*
* @brief Increase the reserved pool of the Allocator pointed to
* by 'current_alloc'. This will shift and increase the Allocator's
* member variables 'reserved_pool_border' and 'reserved_pool_size'
* respectively with function argument 'increase'.
*
* @param The amount to increase the reserved pool.
*/
void increase_reserved_pool(size_t increase);

/*
* @brief Create a metadata Node stores data about the start of a
* memory block, its size, and whether it is free or in use.
*
* @param1 A pointer to the start of the memory block.
* @param2 The size of the memory block.
* @param3 whether the memory block is free or in use.
* @return A Node containing the data from the fucntion argument.
*/
Node* create_metadata_node(char* memory_start, size_t block_size, bool is_free);

/*
* @brief Given the input 'size', allocate memory on the
* sub heap.
*
* @param Determines the size of memory to allocate.
* @return Returns a pointer to the allocated memory.
*/
void* allocator_malloc(size_t size);

/*
* @brief Free up the memory corresponding to the pointer.
*
* @param Pointer to the object to be freed.
*/
void* allocator_free(void* ptr);

// TODO Add comments
void* allocator_realloc(size_t size);

/*
* $brief Destory the given term object and free
* it from the heap.
*
* @param Pointer to the object to be destroyed.
*/
void destroy_allocator(Allocator* alloc);

/*
* @brief Set a new Allocator object to the allocator functions.
* It is not necessary to call 'release_allocator' before setting
* a new allocator. This sets the 'current_allocator' pointer to
* point to the new Allocator object.
*
* @param The Allocator to be set.
*/
void set_allocator(Allocator* alloc);

/*
* @brief Release the current Allocator from the allocator functions,
* effectively setting the 'current_allocator' pointer to NULL.
*
* @param The Allocator to be released.
*/
void release_allocator();

#endif

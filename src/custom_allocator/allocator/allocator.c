#include <stddef.h>
#include <stdlib.h>
#include "allocator.h"
#include "../other_modules/memory_triplet.h"

// The Allocator currently connected to the Allocator functions
static Allocator* current_alloc = NULL;

Allocator* create_allocator(size_t heap_size) {

    // Use the built-in C malloc to get the managed heap
    void* sub_heap = malloc(heap_size);


    // Make a lower bound to input size?
    //
    // TODO Maybe have the sub_heap pointer point to the actual start
    // of useful heap, that is, skip alloc and list??
    //
    // TODO Calculate the total needed to initialize Allocator to
    // create a lower bound for input size
    //

    /*
    * Keeps track of memory used within the heap for
    * Allocator metadata.
    */
    size_t reserved_memory = sizeof(Allocator);
    Allocator* alloc = (Allocator*) sub_heap;

    // Initialize the LinkedList in memory block after allocator
    LinkedList* list = sub_heap + reserved_memory;
    reserved_memory += sizeof(LinkedList);

    // Set LinkedList member variables
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->next_id = 0;

    // Set Allocator member variables
    alloc->heap = sub_heap;
    alloc->heap_size = heap_size;
    alloc->list = list;


    // Create the first Node containing the user memory pool
    void* memory_start = sub_heap + sizeof(Allocator) + sizeof(LinkedList);
    size_t block_size = heap_size - sizeof(Allocator) + sizeof(LinkedList);
    bool is_free = true;
    create_memory_triplet(memory_start, block_size, is_free);

}

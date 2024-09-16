#include <stddef.h>
#include <stdlib.h>
#include "allocator.h"

/*
* Variable points to the allocator currently connected
* to the allocator functions.
*/
static Allocator* current_alloc = NULL;

Allocator* create_allocator(size_t size) {

    // Use the built-in C malloc to get the sub-heap
    void* sub_heap = malloc(size);

    // Reserve the start or the end for the allocator metadata?
    // Make the reservation dynamic?
    // Make a lower bound to input size?
    //
    // TODO Maybe have the sub_heap pointer point to the actual start
    // of useful heap, that is, skip alloc and list??
    //
    //
    // TODO Calculate the total needed to initialize Allocator to
    // create a lower bound for input size

    int allocator_size = sizeof(Allocator);

    // Initialize the allocator at the start of sub_heap
    Allocator* alloc = (Allocator*) sub_heap;

    // Initialize the LinkedList in memory block after allocator
    LinkedList* list = sub_heap + allocator_size;

    // Set LinkedList member variables
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->next_id = 0;

    // Set Allocator member variables
    alloc->sub_heap = sub_heap;
    alloc->size = size;
    alloc->list = list;


    // Create Node to represent the pool of memory that is not reserved
    // by the Allocator

    // Is the Node with the pool the last of metadata reserved???

}

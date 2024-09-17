#include <stddef.h>
#include <stdlib.h>
#include "allocator.h"
#include "../other_modules/memory_triplet.h"
#include "../linked_list/node.h"

/**
 * The Allocator currently referenced to when
 * using the Allocator functions.
 */
static Allocator* current_alloc = NULL;

Allocator* create_allocator(size_t heap_size) {

    // The heap size must be at least this tall to ride
    size_t initial_reserved_pool_size = sizeof(Allocator) + sizeof(LinkedList) + sizeof(MemoryTriplet) + sizeof(Node);
    if (heap_size < initial_reserved_pool_size) {

        return NULL;

    }

    // Utilize the built-in C malloc to acquire the managed heap
    void* heap_start = malloc(heap_size);

    if (heap_start == NULL) {

        // Memory error from malloc()
        return NULL;

    }

    // Pointer to the end of the heap
    void* heap_end = heap_start + heap_size;

    // Initialize Allocator object at the end of heap
    Allocator* alloc = (Allocator*) heap_end;

    // Set Allocator member variables
    alloc->heap_start = heap_start;
    alloc->heap_end = heap_start + heap_size;
    alloc->reserved_pool_start = heap_start + heap_size;
    alloc->heap_size = heap_size;
    alloc->reserved_pool_size = sizeof(Allocator); // Keeps track of memory used for metadata

    // Initialize LinkedList object
    LinkedList* list = heap_end - alloc->reserved_pool_size;

    // Set LinkedList member variables
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->next_id = 0;

    // Add memory used by LinkedList to the reserved pool size
    alloc->reserved_pool_size += sizeof(LinkedList);

    // Set Allocator member variable
    alloc->list = list;

    // Initialize a Node referencing the entire user memory pool
    void* memory_start = heap_start;
    size_t block_size = heap_size - alloc->reserved_pool_size - sizeof(MemoryTriplet);
    bool is_free = true;
    Node* node = create_metadata_node(memory_start, block_size, is_free);

    // Add memory used by Node to the reserved pool size
    alloc->reserved_pool_size += sizeof(Node);

    // Do I need a function that creates a Node internally with the allocator
    // Like maybe create block, so I dont operate with create_memory_triplet
    // and create_node here directly, but for example, create_block??
    //
    // The current issue is that the create_memory_triplet and create_node
    // utilize the allocator_malloc() functions.

    add(list, node);

    return alloc;
}


Node* create_metadata_node(void* memory_start, size_t block_size, bool is_free) {

    MemoryTriplet* triplet = create_memory_triplet(memory_start, block_size, is_free);



    // Add memory used by MemoryTriplet to the reserved pool size
    current_alloc->reserved_pool_size += sizeof(MemoryTriplet);

    // Create Node containing the memory_triplet
    Node* node = create_node((void*) triplet, sizeof(MemoryTriplet));


    return NULL;

}


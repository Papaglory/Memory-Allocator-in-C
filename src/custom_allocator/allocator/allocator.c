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
    size_t initial_reserved_pool_size =
        sizeof(Allocator)
        + sizeof(LinkedList)
        + sizeof(MemoryTriplet)
        + sizeof(Node);
    if (heap_size < initial_reserved_pool_size) {

        return NULL;

    }

    // Utilize the built-in C malloc to acquire the managed heap
    char* heap_start = (char*) malloc(heap_size);

    if (heap_start == NULL) {

        // Memory error from malloc()
        return NULL;

    }

    // Pointer to the end of the heap
    char* heap_end = heap_start + heap_size;

    // Initialize Allocator object at the end of heap
    Allocator* alloc = (Allocator*) heap_end;

    // Set Allocator member variables
    alloc->heap_start = heap_start;
    alloc->heap_end = heap_end;
    alloc->user_pool_border = heap_start;
    alloc->reserved_pool_border =  heap_end - sizeof(Allocator);
    alloc->heap_size = heap_size;
    alloc->reserved_pool_size = sizeof(Allocator); // Keeps track of memory used for metadata

    /*
     * Set the Allocator being used to let Allocator functions
     * know which Allocator object to process.
     */
    set_allocator(alloc);

    // Initialize LinkedList object
    LinkedList* list = (LinkedList*) alloc->reserved_pool_border;

    // Set LinkedList member variables
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->next_id = 0;

    // Increase the reserved pool to accommodate for the LinkedList
    increase_reserved_pool(sizeof(LinkedList));

    // Set Allocator member variable
    alloc->list = list;

    // Initialize a Node referencing the entire user memory pool
    void* memory_start = heap_start;
    /*
    * In determining the size of the initial user pool we also account
    * for the Node which will be created and stored in the reserved pool.
    * Note that the payload of Node is a MemoryTriplet object.
    * Therefore, we subtract the size of Node and MemoryTriplet from
    * the initial user pool.
    */
    size_t block_size =
        heap_size
        - alloc->reserved_pool_size
        - sizeof(Node)
        - sizeof(MemoryTriplet);
    bool is_free = true;
    Node* node = create_metadata_node(memory_start, block_size, is_free);

    add(list, node);

    return alloc;
}

void increase_reserved_pool(size_t increase) {

    if (current_alloc == NULL) {

        // There is no Allocator to operate on
        return;

    }

    // Retrieve the memory pool borders
    char* user_border = current_alloc->user_pool_border;
    char* reserved_border = current_alloc->reserved_pool_border;

    // Check if pools overlap if we incrase the reserved pool
    if (user_border > reserved_border - increase) {

        // The user pool border has reached the reserved pool border

        // Attempt to reduce memory fragmentation in each pool
        cleanse_user_pool();
        cleanse_reserved_pool();

        // Retrieve the cleansed memory pool borders
        char* user_border = current_alloc->user_pool_border;
        char* reserved_border = current_alloc->reserved_pool_border;

        // Check if pool cleansing prevents pool overlap
        if (user_border > reserved_border - increase) {

            // Cleaning up the pools did not work, heap is considered full
            return;

        }

    }

    // Shift the border of the reserved pool downwards
    current_alloc->reserved_pool_border -= increase;
    current_alloc->reserved_pool_size += increase;

}

Node* create_metadata_node(char* memory_start, size_t block_size, bool is_free) {

    if (current_alloc == NULL || memory_start == NULL) {

        // There is no Allocator to work on or function argument missing!
        return NULL;

    }

    MemoryTriplet* triplet = (MemoryTriplet*) current_alloc->reserved_pool_border;

    // Set MemoryTriplet member variables
    triplet->memory_start = (void*) memory_start; // TODO make memory_start char*?
    triplet->block_size = block_size;
    triplet->is_free = is_free;

    // Increase the reserved pool to accommodate for the MemoryTriplet
    increase_reserved_pool(sizeof(MemoryTriplet));

    // Create Node containing the MemoryTriplet
    Node* node = (Node*) current_alloc->reserved_pool_border;

    // Set Node member variables
    node->data = (void*) triplet;
    node->data_size = sizeof(MemoryTriplet);
    node->next = NULL;
    node->id = 0;

    // Increase the reserved pool to accommodate for the Node
    increase_reserved_pool(sizeof(Node));

    return NULL;

}



void* allocator_malloc(size_t size) {

    if (current_alloc == NULL) {

        // There is no Allocator to operate on
        return NULL;

    }


    /*
     *
     *
     * Check if there is space?
     *
     * Check the linked list for vacant nodes.
     *
     * Optimiztion: Find the best fitting node in terms
     * of memory block size? Better than naive but more
     * calculations.
     *
     *
     * Prioritize nodes that are lower in memory compared
     * to higher to avoid raising the user pool border needlessly
     *
     *
     * Order:
     * Check if there is a node with space.
     * If yes, accept this node, but dont use it yet.
     * Then start creating the metadata node and see if there is
     * space in reserved pool.
     * If metadata node was successfully created, then insert the
     * actual data into the accepted node.
     *
     */

    // Attempt to find a Node with an available memory block
    Node* available_node = naive_search(size);

    if (available_node == NULL) {

        /*
         * No available Node was found, start pool cleansing
         * to potensially reduce memory fragmentation and get
         * more space.
         */
        cleanse_user_pool();
        cleanse_reserved_pool();

        // Try again to find an available memory block
        available_node = naive_search(size);

        if (available_node == NULL) {

            return NULL;

        }

    }

    /*
     * A Node has been found. Inspect Node to see how to split
     * up into used memory and free memory Nodes.
     *
     * With the split information, create a new metadata node.
     *
     * If the Node carries the exact required memory, then simply
     * set the 'is_free' to true and use the Node as is.
     */

    return NULL;

}


Node* naive_search(size_t size) {

    Node* meta_node = current_alloc->list->head;

    if (meta_node == NULL) {

        return NULL;

    }

    return NULL;

}


void destroy_allocator() {

    /*
    *
    * Loop through the linked list and free each of the nodes and their
    * corresponding payload, MemoryTriplet.
    *
    *
    * TODO In reality, all I have to do is call C's free() because
    * everything is on the managed heap and by freeing it everything
    * will be handled.
    * However, I wonder if I should do the it the other way for
    * practice with memory management??
    *
    *
    */


}


void set_allocator(Allocator* alloc) {


    if (alloc == NULL) {

        return;

    }

    current_alloc = alloc;

}

void release_allocator() {

    current_alloc = NULL;

}



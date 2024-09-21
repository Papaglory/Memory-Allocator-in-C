#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "allocator.h"
#include "../other_modules/memory_data.h"
#include "../linked_list/node.h"

/**
 * The Allocator currently referenced to when
 * using the Allocator functions.
 */
static Allocator* current_alloc = NULL;

Allocator* create_allocator(size_t heap_size) {

    /*
     * The heap size must be at least this large to accommodate the
     * initial Allocator metadata memory needed.
     */
    size_t initial_reserved_pool_size =
        sizeof(Allocator)
        + sizeof(LinkedList)
        + sizeof(MemoryData)
        + sizeof(Node);
    if (heap_size <= initial_reserved_pool_size) {

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
    alloc->initial_reserved_pool_size = initial_reserved_pool_size;
    alloc->reserved_pool_border =  heap_end - sizeof(Allocator);
    alloc->heap_size = heap_size;
    alloc->reserved_pool_size = sizeof(Allocator);

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
    * Note that the payload of Node is a MemoryData object.
    * Therefore, we subtract the size of Node and MemoryData from
    * the initial user pool.
    */
    size_t block_size =
        heap_size
        - alloc->reserved_pool_size
        - sizeof(Node)
        - sizeof(MemoryData);
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

    // Check if pools overlap if we increase the reserved pool
    if (user_border > reserved_border - increase) {

        /*
         * The user pool border has reached the reserved pool border.
         * Attempt to reduce memory fragmentation in each pool.
         */
        cleanse_user_pool();
        cleanse_reserved_pool();

        // Retrieve the cleansed memory pool borders
        user_border = current_alloc->user_pool_border;
        reserved_border = current_alloc->reserved_pool_border;

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

    // Create Node containing the MemoryData
    Node* node = (Node*) current_alloc->reserved_pool_border;

    // Set Node member variables
    node->data_size = sizeof(MemoryData);
    node->next = NULL;
    node->id = 0;

    // Increase the reserved pool to accommodate for the Node
    increase_reserved_pool(sizeof(Node));

    MemoryData* data = (MemoryData*) current_alloc->reserved_pool_border;

    // Set MemoryData member variables
    data->memory_start = memory_start;
    data->block_size = block_size;
    data->is_free = is_free;
    data->in_use = true;

    // Increase the reserved pool to accommodate for the MemoryData
    increase_reserved_pool(sizeof(MemoryData));

    // Assign the data to the Node
    node->data = (void*) data;

    return node;

}

/*
 * @details Loop through the metadata Nodes in the reserved pool
 * from high memory to lower memory. If a metadata Node that is
 * not in use is found, then move the metadata Node at the pool
 * border to take its place and update the border pointer and
 * reserved pool size.
 *
 * A metadata Node consits of a Node and a MemoryData object.
 * Therefore, when going from high memory to lower memory, we
 * have the order:
 *
 * HIGH MEMORY
 *
 * Node
 *
 * MemoryData
 *
 * LOW MEMORY.
 */
void cleanse_reserved_pool() {

    if (current_alloc == NULL) {

        // There is no Allocator to operate on
        return;

    }

    // This will be the increments when doing metadata Node traversal
    size_t meta_data_node_size = sizeof(Node) + sizeof(MemoryData);

    // Memory location for the start of metadata Node traversal
    char* meta_data_node =
        current_alloc->heap_end
        - current_alloc->initial_reserved_pool_size;

    // Iterate through the metadata Nodes
    while (meta_data_node > current_alloc->reserved_pool_border) {

        // Retrieve Node's corresponding MemoryData
        MemoryData* data = (MemoryData*) (meta_data_node - sizeof(Node));

        if (data->in_use == false) {

            /*
             * Found an available metadata Node space.
             */

            Node* border_node = (Node*) current_alloc->reserved_pool_border;
            MemoryData* border_data = (MemoryData*) border_node->data;

            /*
             * Check for unintended situations:
             * The border Node is not in use, thus the border is
             * located at a wrong memory address!
             *
             * Or The border Node is at a higher memory address
             * than the address where we want to move it to.
             * We are thus done with the cleansing.
             */
            if (border_data->in_use == false || (char*) border_node > meta_data_node) {

                return;

            }

            // Move the border Node to the vacant Node space
            memcpy(meta_data_node, border_node, meta_data_node_size);

            // Update the reserved pool border
            char* new_border = current_alloc->reserved_pool_border;
            size_t new_pool_size = current_alloc->reserved_pool_size;
            while (new_border < meta_data_node) {

                // Move the border to new potensial location
                new_border += meta_data_node_size;
                new_pool_size -= meta_data_node_size;

                Node* new_border_node = (Node*) current_alloc->reserved_pool_border;
                MemoryData* new_border_data = (MemoryData*) border_node->data;

                // Check if the Node is in use
                if (new_border_data->in_use == false) {

                    // Metadata Node is not in use, can therefore move border further
                    continue;

                }

                // Node is in use, new border will therefore be here
                current_alloc->reserved_pool_border = new_border;
                current_alloc->reserved_pool_size = new_pool_size;

            }

        }

        // Update traversal data
        meta_data_node -= meta_data_node_size;

    }

}

void* allocator_malloc(size_t required_size) {

    if (current_alloc == NULL) {

        // There is no Allocator to operate on
        return NULL;

    }

    // Attempt to find a Node with an available memory block
    Node* available_node = naive_search(required_size);

    if (available_node == NULL) {

        /*
         * No available Node was found, start pool cleansing
         * to potensially reduce memory fragmentation and get
         * more space.
         */
        cleanse_user_pool();
        cleanse_reserved_pool();

        // Try again to find an available memory block
        available_node = naive_search(required_size);

        if (available_node == NULL) {

            return NULL;

        }

    }

    /*
     * A Node has been found. Inspect Node to see how to split
     * up into allocated memory Node and residual memory Node.
     */

    // Retrieve Node data
    MemoryData* data = (MemoryData*) available_node->data;
    size_t node_block_size = data->block_size;

    if (node_block_size == required_size) {

        /*
         * Node carries exact required memory block size.
         * Can then just use the Node as is.
         */
        data->is_free = false;

        // Return the pointer to the start of the allocated memory
        return data->memory_start;

    }

    // Modify 'available_node' to reflect that it is in use
    data->block_size = required_size;
    data->is_free = false;

    // Construct the residual metadata Node
    char* residual_memory_start = data->memory_start + required_size;
    size_t residual_memory_size = node_block_size - required_size;

    create_metadata_node(residual_memory_start, residual_memory_size, true);

    // Return the pointer to the start of the allocated memory
    return data->memory_start;

}


Node* naive_search(size_t size) {

    Node* node = current_alloc->list->head;
    Node* found_node = NULL;

    // Loop through the list and find the first vacant Node
    while (node != NULL) {

        MemoryData* data = (MemoryData*) node->data;

        if (size <= data->block_size && data->is_free) {

            // A Node has been found
            found_node = node;
            break;

        }

        // Update for next iteration
        node = node->next;

    }

    return found_node;

}

void* allocator_free(void* ptr) {

    /*
    *
    *
    *
    */

    if (current_alloc == NULL) {

        // There is no Allocator object to process
        return NULL;

    }



    // Loop through the LinkedList searching for a Node
    // corresponding with the argument pointer.


    LinkedList* list = current_alloc->list;

    Node* node = list->head;

    if (node == NULL) {

        // There is no head Node, a bug has occured
        return NULL;

    }

    while (true) {

        MemoryData* data = (MemoryData*) node->data;

        if (node->id == list->tail->id) {



        }

        if (data->memory_start == ptr) {

            // Node has been found
            // The memory is to be set free
            // The node and memory block is to be merged
            // with any adjacent nodes / blocks


        }

        node = node->next;

    }



    return NULL;

}



void destroy_allocator() {

    if (current_alloc == NULL) {

        // There is no Allocator object to process
        return;

    }

    // Free the managed heap
    free(current_alloc->heap_start);

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

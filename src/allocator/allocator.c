#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "allocator.h"
#include "../other_modules/memory_data.h"
#include "../linked_list/node.h"
#include "../linked_list/linked_list_iterator.h"

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

    MemoryData* data = (MemoryData*) current_alloc->reserved_pool_border;

    // Set MemoryData member variables
    data->memory_start = memory_start;
    data->block_size = block_size;
    data->is_free = is_free;
    data->in_use = true;

    // Increase the reserved pool to accommodate for the MemoryData
    increase_reserved_pool(sizeof(MemoryData));

    // Create Node containing the MemoryData
    Node* node = (Node*) current_alloc->reserved_pool_border;

    // Set Node member variables
    node->data_size = sizeof(MemoryData);
    node->next = NULL;
    node->id = 0;
    node->data = (void*) data;

    // Increase the reserved pool to accommodate for the Node
    increase_reserved_pool(sizeof(Node));

    return node;

}

/*
* @brief Merge the two metadata Nodes into a single Node. The
* left Node will remain while the right Node will be discarded.
* Thereafter, the discarded Node is removed from the LinkedList.
*
* @param1 The LinkedList containing the Nodes.
* @param2 The left Node.
* @param3 The right Node.
* @return A pointer to the merged Node (same address as 'left_node').
*/
Node* merge_meta_data_nodes(LinkedList* list, Node* left_node, Node* right_node) {

    if (list == NULL || left_node == NULL || right_node == NULL) {

        // Function argument(s) missing
        return NULL;

    }

    MemoryData* left_data = left_node->data;
    MemoryData* right_data = right_node->data;

    // Check that the 'left_node' is left adjacent to 'right_node'
    if (left_data->memory_start + left_data->block_size != right_data->memory_start) {

        return NULL;

    }

    // merge the block sizes
    left_data->block_size += right_data->block_size;

    // Mark the right Node as vacant
    right_data->in_use = false;
    right_data->is_free = true;

    // Remove the discarded Node from the LinkedList
    size_t id = right_node->id;
    delete_node(list, id);

    return left_node;

}

void cleanse_user_pool() {


    if (current_alloc == NULL) {

        // There is no Allocator to operate on
        return;

    }


    /*
     *
     *
     * Merge adjacent Nodes??
     * Only merge nodes that are both free and adjacent.
     * Move higher memory allocated blocks to lower memory if possible.
     *
     *
     *
     *
     *
     * TODO for improvements, implement merge sort for linked list.
     * sort linked list: O(nlogn)
     * merge adjacent nodes: O(n)
     *
     *
     *
     *
     * For each node, go through the list and see if there is any
     * nodes to merge with.
     *
     *
     * k
     */

    LinkedList* list = current_alloc->list;

    LinkedListIterator iter;
    iter.current = get_head(list);

    while (has_next(&iter)) {

        Node* node = next(&iter);
        MemoryData* data = node->data;

        char* memory_start = data->memory_start;
        char* memory_end = memory_start + data->block_size;

        size_t id = node->id;

        if (data->is_free == false) {

            // Only Nodes with free memory blocks can be merged
            continue;

        }

        // Look for Nodes with adjacent memory blocks
        LinkedListIterator inner_iter;
        inner_iter.current = get_head(list);

        while (has_next(&inner_iter)) {

            Node* inner_node = next(&inner_iter);
            MemoryData* inner_data = inner_node->data;

            char* inner_memory_start = inner_data->memory_start;
            char* inner_memory_end = inner_memory_start + inner_data->block_size;

            size_t inner_id = inner_node->id;

            if (inner_data->is_free == false || id == inner_id) {

                // Only interested in free and unique Nodes
                continue;

            }

            // Check if the inner Node is a right adjacent Node to merge with
            if (memory_start == memory_end) {

                merge_meta_data_nodes(list, node, inner_node);

            }

            // Check if the inner Node is a left adjacent Node to merge with
            if (inner_memory_end == memory_start) {

                merge_meta_data_nodes(list, inner_node, node);

            }


        } // End while

    } // End while

    // The Nodes have been merged. Now time to try and move them to lower memory




    /*
     *
     * I could just move everything down, like slide down the whole thing?
     * Every node will be moved at most once.
     *
     * Does the list need to be sorted then? Yes because I would always
     * want to retrieve the memory block that is next.
     *
     *
     *
     *
     */



}

/*
 * @details
 * Loop through the metadata Nodes in the reserved pool
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
        - current_alloc->initial_reserved_pool_size
        - meta_data_node_size; // Need this because  we go backwards in memory

    // Iterate through the metadata Nodes
    while (meta_data_node > current_alloc->reserved_pool_border) {

        // Retrieve Node's corresponding MemoryData
        MemoryData* data = (MemoryData*) (meta_data_node + sizeof(Node));

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

            // Move the border metadata Node to the vacant space
            memcpy(meta_data_node, border_node, meta_data_node_size);

            // Update the reserved pool border
            char* new_border = current_alloc->reserved_pool_border;
            size_t new_pool_size = current_alloc->reserved_pool_size;
            while (new_border < meta_data_node) {

                // Move the border to new potential location
                new_border += meta_data_node_size;
                new_pool_size -= meta_data_node_size;

                Node* new_border_node = (Node*) current_alloc->reserved_pool_border;
                MemoryData* new_border_data = (MemoryData*) new_border_node->data;

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
         * to potentially reduce memory fragmentation and get
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
    bool is_free = false;

    create_metadata_node(residual_memory_start, residual_memory_size, is_free);

    // Return the pointer to the start of the allocated memory
    return data->memory_start;

}

Node* naive_search(size_t size) {

    // Loop through the LinkedList and find the first vacant memory block
    LinkedListIterator iter;
    iter.current = current_alloc->list->head;
    Node* found_node = NULL;
    while (has_next(&iter)) {

        Node* node = next(&iter);
        MemoryData* data = (MemoryData*) node->data;

        if (data->is_free && size <= data->block_size) {

            // A Node has been found
            found_node = node;
            break;

        }

    }

    return found_node;

}

/*
 * @details
 * When merging to the left adjacent Node, we only have to check if the
 * start of the memory block is equal to the 'memory_start' + 'block_size'
 * of the left adjacent Node (and that the left adjacent Node is free
 * of course).
 *
 * Analogously for the right adjacent Node, we compare the
 * 'memory_start' + 'block_size' to the 'memory_start' of the right
 * adjacent Node.
 *
 * The Node that is to be set free should (if no bug has occured) at most
 * merge with two Nodes, both being adjacent. Assuming this is not the case,
 * say it merges with the right adjacent Node, and then this newly merged
 * Node again merges with its right adjacent Node, then the two Nodes
 * that was merged with the free Node should have been merged beforehand.
 * Therefore, we can assume that is the case, thus we only have to do
 * at most two merges.
 */
void allocator_free(void* ptr) {

    if (current_alloc == NULL) {

        // There is no Allocator object to process
        return;

    }

    LinkedList* list = current_alloc->list;

    // Create iterator on the stack
    LinkedListIterator iter;
    iter.current = get_head(list);

    if (get_head(list) == NULL) {

        // There is no head Node, a bug has occured
        return;

    }

    // Search for the Node corresponding to 'ptr' in the LinkedList
    Node* matched_node;
    while (has_next(&iter)) {

        Node* node = next(&iter);
        MemoryData* data = (MemoryData*) node->data;

        if (data->memory_start == ptr) {

            // Node has been found
            matched_node = node;
            break;

        }

        if (node->id == list->tail->id) {

            // End of list and there is no corresponding Node
            return;

        }

        node = node->next;

    }

    MemoryData* matched_data = (MemoryData*) matched_node->data;


    // Reset the iterator and look for adjacent Nodes
    iter.current = get_head(list);

    char* matched_memory_start = matched_data->memory_start;
    char* matched_memory_end = matched_memory_start + matched_data->block_size;

    bool has_merged = false;
    while (has_next(&iter)) {

        Node* node = next(&iter);
        MemoryData* data = node->data;

        char* memory_start = data->memory_start;
        char* memory_end =
            memory_start
            + data->block_size;

        // Check if there is a right adjacent Node to merge with
        if (memory_start == matched_memory_end) {

            merge_meta_data_nodes(list, matched_node, node);
            has_merged = true;

        }

        // Check if there is a left adjacent Node to merge with
        if (memory_end == matched_memory_start) {

            merge_meta_data_nodes(list, node, matched_node);
            has_merged = true;

        }

    }

    if (has_merged == false) {

        /*
        * No adjacent Nodes were found.
        * Thus we keep this Node in the LinkedList.
        */
        matched_data->is_free = true;

    }

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

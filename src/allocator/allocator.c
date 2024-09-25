#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "allocator.h"
#include "../other_modules/memory_data.h"
#include "../linked_list/node.h"
#include "../linked_list/linked_list_iterator.h"
#include "../linked_list/merge_sort_linked_list.h"

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

    LinkedList* list = current_alloc->list;

    merge_sort_list(list);

    /*
     * Go through the list and for each Node that
     * has a free memory block, see if it is able to merge
     * with the Node that comes after in the LinkedList.
    */
    LinkedListIterator iter;
    iter.current = get_head(list);

    while (has_next(&iter)) {

        Node* node = next(&iter);
        MemoryData* data = node->data;

        char* memory_start = data->memory_start;
        char* memory_end = memory_start + data->block_size;

        if (data->is_free == false) {

            // Only Nodes with free memory blocks can be merged
            continue;

        }

        Node* next_node = node->next;

        if (!next_node) {

            // Have reached the end of the list
            continue;

        }

        MemoryData* next_data = (MemoryData*) next_node->data;

        if (next_data->is_free == false) {

            /*
             * The next Node does not have a free memory block,
             * nothing to merge with and we can also skip
             * the next Node.
             */
            next(&iter);
            continue;

        }

        // We have a Node to merge with
        merge_meta_data_nodes(list, node, next_node);

    } // End while

    /*
     * Reset the iterator and remove empty spaces in the managed
     * heap by sliding each memory block corresponding to each
     * metadata Node down to shrink the user pool.
     */
    iter.current = get_head(list);

    char* optimal_memory_start = current_alloc->heap_start;

    while (has_next(&iter)) {

        Node* node = next(&iter);
        MemoryData* data = (MemoryData*) node->data;

        if (data->memory_start != optimal_memory_start) {

        // The memory block can be moved to a lower address
        memcpy(optimal_memory_start, data->memory_start, data->block_size);

        data->memory_start = optimal_memory_start;

        }

        /*
         * The optimal memory start of the next Node is at the end
         * of the memory block of the current Node.
        */
        optimal_memory_start += data->block_size;

    }

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
 * MemoryData
 *
 * Node
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

    Node* residual_node = create_metadata_node(
        residual_memory_start,
        residual_memory_size,
        is_free
    );

    add(current_alloc->list, residual_node);

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

            /*
             * End of list and there is no corresponding Node,
             * the Allocator has not given out this pointer.
             */
            return;

        }

        node = node->next;

    }

    MemoryData* matched_data = (MemoryData*) matched_node->data;

    // Reset the iterator and look for adjacent Nodes to merge with
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

/*
 * @brief Create a residual Node from the original input Node.
 *
 * @param1 The Node to use to create the residual Node.
 * @param2 The desired memory block size of the residual Node.
 * @return A pointer to the residual Node.
 *
 */
Node* create_residual_node(Node* node, size_t residual_size) {

    if (!node) { return NULL; }

    MemoryData* data = (MemoryData*) node->data;

    if (data->block_size <= residual_size) {

        /*
         * The size of the memory block of the residual Node has
         * to be smaller than the block size of the original Node.
         */
        return NULL;

    }

    // Need to split up such that the residual memory is in a free node
    char* residual_memory_start = data->memory_start + residual_size;
    size_t residual_block_size = data->block_size - residual_size;
    bool residual_is_free = true;

    Node* residual_node = create_metadata_node(
        residual_memory_start,
        residual_block_size,
        residual_is_free
    );

    // Remove the residual memory from the original Node
    data->block_size = residual_size;

    return residual_node;
}

void* allocator_realloc(void* ptr, size_t size) {

    if (!current_alloc || !ptr) { return NULL; }

    /*
     * Reallocating to a memory block of size 0
     * is the same as freeing the memory block.
     */
    if (size == 0) { free(ptr); return NULL; }

    LinkedList* list = current_alloc->list;
    merge_sort_list(list);

    LinkedListIterator iter;
    iter.current = list->head;

    /*
     * Have the left adjacent Node for potensially merging
     * to aquire desired new memory block size.
     */
    Node* prev_node = NULL;

    // Search for Node corresponding to argument pointer
    Node* ptr_node = NULL;
    while (has_next(&iter)) {

        prev_node = ptr_node;

        Node* node = next(&iter);
        MemoryData* data = node->data;

        if (ptr == (void*) data->memory_start) {

            // corresponding Node has been found
            ptr_node = node;

            break;

        }

    } // End while

    // Check for corresponding pointer
    if (!ptr_node) { return NULL; }

    MemoryData* ptr_data = (MemoryData*) ptr_node->data;

    if (size == ptr_data->block_size) {

        /*
         * The function call has requested a realloc
         * where the memory block size remains the same.
         */

        return ptr;

    } else if (size < ptr_data->block_size) {

        /*
         * The function call has requested a trimming
         * of the memory block.
         */

        ptr_data->block_size = size;

        // Create Node for the freed memory
        char* freed_memory_start = ptr_data->memory_start + size;
        size_t freed_block_size = ptr_data->block_size - size;
        bool freed_is_free = true;

        Node* freed_node = create_metadata_node(
            freed_memory_start,
            freed_block_size,
            freed_is_free
        );

        add(list, freed_node);

        return ptr;

    }

    /*
     * The function call has requested an extension of the
     * associated memory block.
     */

    // Check if the adjacent Nodes are free
    bool left_node_free = false;
    bool right_node_free = false;
    size_t left_node_size = 0;
    size_t right_node_size = 0;

    // Check if we can merge with the left adjacent Node
    if (prev_node) {

        MemoryData* prev_data = (MemoryData*) prev_node->data;

        if (prev_data->is_free) {

            // Merge the Nodes to have more space
            left_node_free = true;
            left_node_size = prev_data->block_size;

        }

    }

    // Check if we can merge with the right adjacent Node
    Node* next_node = ptr_node->next;
    if (next_node) {

        MemoryData* next_data = (MemoryData*) next_node->data;

        if (next_data->is_free) {

            // Merge the Nodes to have more space
            right_node_free = true;
            right_node_size = next_data->block_size;

        }

    }

    /*
     * Evaluate the adjacent Nodes and determine if
     * the merging of the memory blocks is sufficient
     * for the size as requested in the function argument.
     */
    if (
        right_node_free &&
        ptr_data->block_size + right_node_size >= size
    ) {

        // Merging with the right Node is sufficient
        Node* merged_node = merge_meta_data_nodes(list, ptr_node, next_node);
        MemoryData* merged_data = (MemoryData*) merged_node->data;

        // Need to split up such that the residual memory is in a free node
        size_t residual_block_size = merged_data->block_size - size;
        Node* residual_node = create_residual_node(merged_node, residual_block_size);

        // Remove the residual memory from the merged Node
        merged_data->block_size = size;

        add(list, residual_node);

        return merged_node;

    } else if (
        left_node_free &&
        ptr_data->block_size + left_node_size >= size
    ) {

        // Merging with the left Node is sufficient
        Node* merged_node = merge_meta_data_nodes(list, prev_node, ptr_node);
        MemoryData* merged_data = (MemoryData*) merged_node->data;

        // Need to split up such that the residual memory is in a free node
        size_t residual_block_size = merged_data->block_size - size;
        Node* residual_node = create_residual_node(merged_node, residual_block_size);

        // Remove the residual memory from the merged Node
        merged_data->block_size = size;

        add(list, residual_node);

        return merged_node;

    } else if (
        left_node_free &&
        right_node_free &&
        ptr_data->block_size + left_node_size + right_node_size >= size
    ) {

        // Merging with both the Nodes is sufficient
        Node* merged_node = merge_meta_data_nodes(list, ptr_node, next_node);
        merged_node = merge_meta_data_nodes(list, prev_node, merged_node);

        MemoryData* merged_data = (MemoryData*) merged_node->data;

        // Need to split up such that the residual memory is in a free node
        size_t residual_block_size = merged_data->block_size - size;
        Node* residual_node = create_residual_node(merged_node, residual_block_size);

        add(list, residual_node);

        return merged_node;

    } else {

        /*
         * Merging with the adjacent Nodes are either
         * not possible or not sufficient. Thus, we
         * need to look for a new location on the
         * managed heap.
         */

        void* new_location = allocator_malloc(size);

        /*
        * Copy the memory data from the original and place it
        * in the new location.
        */
        memcpy(new_location, ptr_data->memory_start, ptr_data->block_size);

        // Free the original Node as it is no longer in use
        allocator_free(ptr);

        return new_location;

    }

    // Reallocation was not possible

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

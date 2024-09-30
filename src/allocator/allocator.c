#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "allocator.h"
#include "../other_modules/memory_data.h"
#include "../linked_list/node.h"
#include "../linked_list/linked_list_iterator.h"
#include "../linked_list/merge_sort_linked_list.h"

#include <stdio.h>

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

    // Initialize Allocator object at the end of the heap
    Allocator* alloc = (Allocator*) (heap_end - sizeof(Allocator));

    // Set Allocator member variables
    alloc->heap_start = heap_start;
    alloc->heap_end = heap_end;
    alloc->reserved_pool_border =  heap_end - sizeof(Allocator);
    alloc->initial_reserved_pool_size = initial_reserved_pool_size;
    alloc->heap_size = heap_size;
    alloc->reserved_pool_size = sizeof(Allocator);

    /*
     * Set the Allocator being used to let Allocator functions
     * know which Allocator object to process.
     *
     * Also, store the current Allocator during the
     * creation of this new Allocator to be able to set it
     * back at the end of this function.
     */
    Allocator* stored_alloc = current_alloc;
    set_allocator(alloc);

    // Increase the reserved pool to accommodate for the LinkedList
    alloc->reserved_pool_border -= sizeof(LinkedList);
    alloc->reserved_pool_size += sizeof(LinkedList);

    // Initialize LinkedList object
    LinkedList* list = (LinkedList*) alloc->reserved_pool_border;

    // Set LinkedList member variables
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->next_id = 0;

    // Set Allocator member variable
    alloc->list = list;

    // Initialize a Node referencing the entire user memory pool
    void* memory_start = heap_start;

    // Increase the reserved pool to accommodate for the MemoryData
    increase_reserved_pool(sizeof(MemoryData));

    MemoryData* data = (MemoryData*) alloc->reserved_pool_border;

    /*
    * Set MemoryData member variables.
    *
    * In determining the size of the initial user pool we also account
    * for the Node which will be created and stored in the reserved pool.
    * Therefore, we subtract the size of Node in the initial user pool.
    */
    size_t block_size =
        heap_size
        - alloc->reserved_pool_size
        - sizeof(Node);
    bool is_free = true;
    data->memory_start = memory_start;
    data->block_size = block_size;
    data->is_free = is_free;
    data->in_use = true;

    // Increase the reserved pool to accommodate for the Node
    increase_reserved_pool(sizeof(Node));

    // Create Node containing the MemoryData
    Node* node = (Node*) alloc->reserved_pool_border;

    // Set Node member variables
    node->data_size = sizeof(MemoryData);
    node->next = NULL;
    node->id = 0;
    node->data = (void*) data;

    add(list, node);

    // Set the Allocator back to the one before this new Allocator
    set_allocator(stored_alloc);

    return alloc;
}

char* retrieve_user_pool_border() {

    if (!current_alloc) { return NULL; }

    LinkedList* list = current_alloc->list;
    merge_sort_list(list);

    Node* found_node = NULL;
    LinkedListIterator iter;
    iter.current = list->head;

    while (has_next(&iter)) {

        Node* node = next(&iter);
        MemoryData* data = (MemoryData*) node->data;

        if (data->is_free == false) {

            found_node = node;

        }

    }

    if (!found_node) {

        /*
         * No free Node was found. This means that
         * the heap is completely free.
         */
        return NULL;

    }

    MemoryData* found_data = (MemoryData*) found_node->data;

    // Retrieve the memory pool borders
    char* user_border = found_data->memory_start + found_data->block_size;

    return user_border;

}


/*
 * @brief Determines if the user pool and reserved pool will
 * overlap if we increase one of the borders by 'increase'.
 *
 * @note Since the pools are dual of each other, it does not
 * matter if we choose to increase the user pool or the
 * reserved pool. If an overlap happens, it will happen
 * regardless of the pool we choose to increase.
 *
 * @param The amount in bytes that we want to increase a pool.
 * @return Whether the pools overlap or not.
 */
bool pool_overlap(size_t increase) {

    if (!current_alloc) { return false; }

    LinkedList* list = current_alloc->list;

    if (list->size == 0) {

        /*
         * The list is empty, thus there can be no overlap.
         */

        return false;

    }

    // Retrieve the memory pool borders
    char* user_border = retrieve_user_pool_border();
    char* reserved_border = current_alloc->reserved_pool_border;

    /*
     * Check if the pools overlap if we increase one of the pools.
     * Note that the choice of pool to increase is arbitrary as
     * they are the dual of each other.
     */
    if (user_border + increase > reserved_border) {

        /*
         * The pool borders have reached each other.
         * Attempt to reduce memory fragmentation in each pool.
         */
        cleanse_user_pool();
        cleanse_reserved_pool();

        // Retrieve the cleansed memory pool borders
        user_border = retrieve_user_pool_border();
        reserved_border = current_alloc->reserved_pool_border;

        // Check if pool cleansing prevents pool overlap
        if (user_border + increase > reserved_border) {

            // Cleaning up the pools did not work, heap is considered full
            return true;

        }

    }

    return false;

}

void increase_reserved_pool(size_t increase) {

    if (current_alloc == NULL) {

        // There is no Allocator to operate on
        return;

    }

    if (pool_overlap(increase) == true) {

        // There is an overlap, the heap is considered full
        printf("there is a pool overlap\n");
        fflush(stdout);
        return;

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

    // Increase the reserved pool to accommodate for the MemoryData
    increase_reserved_pool(sizeof(MemoryData));

    MemoryData* data = (MemoryData*) current_alloc->reserved_pool_border;

    // Set MemoryData member variables
    data->memory_start = memory_start;
    data->block_size = block_size;
    data->is_free = is_free;
    data->in_use = true;

    // Increase the reserved pool to accommodate for the Node
    increase_reserved_pool(sizeof(Node));

    // Create Node containing the MemoryData
    Node* node = (Node*) current_alloc->reserved_pool_border;

    // Set Node member variables
    node->data_size = sizeof(MemoryData);
    node->next = NULL;
    node->id = 0;
    node->data = (void*) data;

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

        printf("Node is not left adjacent\n");
        fflush(stdout);
        return NULL;

    }

    // merge the block sizes
    left_data->block_size += right_data->block_size;

    printf("-----new block_size: %zu\n", left_data->block_size);

    // Mark the right Node as vacant
    right_data->in_use = false;
    right_data->is_free = true;

    /*
     * Remove the discarded Node from the LinkedList.
     * Note that when using drop_node(), the Node is
     * not freed, only dropped from the list.
     */
    size_t id = right_node->id;
    drop_node(list, id);

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

        /*
         * With the Nodes merged, we have to check if the
         * next Node of the merged Node is free and can
         * be merged with.
         */

        next_node = node->next;
        next_data = (MemoryData*) next_node->data;

        while (next_node && next_data->is_free) {

            // The next Node is free as well, we can merge
            merge_meta_data_nodes(list, node, next_node);

            next_node = node->next;
            next_data = (MemoryData*) next_node->data;

        }

    } // End while

    /*
     * Reset the iterator and move free memory blocks to a
     * higher memory addresses and move the non-free memory
     * blocks to lower memory addresses such that there is
     * no memory fragmentation.
     *
     * A 'snowball effect' algorithm is used.
     * We go through the list and mark the first free Node
     * as the snowball. Since the list have already merged
     * all free adjacent Nodes, the next Node must be
     * non-free (if not, there is no fragmentation and we
     * are done).
     *
     * We swap the memory block of the snowball
     * Node with the non-free Node. By swapping, we move
     * the allocated data from the non-free Node to the
     * free Node. Then, we update the 'block_size' and
     * 'is_free' member variables of each Node.
     *
     * We then check if the snowball Node can merge with the
     * right-adjacent Node (add to the snowball).
     *
     * Thereafter, we swap the snowball Node with the right-adjacent
     * Node which must be non-free (if not, we are done).
     *
     * Eventually, the free Node will have floated to the top of
     * the managed heap with all the free memory.
     */
    iter.current = get_head(list);

    // Locate the first free Node (snowball Node)
    Node* snowball_node = NULL;
    while (has_next(&iter)) {

        Node* node = next(&iter);
        MemoryData* data = (MemoryData*) node->data;

        if (data->is_free) {

            snowball_node = node;
            break;

        }

    }

    if (!snowball_node) {

        // There is no free Node, the heap must be full
        return;

    }

    /*
     * Perform the Node swapping and float the snowball to higher memory.
     */
    while (has_next(&iter)) {

        Node* node = next(&iter);
        MemoryData* data = (MemoryData*) node->data;

        if (data->is_free) {

            printf("The next Node is free, this is not supposed to be\n");
            return;

        }

        MemoryData* snowball_data = (MemoryData*) snowball_node->data;

        /*
         * Temporarily store the data on the stack, then perform
         * the swapping of the Nodes.
         */
        size_t snowball_block_size = snowball_data->block_size;

        // Move the non-free Node to the memory start of the snowball Node
        snowball_data->block_size = data->block_size;
        snowball_data->is_free = false;
        memcpy(
            snowball_data->memory_start,
            data->memory_start,
            data->block_size
        );

        /*
         * Swap the content of the Nodes, but keep the
         * Nodes and their reference in the list the same.
         */

        // Move the snowball (don't care about allocated data since its free)
        data->block_size = snowball_block_size;
        data->is_free = true;

        // Update where the snowball Node is
        snowball_node = node;

        // See if the snowball Node can merged with the right-adjacent Node
        Node* merge_node = snowball_node->next;
        if (merge_node) {

            MemoryData* merge_data = (MemoryData*) merge_node->data;
            if (merge_data->is_free) {

                // It is free, thus we merge
                merge_meta_data_nodes(list, snowball_node, merge_node);

            }

        }

    } // End while

    //merge_sort_list(list);

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

    /*
     * Memory location for the start of metadata Node traversal.
     * Note that during Allocator creation, a metadata Node is
     * created to accommodate for the memory block of the whole
     * user pool. Thus, the first metadata Node is located at the
     * initial reserved pool border during creation.
     */
    char* meta_data_node =
        current_alloc->heap_end
        - current_alloc->initial_reserved_pool_size;

    // Iterate through the metadata Nodes
    while (meta_data_node >= current_alloc->reserved_pool_border) {

        // Retrieve Node's corresponding MemoryData
        MemoryData* data = (MemoryData*) (meta_data_node + sizeof(Node));

        /*
         * Check if we have found a Node that is not
         * in use by the LinkedList.
         */
        if (data->in_use == false) {

            /*
             * Search for a border Node that is in use by the
             * LinkedList. If it is not in use, then we can
             * ignore it and move the border to the next
             * metadata Node that is in use.
             */

            Node* border_node = (Node*) current_alloc->reserved_pool_border;
            MemoryData* border_data = (MemoryData*) border_node->data;

            while (!border_data->in_use) {

                // Shift the reserved pool border by one metadata Node
                current_alloc->reserved_pool_border += meta_data_node_size;
                current_alloc->reserved_pool_size -= meta_data_node_size;

                // Update for next iteration
                border_node = (Node*) current_alloc->reserved_pool_border;
                border_data = (MemoryData*) border_node->data;

            }

            if ((char*) border_node == meta_data_node) {

                // No unique Node was found
                return;

            }

            // Move the border metadata Node to the vacant space
            memcpy(meta_data_node, border_node, meta_data_node_size);

            // Shift the border now that the border Node has been moved
            current_alloc->reserved_pool_border += meta_data_node_size;
            current_alloc->reserved_pool_size -= meta_data_node_size;

        }

        // Update traversal data
        meta_data_node -= meta_data_node_size;

    }

}

/*
 * @brief Create a residual Node from the original input Node.
 * and update the original Node by discarding the memory block
 * now given to the residual Node.
 *
 * @param1 The Node to use to create the residual Node.
 * @param2 The desired memory block size of the residual Node.
 * @return A pointer to the residual Node.
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

    // Need to split up such that the residual memory is in a free Node
    char* residual_memory_start =
        data->memory_start
        + data->block_size
        - residual_size;
    bool residual_is_free = true;

    Node* residual_node = create_metadata_node(
        residual_memory_start,
        residual_size,
        residual_is_free
    );

    // Change the original block size to carry the allocated memory block
    data->block_size -= residual_size;

    return residual_node;
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

    // Construct the residual metadata Node
    size_t residual_memory_size = node_block_size - required_size;
    Node* residual_node = create_residual_node(available_node, residual_memory_size);

    add(current_alloc->list, residual_node);

    // Modify 'available_node' to reflect that it is now in use
    data->is_free = false;

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

    printf("----- allocator_free has been called!\n");
    fflush(stdout);

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
    Node* matched_node = NULL;
    while (has_next(&iter)) {

        Node* node = next(&iter);
        MemoryData* data = (MemoryData*) node->data;

        if (data->memory_start == ptr && !data->is_free) {

            // Node has been found
            matched_node = node;
            break;

        }

        node = node->next;

    }

    if (!matched_node) {

        printf("---- No Node was found!\n");
        fflush(stdout);

       /*
        * The corresponding Node was not found.
        * the Allocator has not given out this pointer.
        */
        return;

    } else {

        printf("----NODE ID %zu\n", matched_node->id);
        fflush(stdout);

    }

    MemoryData* matched_data = (MemoryData*) matched_node->data;
    char* matched_memory_start = matched_data->memory_start;
    char* matched_memory_end = matched_memory_start + matched_data->block_size;

    // Mark the memory block as free
    matched_data->is_free = true;

    /*
     * Reset the iterator and see if the newly freed Node
     * has adjacent free Nodes that it can merge with.
     */
    iter.current = get_head(list);

    while (has_next(&iter)) {

        Node* node = next(&iter);

        // This is the same Node
        if (node->id == matched_node->id) { continue; }

        MemoryData* data = node->data;
        char* memory_start = data->memory_start;
        char* memory_end = memory_start + data->block_size;

        // Check if there is a right adjacent Node to merge with
        if (memory_start == matched_memory_end && data->is_free) {

            merge_meta_data_nodes(list, matched_node, node);

            // Update the block size ends
            matched_memory_start = matched_data->memory_start;
            matched_memory_end = matched_memory_start + matched_data->block_size;

        }

        // Check if there is a left adjacent Node to merge with
        if (memory_end == matched_memory_start && data->is_free) {

            merge_meta_data_nodes(list, node, matched_node);
            /*
             * Since merging results in the left Node remaining,
             * 'matched_node' will be discarded from the list.
             * Therefore, update to keep track of the matched
             * Node.
             */
            matched_node = node;

            // Update the block size ends
            matched_memory_start = matched_data->memory_start;
            matched_memory_end = matched_memory_start + matched_data->block_size;

        }

    }

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
     * to aquire desired new memory block size later in the
     * function.
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

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "linked_list.h"
#include "node.h"
#include "linked_list_iterator.h"
#include "../other_modules/constants.h"
#include "../allocator/allocator.h"

LinkedList* create_list() {

    LinkedList* list = (LinkedList*) allocator_malloc(sizeof(LinkedList));
    if (list == NULL) {

        // Allocation failed
        return NULL;

    }

    // Initialize member variables
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->next_id = 0;

    return list;
}

LinkedList* add(LinkedList* list, Node* node) {

    if (list == NULL || node == NULL) {

        // There is nothing to operate on
        return NULL;

    }

    // Logic for inserting into an empty list
    if (list->head == NULL) {

        list->head = node;
        list->tail = node;

        node->next = NULL;

        list->size++;
        node->id = list->next_id++;

    } else { // Logic for inserting into a non-empty list

        Node* tail_node = list->tail;

        // Overwrite tail node
        list->tail = node;

        // Insert new node
        tail_node->next = node;
        node->next = NULL;

        list->size++;
        node->id = list->next_id++;

    }

    return list;

}

LinkedList* delete_node(LinkedList* list, size_t id) {

    // Check: list exists, non-empty, id within range
    if (list == NULL || list->size == 0 || list->size < id) {

        return NULL;

    }

    // Create an iterator for the list
    LinkedListIterator* iter = create_iterator(list);

    // Retrieve Nodes for base case
    Node* prev = next(iter);
    if (prev == NULL) {

        destroy_iterator(iter);

        return NULL;

    }
    Node* current = next(iter);

    // Check if head corresponds with 'id'
    if (prev->id == id) {

        // Nominate new head
        list->head = current;
        list->size--;

        // Update tail if this was the only Node
        if (list->size == 0) {

            list->tail = NULL;

        }

        // Free the Node from memory
        destroy_node(prev);
        destroy_iterator(iter);

        return list;

    }

    // Search remaining part of list
    while(current != NULL) {

        // Check if it is the Node to be removed
        if (current->id == id) {

            // Remove target from reference chain
            prev->next = current->next;

            // Check if prev is the new tail
            if (current->next == NULL) {

                list->tail = prev;

            }

            // Free the Node from memory
            destroy_node(current);
            list->size--;

            break;

        }

        // Iterate to the next Node
        prev = current;
        current = next(iter);

    }

    destroy_iterator(iter);

    return list;

}

size_t search_by_value(LinkedList* list, void* data, size_t data_size) {

    if (data == NULL || data_size == 0) {

        return NOT_FOUND;

    }

    // Go through each Node and check if it matches
    LinkedListIterator* iter = create_iterator(list);
    while(has_next(iter)) {

        Node* node = next(iter);
        if (node == NULL) {

            destroy_iterator(iter);
            return NOT_FOUND;

        }

        // Compare the argument data to that of the Node
        if (data_size == node->data_size && memcmp(data, node->data, data_size) == 0) {

            destroy_iterator(iter);
            return node->id;

        }

    }

    destroy_iterator(iter);

    return NOT_FOUND;

}

Node* get_head(LinkedList* list) {

    return list->head;

}

Node* get_tail(LinkedList* list) {

    return list->tail;

}

void destroy_list(LinkedList* list) {

    if (list == NULL) {

        return;

    }

    // Loop through the list and free each Node
    LinkedListIterator* iter = create_iterator(list);
    while (has_next(iter)) {

        // Free the Node
        Node* node = next(iter);
        destroy_node(node);

    }

    destroy_iterator(iter);

    // With all the node set free, it is safe to free list struct
    allocator_free(list);

}

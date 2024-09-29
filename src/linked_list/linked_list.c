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

    LinkedList* list = (LinkedList*) malloc(sizeof(LinkedList));
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

        // Update next Node references
        tail_node->next = node;
        node->next = NULL;

        list->size++;
        node->id = list->next_id++;

    }

    return list;

}

LinkedList* delete_node(LinkedList* list, size_t id) {

    Node* dropped_node = drop_node(list, id);

    if (!dropped_node) { return NULL; }

    free(dropped_node);

    return list;

}

Node* drop_node(LinkedList* list, size_t id) {

    if (!list || list->head == NULL) { return NULL; }

    /*
    * Check special conditions when the
    * drop Node is the head.
    */
    if (list->size == 1 && list->head->id == id) {

        // There is only one Node
        Node* dropped_node = list->head;
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;

        return dropped_node;

    } else if (list->head->id == id) {

        // The head is the Node
        Node* dropped_node = list->head;
        list->head = list->head->next;
        list->size -= 1;

        return dropped_node;

    }

    LinkedListIterator iter;
    iter.current = list->head;

    // Drop the head as it has already been inspected
    Node* prev_node = next(&iter);
    Node* node = NULL;
    while (has_next(&iter)) {

        node = next(&iter);

        if (node->id == id) {

            /*
            * The Node has been found!
            * Handle special case if it is the tail.
            */
            if (node == list->tail) {

                list->tail = prev_node;
                prev_node->next = NULL;

            } else {

                prev_node->next = node->next;

            }

            list->size -= 1;

            return node;

        }

        prev_node = node;

    } // End while

    // No Node was found
    return NULL;

}

size_t search_by_value(LinkedList* list, void* data, size_t data_size) {

    if (data == NULL || data_size == 0) {

        return NOT_FOUND;

    }

    // Go through each Node and check if it matches
    LinkedListIterator iter;
    iter.current = list->head;

    while (has_next(&iter)) {

        Node* node = next(&iter);
        if (node == NULL) {

            return NOT_FOUND;

        }

        // Compare the argument data to that of the Node
        if (data_size == node->data_size && memcmp(data, node->data, data_size) == 0) {

            return node->id;

        }

    }

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
    LinkedListIterator iter;
    iter.current = list->head;

    while (has_next(&iter)) {

        // Free the Node
        Node* node = next(&iter);
        destroy_node(node);

    }

    // With all the node set free, it is safe to free list struct
    free(list);

}

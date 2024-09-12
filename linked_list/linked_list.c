#include <stddef.h>
#include <stdlib.h>
#include "linked_list.h"
#include "node.h"
#include "../iterator/linked_list_iterator.h"

LinkedList* create_list() {

    LinkedList* list = (LinkedList*) malloc(sizeof(LinkedList));

    if (list == NULL) {

        return NULL;

    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

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
        node->id = list->size;

    } else { // Logic for inserting into a non-empty list

        Node* tail_node = list->tail;

        // Overwrite tail node
        list->tail = node;

        // Insert new node
        tail_node->next = node;
        node->next = NULL;

        list->size++;
        node->id = list->size;

    }

    return list;

}

LinkedList* delete_node(LinkedList* list, size_t id) {

    // Check: list exists, non-empty, id within range
    if (list == NULL || list->size == 0 || list->size < id) {

        return NULL;

    }

    // Create an iterator for the list
    LinkedListIterator* iterator = create_iterator(list);

    Node* prev = iterator->current;
    Node* current = iterate(iterator);

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
        free(prev);
        free(iterator);

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
            free(current);
            list->size--;

            break;

        }

        // Iterate to the next Node
        prev = current;
        current = iterate(iterator);

    }

    free(iterator);

    return list;

}

#include <stddef.h>
#include <stdlib.h>
#include "linked_list.h"
#include "node.h"

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

    if (list->head == NULL) {

        list->head = node;
        list->tail = node;

        node->next = NULL;
        node->id = list->size;

        list->size++;

    } else {

        Node* tail_node = list->tail;

        // Overwrite tail node
        list->tail = node;

        // Insert new node
        tail_node->next = node;
        node->next = NULL;
        node->id = list->size;

        list->size++;

    }

}

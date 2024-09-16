#include <stdlib.h>
#include <stddef.h>
#include "linked_list_iterator.h"

LinkedListIterator* create_iterator(LinkedList* list) {

    // Check if list exists
    if (list == NULL) {

        return NULL;

    }

    // Allocate iterator on the heap
    LinkedListIterator* iter = (LinkedListIterator*) malloc(sizeof(LinkedListIterator));
    if (iter == NULL) {

        // Allocation failed
        return NULL;

    }

    // Set start of iteration to head Node
    iter->current = get_head(list);

    return iter;

}

Node* next(LinkedListIterator* iter) {

    // Check if there is an iterator and a Node
    if (iter == NULL || iter->current == NULL) {

        return NULL;

    }

    Node* current = iter->current;

    // Update the iterator to point to next Node
    iter->current = current->next;

    return current;

}

bool has_next(LinkedListIterator* iter) {

    // Handle if iterator exists
    if (iter == NULL) {

        return false;

    }

    // Check if there is a Node to retrieve given a 'next' call
    Node* current = iter->current;
    if (current == NULL) {

        return false;

    }

    return true;

}

void destroy_iterator(LinkedListIterator* iter) {

    if (iter == NULL) {

        return;

    }

    free(iter);

}

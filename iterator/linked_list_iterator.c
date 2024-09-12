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
    iter->current = list->head;

    return iter;

}

Node* next(LinkedListIterator* iter) {

    // Check if there is an iterator and a Node
    if (iter == NULL || iter->current == NULL) {

        return NULL;

    }

    Node* current = iter->current;

    // Update the Node in the iterator to point to next
    iter->current = current->next;

    return current;

}

bool has_next(LinkedListIterator* iterator) {

    // Handle if iterator exists
    if (iterator == NULL) {

        return false;

    }

    // Check if there is a next Node
    Node* current = iterator->current;
    if (current == NULL || current->next == NULL) {

        return false;

    }

    return true;

}

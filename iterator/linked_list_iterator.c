#include <stdlib.h>
#include <stddef.h>
#include "linked_list_iterator.h"

LinkedListIterator* create_iterator(LinkedList* list) {

    // Check if list exists or is empty
    if (list == NULL || list->size == 0) {

        return NULL;

    }

    // Allocate iterator on the heap
    LinkedListIterator* iter = (LinkedListIterator*) malloc(sizeof(LinkedListIterator));

    // Set start of iteration to head Node
    iter->current = list->head;

    return iter;

}

Node* iterate(LinkedListIterator* iter) {

    // Check if there is an iterator and a Node
    if (iter == NULL || iter->current == NULL) {

        return NULL;

    }

    Node* current = iter->current;

    // Update the Node in the iterator to point to next
    iter->current = current->next;

    return current;

}

#include <stdlib.h>
#include <stddef.h>
#include "linked_list_iterator.h"

LinkedListIterator* create_iterator(LinkedList* list) {

    if (list == NULL || list->size == 0) {

        return NULL;

    }

    // Allocate iterator on the heap
    LinkedListIterator* iter = (LinkedListIterator*) malloc(sizeof(LinkedListIterator));




    return NULL;
}

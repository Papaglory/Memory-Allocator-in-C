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

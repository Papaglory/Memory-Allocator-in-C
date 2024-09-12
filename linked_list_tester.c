#include <stddef.h>
#include <stdlib.h>
#include "linked_list/linked_list.h"
#include "linked_list/node.h"
#include "iterator/linked_list_iterator.h"

int main() {

    // Create an empty list
    LinkedList* list = create_list();

    // Create the nodes for the list
    Node* n1 = NULL;

    // Create a corresponding iterator
    LinkedListIterator* iterator = create_iterator(list);

    // Free from memory
    free(iterator);
    free(list);

}

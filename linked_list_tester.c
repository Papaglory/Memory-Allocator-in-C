#include <stddef.h>
#include <stdlib.h>
#include "linked_list/linked_list.h"
#include "linked_list/node.h"
#include "iterator/linked_list_iterator.h"

int main() {

    // Create an empty list
    LinkedList* list = create_list();

    // Create the nodes for the list
    int data_1 = 42;
    create_node(&data_1, sizeof(data_1));

    int data_2 = 11;
    create_node(&data_2, sizeof(data_2));

    // Create a corresponding iterator
    LinkedListIterator* iterator = create_iterator(list);

    // Free from memory
    free(iterator);
    free(list);

}

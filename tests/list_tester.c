#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "../src/linked_list/linked_list.h"
#include "../src/linked_list/node.h"
#include "../src/linked_list/linked_list_iterator.h"

int main() {

    printf("%s", "----TEST STARTED----\n");

    // Create an empty list
    LinkedList* list = create_list();

    // Create the nodes for the list
    int data_1 = 42;
    Node* node_1 = create_node(&data_1, sizeof(data_1));
    add(list, node_1);

    int data_2 = 11;
    Node* node_2 = create_node(&data_2, sizeof(data_2));
    add(list, node_2);

    int data_3 = 99;
    Node* node_3 = create_node(&data_3, sizeof(data_3));
    add(list, node_3);

    // Create a corresponding iterator
    LinkedListIterator* iter = create_iterator(list);

    printf("\n%s\n", "Searching for node_2 by value");
    int found_id = search_by_value(list, &data_2, sizeof(data_2));

    printf("Node ID found: %d", found_id);

    printf("\n%s\n", "Trying to remove a node from the list");

    // Delete the first Node in the list
    delete_node(list, 1);
    node_1 = NULL;

    printf("%s\n", "Node removed");

    // Free from memory
    destroy_iterator(iter);
    destroy_list(list);

    printf("%s", "----TEST ENDED----\n");

}

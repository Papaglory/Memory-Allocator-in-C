#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "../src/linked_list/linked_list.h"
#include "../src/linked_list/node.h"
#include "../src/linked_list/linked_list_iterator.h"

int main() {

    printf("%s", "----TEST STARTED----\n");

    // Create an empty list
    LinkedList list;
    list.head = NULL;
    list.tail = NULL;
    list.size = 0;
    list.next_id = 0;

    // Create the nodes for the list
    int data_1 = 42;
    Node* node_1 = create_node(&data_1, sizeof(data_1));
    add(&list, node_1);

    int data_2 = 11;
    Node* node_2 = create_node(&data_2, sizeof(data_2));
    add(&list, node_2);

    int data_3 = 99;
    Node* node_3 = create_node(&data_3, sizeof(data_3));
    add(&list, node_3);

    int data_4 = 101;
    Node* node_4 = create_node(&data_4, sizeof(data_4));
    add(&list, node_4);

    printf("\n%s\n", "Searching for node_2 by value");
    int found_id = search_by_value(&list, &data_2, sizeof(data_2));

    printf("Node ID found: %d", found_id);

    printf("\n%s\n", "Trying to remove a node from the list");

    printf("Tail Node: %zu\n", list.tail->id);

    //delete_node(&list, 2);
    //printf("%s\n", "Node removed");
    Node* dropped_node = drop_node(&list, 2);

    int* dropped_value = (int*) dropped_node->data;

    printf("Dropped value is: %d\n", *dropped_value);

    printf("%s\n", "Printing the content of the Nodes");

    // Create a corresponding iterator
    LinkedListIterator iter;
    iter.current = list.head;

    while (has_next(&iter)) {

        Node* node = next(&iter);

        printf("Node ID: %zu\n", node->id);

        if (node->data_size == sizeof(int)) {

            int* value = (int*) node->data;

            printf("Node value: %d\n", *value);

        }

    }

    printf("%s", "----TEST ENDED----\n");

}

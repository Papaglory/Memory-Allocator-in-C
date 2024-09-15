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

    // Create a corresponding iterator
    LinkedListIterator* iterator = create_iterator(list);

    // Some tests for the linked list
    Node* head = get_head(list);
    Node* tail = get_tail(list);

    printf("Head ID: %zu\n", head->id);
    printf("tail ID: %zu\n", tail->id);
    printf("Linked list size: %zu\n", list->size);

    // Testing each of the nodes in the list along with the iterator
    Node* node = NULL;
    while(has_next(iterator)) {

        // Retrieve the next Node
        node = next(iterator);

        // Payload of the Node
        void* data = node->data;
        size_t data_size = node->data_size;

        // Process the data if it is an integer
        if (data_size == sizeof(int)) {

            // Cast void pointer to integer pointer
            int* int_data = (int*) data;
            printf("Value: %d\n", *int_data);
            printf("ID: %zu\n", node->id);

        }

    }

    // Free from memory
    free(iterator);
    destroy_list(list);

    printf("%s", "----TEST ENDED----\n");

}

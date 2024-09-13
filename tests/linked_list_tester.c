#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "../src/linked_list/linked_list.h"
#include "../src/linked_list/node.h"
#include "../src/linked_list/linked_list_iterator.h"

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

    Node* node = NULL;
    while(has_next(iterator)) {

        // Retrieve the next Node
        node = next(iterator);

        // Payload of the Node
        void* data = node->data;
        size_t data_size = node->data_size;

        // Process the data if it is an integer
        if (node->data_size == sizeof(int)) {

            // Cast void pointer to integer pointer
            int* int_data = (int*) data;
            printf("%d", *int_data);

        }

    }

    // Free from memory
    free(iterator);
    free(list);

}

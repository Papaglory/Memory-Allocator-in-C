#include <string.h>
#include <stdlib.h>
#include "node.h"

Node* create_node(void* data, size_t data_size) {

    // Allocate space for the Node
    Node* node = (Node*) malloc(sizeof(Node));
    if (node == NULL) {

        // Memory allocation failed
        return NULL;

    }

    // Allocate memory corresponding to this pointer
    node->data = malloc(data_size);
    if (node->data == NULL){

        // Memory allocation failed
        free(node); // Free in case of failure

        return NULL;

    }

    // Copy the data pointed by 'data' into location
    // pointed to by 'node->data'
    memcpy(node->data, data, data_size);

    // Initialize the other members
    node->data_size = data_size;
    node->next = NULL;
    node->id = 0;

    return node;

}

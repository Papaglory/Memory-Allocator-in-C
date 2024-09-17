#include <string.h>
#include <stdlib.h>
#include "node.h"
#include "../allocator/allocator.h"

Node* create_node(void* data, size_t data_size) {

    // Allocate space for the Node
    Node* node = (Node*) allocator_malloc(sizeof(Node));
    if (node == NULL) {

        // Memory allocation failed
        return NULL;

    }

    // Allocate memory corresponding to this pointer
    node->data = allocator_malloc(data_size);
    if (node->data == NULL){

        // Memory allocation failed
        allocator_free(node); // Free in case of failure

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

void destroy_node(Node* node) {

    if (node == NULL) {

        return;

    }

    // Free memory corresponding to the data the node carries
    if (node->data != NULL) {

        allocator_free(node->data);
        node->data = NULL;

    }

    // Free the node itself
    allocator_free(node);

}

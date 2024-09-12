#include <stdlib.h>
#include "node.h"

Node* create_node(void* data, size_t data_size) {

    // Allocate space for the node
    Node* node = (Node*) malloc(sizeof(Node));

    // Insert the data into the Node
    node->data = data;
    node->data_size = data_size;

    return node;

}

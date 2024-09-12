#ifndef NODE_H
#define NODE_H

#include <stddef.h>

typedef struct Node {

    // Payload of the node
    void* data;
    size_t data_size;

    // reference to the next node
    Node* next;

    // ID of the node. Starts at 1
    size_t id;

} Node;

#endif // NODE_H

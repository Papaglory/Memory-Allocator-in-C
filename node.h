#ifndef NODE_H
#define NODE_H

#include <stddef.h>
#include <tuple.h>

// Forward declaration of Node
typedef struct Node Node;

typedef struct {

    // Payload of the node
    void* data;
    size_t data_size;

    // reference to the next node
    Node* next;

    // ID of the node
    size_t id;

} Node

#endif // NODE_H

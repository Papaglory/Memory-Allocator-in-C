#ifndef NODE_H
#define NODE_H

#include <stddef.h>

typedef struct Node {

    // Payload of the Node
    void* data;
    size_t data_size;

    // reference to the next Node. Used by the linked list
    struct Node* next;

    // ID of the node used by the linked list. Starts at 1
    size_t id;

} Node;

/*
* @brief Create a Node with data inside it corresponding
* to the function input.
*
* @param1 A pointer pointing to the data to be contained
* within the Node.
* @param2 Metadata informs how much data corresponds to
* the pointer.
* @return The Node constructed.
*/
Node* create_node(void* data, size_t data_size);

#endif // NODE_H

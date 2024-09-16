#ifndef NODE_H
#define NODE_H

#include <stddef.h>

typedef struct Node {

    // Payload of the Node
    void* data;
    size_t data_size;

    // reference to the next Node. Used by the linked list
    struct Node* next;

    /*
    * ID of the node used by the linked list. Starts at 1.
    * When a Node is inserted into a LinkedList, the list
    * will assign an ID to the Node. The ID is unique.
    * The list will not recycle old IDs, but keep
    * incrementing the ID values it assigns.
    */
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

/*
* @brief Destroy and free the memory corresponding to this
* Node.
* Careful to use this if the Node is part of a LinkedList.
* Consider using delete_node().
*
* @param The Node to be destroyed.
*/
void destroy_node(Node* node);

#endif // NODE_H

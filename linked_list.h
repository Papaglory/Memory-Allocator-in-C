#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

// Forward declaration used to reference Node
// without having to include the whole header file.
typedef struct Node Node;

typedef struct {

    // Pointer to first node
    Node* head;

    // Pointer to final node
    Node* tail;

    // Size of the list
    size_t size;

} LinkedList;

LinkedList* create_list();

/*
* @brief Add a node at the end of the list.
*
* @param1 The linked list to append the node.
* @param2 The node to be appended.
* @return Return pointer to the list.
*/
LinkedList* add(LinkedList* list, Node* node);

LinkedList* delete_node(LinkedList* list, size_t id);

size_t search_by_value(void* data, size_t data_size);

Node* get_head(LinkedList* list);

Node* get_tail(LinkedList* list);

#endif // LINKED_LIST_H

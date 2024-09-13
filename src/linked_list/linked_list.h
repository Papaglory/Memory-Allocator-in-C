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

/*
* @brief Create an empty list.
*
* @return Return pointer pointing to the list.
*/
LinkedList* create_list();

/*
* @brief Add a node at the end of the list.
*
* @param1 The linked list to append the node.
* @param2 The node to be appended.
* @return Return pointer to the list.
*/
LinkedList* add(LinkedList* list, Node* node);

/*
* @brief Delete a node from the list with ID
* corresponding to 'id'.
*
* @param1 The linked list.
* @param2 The ID corresponding to the Node to
* be deleted.
* @return Returns a pointer to the linked list.
*/
LinkedList* delete_node(LinkedList* list, size_t id);

size_t search_by_value(void* data, size_t data_size);

/*
* @brief Retrieve the first Node of the list.
*
* @param1 The linked list.
* @return The first node in the list.
*/
Node* get_head(LinkedList* list);

/*
* @brief Retrieve the last Node of the list.
*
* @param1 The linked list.
* @return The last node in the list.
*/
Node* get_tail(LinkedList* list);

#endif // LINKED_LIST_H

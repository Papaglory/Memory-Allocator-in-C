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

    /*
    * Each Node in the list will have a unique ID.
    * IDs are not recycled, meaning that the ID of
    * a node that has been removed will not be given
    * to a new Node added to the list.
    */
    size_t next_id;

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
* @note This will also free the Node from the heap.
*
* @param1 The linked list.
* @param2 The ID corresponding to the Node to
* be deleted.
* @return Returns a pointer to the LinkedList.
*/
LinkedList* delete_node(LinkedList* list, size_t id);

/*
* @brief Drop a node from the list with ID
* corresponding to 'id'.
*
* $note This will not free the Node from the heap,
* only drop it from the LinkedList. Use with caution.
*
* @param1 The linked list.
* @param2 The ID corresponding to the Node to
* be deleted.
* @return Returns a pointer to the LinkedList.
*/
LinkedList* drop_node(LinkedList* list, size_t id) {

/*
* @brief Search by value in the linked list. The function
* looks through the nodes and find the first Node
* where the value is equal.
*
* @param1 The value to compare each Node with.
* @param2 The size of the data we are looking for.
* @return The ID corresponding to the node.
*/
size_t search_by_value(LinkedList* list, void* data, size_t data_size);

/*
* @brief Retrieve the first Node of the list.
*
* @param The linked list.
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

/*
* @brief Destroy the list.
*
* @param The linked list.
*/
void destroy_list(LinkedList* list);

#endif // LINKED_LIST_H

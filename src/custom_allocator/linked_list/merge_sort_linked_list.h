#ifndef MERGE_SORT_LINKED_LIST_H
#define MERGE_SORT_LINKED_LIST_H

#include "linked_list.h"
#include "node.h"

/*
* @brief Split the Node chain into two halves using
* the slow and fast pointer technique.
*
* @param The start of the list (sub-list).
* @return Return the pointer to the second half of
* the list (same as the slow pointer).
*/
Node* split_list(Node* head);

/*
* @brief Merge two sorted lists. The merging is based
* on the 'memory_start' member variable of the MemoryData
* object which is the payload of the Node.
*
* @param1 The head Node of the first list.
* @param2 The head Node of the second list.
* @return The head Node of the merged list.
* */
Node* merge(Node* left, Node* right);


Node* merge_sort(Node* head);

#endif // MERGE_SORT_LINKED_LIST_H

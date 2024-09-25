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
*/
Node* merge(Node* left, Node* right);

/*
* @brief Perform the merge sort algorithm on the next
* reference chain strating from Node head.
*
* @param The head Node of the corresponding LinkedList
* to be sorted.
* @return The head Node of the sorted next reference chain.
*/
Node* merge_sort(Node* head);

/*
* @brief Given a LinkedList, perform the merge sort algorithm,
*
* @param The LinkedList to be sorted.
* @return A pointer to the sorted LinkedList (same as argument input).
*/
LinkedList* merge_sort_list(LinkedList* list);

#endif // MERGE_SORT_LINKED_LIST_H

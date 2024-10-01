/*
 * @file linked_list_iterator.h
 * @brief Iterator to traverse a LinkedList.
 *
 * @note When modifying the list (e.g., merging or deleting nodes),
 * you should manually manage the position of the iterator to avoid
 * it getting out of sync with the list structure. After merging nodes,
 * the iterator's `current` node should be manually updated to the
 * appropriate position (typically the node after the merged nodes).
 *
 * TLDR, the next() function stores the current Node, then updates
 * the 'current' member variable, then returns the stored Node.
 * Thus, the update happens immediately when calling next().
 */

#ifndef LINKEDLISTITERATOR_H
#define LINKEDLISTITERATOR_H

#include <stdbool.h>
#include "linked_list.h"
#include "node.h"

typedef struct {

    Node* current;

} LinkedListIterator;

/*
* @brief Create an iterator corresponding to the list.
*
* @param The list to create an iterator for.
* @return The iterator used to iterate the list.
*/
LinkedListIterator* create_iterator(LinkedList* list);

/*
* @brief Retrieve the next Node in the list according
* to the iterator.
*
* @param The iterator to iterate.
* @return The Node the iterator is currently pointing to.
*/
Node* next(LinkedListIterator* iter);

/*
* @brief Determines if there is a next Node in the iteration.
*
* @param The iterator to iterate.
* @return A boolean indicating if there is a next item.
*/
bool has_next(LinkedListIterator* iter);

/*
* @brief Destroy and free up the memory corresonding to
* the iterator.
*
* @param The iterator to be destroyed.
*/
void destroy_iterator(LinkedListIterator* iter);

#endif // LINKEDLISTITERATOR_H

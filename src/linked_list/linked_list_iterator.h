#ifndef LINKEDLISTITERATOR_H
#define LINKEDLISTITERATOR_H

#include <stdbool.h>
#include "../linked_list/linked_list.h"
#include "../linked_list/node.h"

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
* @brief This function will make an iterator in the
* given iterator incrementing by one step.
*
* @param The iterator to iterate.
* @return The Node the iterator is currently pointing to.
*/
Node* next(LinkedListIterator* iterator);

/*
* @brief Determines if there is a next node in the iteration.
*
* @param The iterator to iterate.
* @return A boolean indicating if there is a next item.
*/
bool has_next(LinkedListIterator* iterator);

#endif // LINKEDLISTITERATOR_H

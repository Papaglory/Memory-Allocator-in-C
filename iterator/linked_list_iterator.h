#ifndef LINKEDLISTITERATOR_H
#define LINKEDLISTITERATOR_H

#include "../linked_list/linked_list.h"
#include "../linked_list/node.h"

typedef struct {

    Node* current;

} LinkedListIterator;

LinkedListIterator* create_iterator(LinkedList* list);

Node* iterate(LinkedListIterator* iterator);

#endif // LINKEDLISTITERATOR_H

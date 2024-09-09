#ifndef LINKEDLISTITERATOR_H
#define LINKEDLISTITERATOR_H

// Forward declaration
typedef struct Node Node;
typedef struct LinkedList LinkedList;

typedef struct {

    Node* current;

} LinkedListIterator;

LinkedListIterator create_iterator(LinkedList* list);

LinkedListIterator iterate(LinkedList* list, LinkedListIterator* iterator);

#endif // LINKEDLISTITERATOR_H

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

typedef struct Node Node;

typedef struct {

    // Pointer to first node
    Node* head;

    // Pointer to final node
    Node* tail;

    // Size of the list
    size_t size;

} LinkedList

void create_list();

void add(LinkedList list, Node node);

void delete(size_t id);

size_t search(Value val);

Node* get_head(LinkedList list);

Node* get_tail(LinkedList list);

#endif // LINKED_LIST_H

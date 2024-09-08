#include<stddef.h>

#ifndef TERMINATOR_ALLOC.H
#define TERMINATOR_ALLOC.H

typedef struct {
    unsigned char* sub_heap;
    size_t size;
    // Need some kind of register to keep track of size
    // corresponding to pointer handed out.
    // Perhaps create a hasmap?? linkedlist??
} TerminatorAlloc

/*
* @brief Given the size of the desired sub heap, an allocator
* will be created that manages this sub heap.
*
* @param The size of the sub heap that will be allocated.
* @return Returns a pointer to the object.
*/
TerminatorAlloc* create_terminator(size_t size);

/*
* $brief Terminate the given term object and free
* it from the heap.
*
* @param Pointer to the object to be terminated.
*/
void terminate_terminator(TerminatorAlloc* term);

void* term_malloc(size_t size);

void* term_realloc(size_t size);

void* term_free(void* ptr);

#endif

#ifndef TERMINATOR_ALLOC.H
#define TERMINATOR_ALLOC.H

typedef struct {
    unsigned char* heap;
} TerminatorAlloc


void* term_malloc(int size);

void* term_realloc(int size);

void* term_free(void* ptr);

TerminatorAlloc* get_instance();

#endif

/*
* Create as a singelton object
*/

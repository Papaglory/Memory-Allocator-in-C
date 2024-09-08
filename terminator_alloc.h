#ifndef TERMINATOR_ALLOC.H

#define TERMINATOR_ALLOC.H


typedef struct {
    unsigned char[] *heap;


} TerminatorAlloc



#endif

void* term_malloc(int size);

void* term_realloc(int size);

void* term_free(void* ptr);


/*
* Create as a singelton object
*/

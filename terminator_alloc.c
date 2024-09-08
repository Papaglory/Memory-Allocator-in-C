#include <terminator_alloc.h>
#include <stddef.h>

static TerminatorAlloc* term_alloc;

TerminatorAlloc* create_terminator(size_t size, void* heap_ptr) {

    // Allocate memory for the instance
    term = (TerminatorAlloc*) malloc(sizeof(TerminatorAlloc));

    // Make member variable point to allocated sub heap
    term->sub_heap = (unsigned char*) malloc(size);

}

void terminate_terminator(TerminatorAlloc* term) {

    // Free the memory corresponding to sub heap
    void* sub_heap = term->sub_heap;
    free(sub_heap)

    // Free TerminatorAlloc object
    free(term);

}

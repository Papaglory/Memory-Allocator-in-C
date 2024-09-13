#include <stddef.h>
#include <stdlib.h>
#include "terminator_alloc.h"

TerminatorAlloc* create_terminator(size_t size) {

    // Allocate memory for the instance
    TerminatorAlloc* term = (TerminatorAlloc*) malloc(sizeof(TerminatorAlloc));
    if (term == NULL) {

        // Allocation failed
        return NULL;

    }

    // Make member variable point to allocated sub heap
    term->sub_heap = (unsigned char*) malloc(size);
    if (term->sub_heap == NULL) {

        // Allocation failed
        free(term);
        return NULL;

    }

    // TODO implement
    return NULL;
}

void destroy_terminator(TerminatorAlloc* term) {

    // Free the memory corresponding to sub heap
    void* sub_heap = term->sub_heap;
    free(sub_heap);

    // Free TerminatorAlloc object
    free(term);

}

#include <stddef.h>
#include <stdlib.h>
#include "allocator.h"

Allocator* create_allocator(size_t size) {

    // Allocate memory for the instance
    Allocator* alloc = (Allocator*) malloc(sizeof(Allocator));
    if (alloc == NULL) {

        // Allocation failed
        return NULL;

    }

    // Make member variable point to allocated sub heap
    alloc->sub_heap = (unsigned char*) malloc(size);
    if (alloc->sub_heap == NULL) {

        // Allocation failed
        free(alloc);
        return NULL;

    }

    // TODO implement
    return NULL;
}

void destroy_allocator(Allocator* term) {

    // Free the memory corresponding to sub heap
    void* sub_heap = term->sub_heap;
    free(sub_heap);

    // Free TerminatorAlloc object
    free(term);

}

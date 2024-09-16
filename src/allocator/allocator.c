#include <stddef.h>
#include <stdlib.h>
#include "allocator.h"

Allocator* create_allocator(size_t size) {

    // Use the built-in C malloc to get the sub-heap
    void* sub_heap = malloc(size);



}

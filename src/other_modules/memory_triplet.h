#ifndef MEMORY_TRIPLET_H
#define MEMORY_TRIPLET_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {

    // Points to the start of a memory block
    void* memory_start;

    // The size of the memory block
    size_t block_size;

    // Whether the memory block is free or used
    bool free;

} MemoryTriplet;

MemoryTriplet

#endif // MEMORY_TRIPLET_H

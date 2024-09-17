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
    bool is_free;

} MemoryTriplet;

/*
* @brief Create a MemoryTriplet and allocate it on the sub-heap.
*
* @param1 Pointer to the start of memory block.
* @param2 Size of the memory block pointed to.
* @param3 Whether the memory block is free or used.
* @return A MemoryTriplet object.
*/
MemoryTriplet* create_memory_triplet(void* memory_start, size_t block_size, bool is_free);

/*
* @brief Destroy a MemoryTriplet object and free it from the sub-heap.
*
* @param The MemoryTriplet object to be destroyed and freed.
*/
void destroy_memory_triplet(MemoryTriplet* triplet);

#endif // MEMORY_TRIPLET_H

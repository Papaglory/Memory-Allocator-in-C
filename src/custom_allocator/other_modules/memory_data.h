#ifndef MEMORY_DATA_H
#define MEMORY_DATA_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {

    // Points to the start of a memory block
    char* memory_start;

    // The size of the memory block
    size_t block_size;

    // Whether the memory block is free or used
    bool is_free;

    // Whether the MemoryData is in use or not
    bool in_use;

} MemoryData;

/*
* @brief Create a MemoryData and allocate it on the sub-heap.
*
* @param1 Pointer to the start of memory block.
* @param2 Size of the memory block pointed to.
* @param3 Whether the memory block is free or used.
* @return A MemoryData object.
*/
MemoryData* create_memory_data(void* memory_start, size_t block_size, bool is_free);

/*
* @brief Destroy a MemoryData object and free it from the sub-heap.
*
* @param The MemoryData object to be destroyed and freed.
*/
void destroy_memory_data(MemoryData* triplet);

#endif // MEMORY_DATA_H

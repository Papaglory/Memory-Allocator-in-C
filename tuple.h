#ifndef TUPLE_H
#define TUPLE_H

#include <stddef.h>

typedef struct {

    // Reference to the first generic data
    void* first_data;

    // Reference to the second generic data
    void* snd_data;

    // Data size of 'first_data'
    size_t first_size;

    // Data size of 'snd_data'
    size_t snd_size;

} Tuple;

#endif // TUPLE_H

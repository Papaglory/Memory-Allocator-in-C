#include "../src/other_modules/memory_data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../src/allocator/allocator.h"
#include "../src/linked_list/linked_list_iterator.h"

void print_allocator_stats(Allocator* alloc) {

    if (!alloc) { return; }

    printf("\n%s\n", "FUNCTION CALL: print_allocator_stats");

    LinkedList* list = alloc->list;

    int align_size = 30;

    printf("%-*s%p\n", align_size, "heap_start:", alloc->heap_start);
    printf("%-*s%p\n", align_size, "heap_end:", alloc->heap_end);
    printf("%-*s%p\n", align_size, "user_pool_border:", alloc->user_pool_border);
    printf("%-*s%p\n", align_size, "reserved_pool_border:", alloc->reserved_pool_border);
    printf("%-*s%zu\n", align_size, "initial_reserved_pool_size:", alloc->initial_reserved_pool_size);
    printf("%-*s%zu\n", align_size, "heap_size:", alloc->heap_size);
    printf("%-*s%zu\n", align_size, "reserved_pool_size:", alloc->reserved_pool_size);

}

void print_list_stats(LinkedList* list) {

    if (!list) { return; }

    printf("\n%s\n", "FUNCTION CALL: print_list_stats");

    int align_size = 16;

    printf("%-*s%zu\n", align_size, "list size: ", list->size);

    int indent_size = 8;

    LinkedListIterator iter;
    iter.current = list->head;

    printf("%s\n", "Nodes:");

    while (has_next(&iter)) {

        Node* node = next(&iter);
        MemoryData* data = node->data;

        // Printing Node id
        printf(
            "%*s%-*s%zu\n",
            indent_size,
            "",
            align_size,
            "ID:",
            node->id
        );

        // Printing in_use with true/false text
        printf(
            "%*s%-*s%s\n",
            indent_size,
            "",
            align_size,
            "in_use:",
            data->in_use ? "true" : "false"
        );

        // Printing is_free with true/false text
        printf(
            "%*s%-*s%s\n",
            indent_size,
            "",
            align_size,
            "is_free:",
            data->is_free ? "true" : "false"
        );

        // Printing block_size
        printf(
            "%*s%-*s%zu\n",
            indent_size,
            "",
            align_size,
            "block_size:",
            data->block_size
        );

        // Printing memory_start
        printf(
            "%*s%-*s%p\n",
            indent_size,
            "",
            align_size,
            "memory_start:",
            data->memory_start
        );

        printf("\n");

    }

}

void creation_test() {

    printf("\n%s\n", "FUNCTION CALL: creation_test");
    // 152 bytes is the limit to create an Allocator atm
    Allocator* alloc = create_allocator(200);
    set_allocator(alloc);

    print_allocator_stats(alloc);

    LinkedList* list = alloc->list;
    print_list_stats(list);

    destroy_allocator();

}

void malloc_test() {

    printf("\n%s\n", "FUNCTION CALL: malloc_test");

    Allocator* alloc = create_allocator(200);
    set_allocator(alloc);

    int* foo = allocator_malloc(sizeof(int));

    // printf("malloc result: %p", foo);

    destroy_allocator();

}

int main() {

    // Seed the random number generator
    srand(time(NULL));

    printf("\n%s\n", "----TEST STARTED----");

    creation_test();

    //malloc_test();


    printf("\n%s\n", "----TEST ENDED----");

    return 0;

}

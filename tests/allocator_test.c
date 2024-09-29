#include "../src/other_modules/memory_data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../src/allocator/allocator.h"
#include "../src/linked_list/linked_list_iterator.h"
#include "../src/linked_list/merge_sort_linked_list.h"

void print_allocator_stats(Allocator* alloc) {

    if (!alloc) { return; }

    printf("\n%s\n", "FUNCTION CALL: print_allocator_stats");

    LinkedList* list = alloc->list;

    int align_size = 30;

    printf("%-*s%p\n", align_size, "heap_start:", alloc->heap_start);
    printf("%-*s%p\n", align_size, "heap_end:", alloc->heap_end);
    printf("%-*s%p\n", align_size, "reserved_pool_border:", alloc->reserved_pool_border);
    printf("%-*s%zu\n", align_size, "initial_reserved_pool_size:", alloc->initial_reserved_pool_size);
    printf("%-*s%zu\n", align_size, "heap_size:", alloc->heap_size);
    printf("%-*s%zu\n", align_size, "reserved_pool_size:", alloc->reserved_pool_size);

    printf("\n");
    printf("%-*s%zu\n", align_size, "Calculated heap size", alloc->heap_end-alloc->heap_start);

    printf("\n");

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

    printf("\n%s\n", "STARTING TEST: creation_test");
    // 152 bytes is the limit to create an Allocator atm
    Allocator* alloc = create_allocator(800);
    set_allocator(alloc);

    print_allocator_stats(alloc);

    LinkedList* list = alloc->list;
    print_list_stats(list);

    printf("Calling create residual node\n");
    fflush(stdout);
    Node* residual_node = create_residual_node(list->head, 10);

    add(list, residual_node);
    merge_sort_list(list);

    print_list_stats(list);

    destroy_allocator();

}

void malloc_test() {

    printf("\n%s\n", "STARTING TEST: malloc_test");

    Allocator* alloc = create_allocator(800);
    set_allocator(alloc);

    int* foo = allocator_malloc(sizeof(int));

    int align_size = 16;

    printf("%-*s%p\n", align_size, "malloc result:", foo);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    destroy_allocator();

}

void residual_node_test() {

    Allocator* alloc = create_allocator(800);
    set_allocator(alloc);

    Node* node = malloc(sizeof(Node));

    MemoryData* data = malloc(sizeof(MemoryData));

    node->data = (void*) data;
    node->id = 0;
    node->next = NULL;
    node->data_size = sizeof(MemoryData);


    data->in_use = true;
    data->is_free = true;
    data->memory_start = (char*) 8;
    data->block_size = 12;

    LinkedList list;

    add(&list, node);

    size_t residual_size = 10;

    Node* residual_node = create_residual_node(node, residual_size);

    printf("%p\n", node);
    printf("%p\n", residual_node);

    add(&list, residual_node);

    print_list_stats(&list);
}

int main() {

    // Seed the random number generator
    srand(time(NULL));

    printf("\n%s\n", "----TEST STARTED----");
    //residual_node_test();
    creation_test();

    //malloc_test();


    printf("\n%s\n", "----TEST ENDED----");

    return 0;

}

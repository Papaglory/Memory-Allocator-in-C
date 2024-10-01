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
    printf("%-*s%zu\n", align_size, "Calculated user pool size", alloc->reserved_pool_border-alloc->heap_start);
    size_t size = sizeof(Node) + sizeof(MemoryData);
    printf("%-*s%zu\n", align_size, "Calculated metadata Node size", size);

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

    print_allocator_stats(alloc);

    int* my_int = allocator_malloc(sizeof(int));
    *my_int = 42;

    int align_size = 16;

    printf("%-*s%p\n", align_size, "Address of int:", my_int);
    printf("%-*s%d\n", align_size, "Value of int:", *my_int);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    size_t* my_size = allocator_malloc(sizeof(size_t));
    *my_size = 101;

    printf("%-*s%p\n", align_size, "Address of size_t:", my_size);
    printf("%-*s%zu\n", align_size, "Value of size:", *my_size);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    destroy_allocator();

}

void free_test() {

    printf("\n%s\n", "STARTING TEST: free_test");

    Allocator* alloc = create_allocator(800);
    set_allocator(alloc);

    print_allocator_stats(alloc);

    int* my_int = allocator_malloc(sizeof(int));
    *my_int = 42;

    int* my_int2 = allocator_malloc(sizeof(int));
    *my_int = 49;

    int align_size = 16;

    printf("%-*s%p\n", align_size, "Address of int:", my_int);
    printf("%-*s%d\n", align_size, "Value of int:", *my_int);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    size_t* my_size = allocator_malloc(sizeof(size_t));
    *my_size = 101;

    printf("%-*s%p\n", align_size, "Address of size_t:", my_size);
    printf("%-*s%zu\n", align_size, "Value of size:", *my_size);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    char* char_ptr = allocator_malloc(sizeof(char*));
    *char_ptr = 'A';

    printf("%-*s%p\n", align_size, "Address of char_ptr:", char_ptr);
    printf("%-*s%c\n", align_size, "Value of char_ptr:", *char_ptr);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);


    printf("Calling Allocator free\n");
    allocator_free(my_int2);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    printf("Calling Allocator free\n");
    allocator_free(char_ptr);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    printf("Calling Allocator free\n");
    allocator_free(my_size);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);


    destroy_allocator();

}


void cleanse_reserved_pool_test() {

    printf("\n%s\n", "STARTING TEST: cleanse_reserved_pool_test");

    Allocator* alloc = create_allocator(800);
    set_allocator(alloc);

    print_allocator_stats(alloc);

    int* my_int = allocator_malloc(sizeof(int));
    *my_int = 42;

    int align_size = 16;
    int* my_int2 = allocator_malloc(sizeof(int));

    *my_int = 49;

    printf("%-*s%p\n", align_size, "Address of int:", my_int);
    printf("%-*s%d\n", align_size, "Value of int:", *my_int);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    size_t* my_size = allocator_malloc(sizeof(size_t));
    *my_size = 101;

    printf("%-*s%p\n", align_size, "Address of size_t:", my_size);
    printf("%-*s%zu\n", align_size, "Value of size:", *my_size);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    printf("Calling Allocator free\n");
    allocator_free(my_int);

    print_allocator_stats(alloc);
    print_list_stats(alloc->list);

    printf("Calling Allocator free\n");
    allocator_free(my_int2);

    print_allocator_stats(alloc);
    print_list_stats(alloc->list);

    printf("Calling cleanse_reserved_pool\n");
    cleanse_reserved_pool();
    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    destroy_allocator();

}

void cleanse_user_pool_test() {

    printf("\n%s\n", "STARTING TEST: cleanse_user_pool_test");

    Allocator* alloc = create_allocator(800);
    set_allocator(alloc);

    print_allocator_stats(alloc);

    int* my_int = allocator_malloc(sizeof(int));
    *my_int = 42;

    int align_size = 16;
    int* my_int2 = allocator_malloc(sizeof(int));

    *my_int = 49;

    printf("%-*s%p\n", align_size, "Address of int:", my_int);
    printf("%-*s%d\n", align_size, "Value of int:", *my_int);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    size_t* my_size = allocator_malloc(sizeof(size_t));
    *my_size = 101;

    printf("%-*s%p\n", align_size, "Address of size_t:", my_size);
    printf("%-*s%zu\n", align_size, "Value of size:", *my_size);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    printf("Calling Allocator free\n");
    allocator_free(my_int);

    print_allocator_stats(alloc);
    print_list_stats(alloc->list);

    printf("Calling Allocator free\n");
    allocator_free(my_size);

    print_allocator_stats(alloc);
    print_list_stats(alloc->list);

    printf("Calling cleanse_user_pool\n");
    cleanse_user_pool();
    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    destroy_allocator();

}


void realloc_test() {

    printf("\n%s\n", "STARTING TEST: realloc_test");

    Allocator* alloc = create_allocator(800);
    set_allocator(alloc);

    print_allocator_stats(alloc);

    int* my_int = allocator_malloc(sizeof(int));
    *my_int = 42;

    int arr_size = 4;
    int* arr = allocator_malloc(arr_size * sizeof(int));

    arr[0] = 10;
    arr[1] = 41;
    arr[2] = 42;
    arr[3] = 101;

    int align_size = 16;
    int* my_int2 = allocator_malloc(sizeof(int));

    *my_int2 = 49;

    printf("%-*s%p\n", align_size, "Address of int:", my_int);
    printf("%-*s%d\n", align_size, "Value of int:", *my_int);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    size_t* my_size = allocator_malloc(sizeof(size_t));
    *my_size = 101;

    printf("%-*s%p\n", align_size, "Address of size_t:", my_size);
    printf("%-*s%zu\n", align_size, "Value of size:", *my_size);

    print_allocator_stats(alloc);

    print_list_stats(alloc->list);

    // Free for array space
    //allocator_free(my_int);
    //allocator_free(my_int2);

    /*
     *
     * Because my_int was free, by merging we turned the
     * array which was non-free into free!
     */

    // Resize the array
    arr_size = 6;

    printf("Realloc called\n");
    arr = allocator_realloc(arr, arr_size*sizeof(int));

    print_allocator_stats(alloc);
    print_list_stats(alloc->list);


    printf("myint2: %d", *my_int2);

    destroy_allocator();

}


void foo() {

    printf("\n%s\n", "STARTING TEST: heap_full_test");

    Allocator* alloc = create_allocator(160);
    set_allocator(alloc);

    int* my_int = allocator_malloc(sizeof(int));
    *my_int = 42;

    int align_size = 16;

    printf("%-*s%p\n", align_size, "Address of int:", my_int);
    printf("%-*s%d\n", align_size, "Value of int:", *my_int);

    print_allocator_stats(alloc);
    print_list_stats(alloc->list);


}

void heap_full_test() {

    printf("\n%s\n", "STARTING TEST: heap_full_test");

    Allocator* alloc = create_allocator(160);
    set_allocator(alloc);

    print_allocator_stats(alloc);
    print_list_stats(alloc->list);

    int* my_int = allocator_malloc(sizeof(int));

    print_allocator_stats(alloc);
    print_list_stats(alloc->list);

    destroy_allocator();

}



int main() {

    // Seed the random number generator
    srand(time(NULL));

    printf("\n%s\n", "----TEST STARTED----");
    //creation_test();

    //malloc_test();

    //free_test();

    //cleanse_reserved_pool_test();

    //cleanse_user_pool_test();

    //realloc_test();
     heap_full_test();


    /*
     * TODO
     *
     * Implement padding.
     *
     *
     * Have multiple LinkedList, 8 bytes, 16 bytes, etc.?
     *
    */

    printf("\n%s\n", "----TEST ENDED----");

    return 0;

}

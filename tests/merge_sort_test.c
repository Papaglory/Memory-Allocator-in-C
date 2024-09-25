#include "../src/other_modules/memory_data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../src/linked_list/merge_sort_linked_list.h"
#include "../src/linked_list/linked_list_iterator.h"

void print_list(LinkedList* list) {

    if (!list) { return; }

    printf("\n%s\n", "Printing the list:");

    LinkedListIterator iter;
    iter.current = list->head;

    while (has_next(&iter)) {

        Node* node = next(&iter);

        MemoryData* data = node->data;

        printf("Node ID: %zu\n", node->id);
        fflush(stdout);
        printf("Memory start: %p\n", (void*) data->memory_start);
        fflush(stdout);

    }

}

void empty_list_test(LinkedList* list) {

    if (!list) { return; }

    printf("\n%s\n", "EMPTY LIST TEST:");

    merge_sort_list(list);

}

Node* create_test_node(size_t block_size, char* memory_start) {

    Node* node = (Node*) malloc(sizeof(Node));

    MemoryData* data = (MemoryData*) malloc(sizeof(MemoryData));

    // Set MemoryData member variables
    data->in_use = true;
    data->is_free = false;
    data->block_size = block_size;
    data->memory_start = memory_start;

    // Set Node member variables
    node->data = data;
    node->data_size = sizeof(MemoryData);
    node->id = 0;
    node->next = NULL;

    return node;

}

void singleton_list_test(LinkedList* list) {

    if (!list) { return; }

    printf("\n%s\n", "SINGLETON LIST TEST:");

    // Create Node
    size_t memory_block = 4;
    char* memory_start = (char*) 1;
    Node* node = create_test_node(memory_block, memory_start);
    add(list, node);

    merge_sort_list(list);

}

void pair_list_test(LinkedList* list) {

    if (!list) { return; }

    printf("\n%s\n", "PAIR LIST TEST:");

    // Create Node
    size_t memory_block_1 = 4;
    char* memory_start_1 = (char*) 3;
    Node* node_1 = create_test_node(memory_block_1, memory_start_1);
    add(list, node_1);

    // Create Node
    size_t memory_block_2 = 8;
    char* memory_start_2 = (char*) 2;
    Node* node_2 = create_test_node(memory_block_2, memory_start_2);
    add(list, node_2);

    print_list(list);
    merge_sort_list(list);

}

void multiple_list_test(LinkedList* list) {

    if (!list) { return; }

    printf("\n%s\n", "MULTIPLE LIST TEST:");

    // Create Node
    size_t memory_block_1 = 4;
    int val_1 = rand() % 100;
    char* memory_start_1 = (char*) val_1;
    Node* node_1 = create_test_node(memory_block_1, memory_start_1);
    add(list, node_1);

    // Create Node
    size_t memory_block_2 = 8;
    int val_2 = rand() % 100;
    char* memory_start_2 = (char*) val_2;
    Node* node_2 = create_test_node(memory_block_2, memory_start_2);
    add(list, node_2);

    // Create Node
    size_t memory_block_3 = 4;
    int val_3 = rand() % 100;
    char* memory_start_3 = (char*) val_3;
    Node* node_3 = create_test_node(memory_block_3, memory_start_3);
    add(list, node_3);

    // Create Node
    size_t memory_block_4 = 4;
    int val_4 = rand() % 100;
    char* memory_start_4 = (char*) val_4;
    Node* node_4 = create_test_node(memory_block_4, memory_start_4);
    add(list, node_4);

    // Create Node
    size_t memory_block_5 = 4;
    int val_5 = rand() % 100;
    char* memory_start_5 = (char*) val_5;
    Node* node_5 = create_test_node(memory_block_5, memory_start_5);
    add(list, node_5);

    print_list(list);
    merge_sort_list(list);

}

void reset_list(LinkedList* list) {

    list->head = NULL;
    list->size = 0;
    list->tail = NULL;
    list->next_id = 0;

}

int main() {

    // Seed the random number generator
    srand(time(NULL));

    printf("%s", "----TEST STARTED----\n");

    // Create an empty list
    LinkedList list;

    // TEST 1
    reset_list(&list);
    empty_list_test(&list);
    print_list(&list);

    // TEST 2
    reset_list(&list);
    singleton_list_test(&list);
    print_list(&list);

    // TEST 3
    reset_list(&list);
    pair_list_test(&list);
    print_list(&list);

    // TEST 4
    reset_list(&list);
    multiple_list_test(&list);
    print_list(&list);

    printf("%s", "----TEST ENDED----\n");

    return 0;

}

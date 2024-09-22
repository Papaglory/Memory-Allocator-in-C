#include "merge_sort_linked_list.h"
#include "../other_modules/memory_data.h"
#include <stdbool.h>

Node* split_list(Node* head) {

    if (head == NULL) {

        // There is no Node
        return NULL;

    }

    // Slow and fast pointer technique
    Node* slow = head;
    Node* fast = head;

    /*
    * Loop until the fast pointer has reached the end of
    * the list, the slow pointer will then be in the middle.
    */
    while (fast != NULL && fast->next != NULL) {

        slow = slow->next;
        fast = fast->next->next;

    }

    return slow;

}

Node* merge(Node* left, Node* right) {

    if (left == NULL) {

        return right;

    }

    if (right == NULL) {

        return left;

    }

    Node* result = NULL;

    MemoryData* left_data = left->data;
    MemoryData* right_data = right->data;

    if (left_data->memory_start <= right_data->memory_start) {

        result = left;
        result->next = merge(left->next, right);


    } else {

        result = right;
        result->next = merge(left, right->next);

    }

    return result;

}

Node* merge_sort(Node* head) {

    if (head == NULL || head->next == NULL) {

        return head;

    }

    Node* middle = split_list(head);
    Node* left = head;
    Node* right = middle->next;

    // Make an end to the first half ot the list
    middle->next = NULL;

    left = merge_sort(left);
    right = merge_sort(right);


    return merge(left, right);

}

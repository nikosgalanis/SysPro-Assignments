#include "../include/LinkedList.h"

// Create and initialize a new list
List create_list(CompareFunc compare, DestroyFunc destroy) {
    List new_list = malloc(sizeof(*new_list));
    new_list->head = NULL;
    new_list->size = 0;
    new_list->compare = compare;
    new_list->destroy = destroy;

    return new_list;
}

// Insert an item in the end of a list
void list_insert(List list, Pointer node_info) {
    // Special behavior if a list is empty
    if (list->size == 0) {
        list->head = malloc(sizeof(*list->head));
        list->head->value = node_info;
        list->head->next = NULL;
        list->size++;
        return;
    }
    ListNode temp = list->head;
    while ( temp->next != NULL) {
        temp = temp->next;
    }
    
    ListNode new_node = malloc(sizeof(*new_node));
    new_node->value = node_info;
    new_node->next = NULL;
    temp->next = new_node;
    list->size++;
}

// Search for a specific entry into the list
bool list_search(List list, Pointer node_info) {
    ListNode temp = list->head;

    while (temp != NULL) {
        if (list->compare(temp->value, node_info) == 0) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

// Return the size of the list
int list_size(List list) {
    return list->size;
}

// Return the nth item of a list
Pointer list_nth(List list, int n) {
    ListNode temp = list->head;
    while (temp) {
        if (n == 0) {
            return temp->value;
        }
        temp = temp->next; n--;
    }
    // If we reach this point, then the item is not on the list, thus return NULL
    return NULL;
}

bool is_empty(List list) {
    return (list->size == 0);
}
// Destroy a list by freeing all of the memory occupied
void destroy_list(Pointer l) {
    List list = (List)l;
    ListNode temp = list->head;
        while (temp != NULL) {
        ListNode next = temp->next;
        if (list->destroy) {
            list->destroy(temp->value);
        }
        free(temp);
        temp = next;
    }
    free(list);
}

#pragma once

#include "common_types.h"

typedef struct list_node* ListNode;
typedef struct list* List;


struct list_node {
    Pointer value;
    ListNode next;  
};

struct list {
    ListNode head;
    int size;
    DestroyFunc destroy;
    CompareFunc compare;
};


List create_list(CompareFunc compare, DestroyFunc destroy);
void list_insert(List list, Pointer node);
bool list_search(List list, Pointer value);
int list_size(List list);
Pointer list_nth(List list, int n);
void destroy_list(Pointer list);

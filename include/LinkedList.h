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
bool in_list(List list, Pointer node_info);
int list_size(List list);
bool is_empty(List list);
Pointer list_nth(List list, int n);
void destroy_list(Pointer list);

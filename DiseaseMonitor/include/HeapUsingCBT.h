#pragma once

#include "TreeTypes.h"

struct binary_tree_entry {
	char* key;
	int priority;
};

typedef struct binary_tree_entry* BTEntry;
typedef struct binary_tree_node* BTNode;

struct binary_tree_node {
	BTNode left, right, parent;
	BTEntry value;
};
// A Complete binary tree node, is just a node
typedef BTNode CBTreeNode;

// As well as a complete binary tree
typedef Tree CBTree;

CBTree create_CBTree(DestroyFunc destroy);
CBTreeNode create_cbinary_node(Pointer item);
CBTreeNode get_node_by_index (CBTree tree, int i);
void CBTree_insert(CBTree tree, BTEntry entry);
// void CBTree_destroy(Pointer t);

// Our heap is just a complete binary tree
typedef CBTree Heap;

typedef BTEntry HeapEntry;
typedef CBTreeNode HeapNode;

HeapEntry create_heap_entry(char* key, int priority);
Heap create_heap(DestroyFunc destroy);
void heapify_up(Heap heap, BTNode node);
void heapify_down(Heap heap, BTNode node);
void heap_insert(Heap heap, int priority, char* key);
HeapNode get_nth_node(Heap heap, int i);
HeapEntry pop(Heap heap);
void destroy_heap(Heap heap);
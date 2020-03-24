#include <stdlib.h>
#include "HeapUsingCBT.h"
#include <stdio.h>



//============= Complete binary tree functions ================//

// We are going to use those function in order to create a heap

CBTree create_CBTree(DestroyFunc destroy) {
   CBTree tree = malloc(sizeof(*tree));
   tree->root = NULL;
   tree->compare = NULL;
   tree->destroy = destroy;
   tree->size = 0;

   return tree;
}

CBTreeNode create_cbinary_node(Pointer item) {
	CBTreeNode node = malloc(sizeof(*node));
	node->value = item;
	node->left = node->right = node->parent = NULL;

	return node;
}

// Given a integer, return the node in the correct position of that tree
CBTreeNode get_node_by_index (CBTree tree, int i) {
	// basecase: if we want the 1st item, then it's in the root
	if (i == 1){
		return tree->root;
	} 
	// else, we want to call recursively the function to reach the childs
	// all the even numbers are right childs 
	else if (i % 2 == 0) {
		CBTreeNode node = get_node_by_index(tree, i / 2);
		return node->left;
	}
	// all the odd numbers are right childs
	else {
		CBTreeNode node = get_node_by_index(tree, i / 2);
		return node->right;
	}
}

// Insert a new entry into a complete binary tree
void CBTree_insert(CBTree tree, BTEntry entry) {
	int pos = tree->size + 1;
	// Same logic with the previous function
	if (pos == 1) {
		tree->root = create_cbinary_node(entry);
	}
	else if (pos % 2 == 0) {
		CBTreeNode node = get_node_by_index(tree, pos / 2);
		node->left = create_cbinary_node(entry);
		node->left->parent = node; 
	}
	else {
		CBTreeNode node = get_node_by_index(tree, pos / 2);
		node->right = create_cbinary_node(entry);
		node->right->parent = node;
	}
	tree->size++;
}

// remove the last entry from a cbt
BTEntry remove_last(CBTree tree) {
	int pos = tree->size;
	assert (pos > 0);
	CBTreeNode node = get_node_by_index(tree, pos);
	if (node->parent != NULL) {
		if (pos % 2 == 0) {
			node->parent->left = NULL;
		} else {
			node->parent->right = NULL;
		}
	}
	BTEntry to_return = node->value;
	free(node);
	tree->size--;
	return to_return;
} 

//============= Binary heap functions =========================//

// Create a heap entry
HeapEntry create_heap_entry(char* key, int priority) {
	HeapEntry new_entry = malloc(sizeof(*new_entry));
	new_entry->key = key;
	new_entry->priority = priority;

	return new_entry;
}

// Create a heap, simply by creating a complete bt
Heap create_heap(DestroyFunc destroy) {
	return create_CBTree(destroy);
}

void heapify_up(Heap heap, HeapNode node) {
	// if the root is null, there is nothing to do
	if (heap->root == NULL)
		return;
	HeapNode parent;
	// we continue in this loop while the child has bigger priority than the parent
	// thus we need to swap them
	while ((parent = node->parent) != NULL) {
		HeapEntry parent_entry = parent->value;
		HeapEntry node_entry = node->value;
		// done! the heap property is restored!
		if (parent_entry->priority >= node_entry->priority)
			return;
		// performing the neccesary swapping
		parent->value = node_entry;
		node->value = parent_entry;
		// we move up one level
		node = parent;
	}
}

void heapify_down(Heap heap, HeapNode node) {
	// Nothing to be done if the node that we want to begin the heapify from is null!
	if (node == NULL)
		return;
	HeapNode child;
	// we continue in this loop while the child has bigger priority than the parent
	// thus we need to swap them
	while ((child = node->left) != NULL) {
		// we naively choose the left child to compare. gonna change that very soon...
		HeapEntry child_entry = child->value;
		HeapEntry node_entry = node->value;

		// The right node might have bigger priority than the left, so we check that (here, we've changed our naive assumption)
		HeapNode right = node->right;
		if ((right != NULL) && right->value->priority > child->value->priority) {
			child = right;
			child_entry = right->value;
		}
		// done! the heap priority is restored!
		if (node->value->priority >= child->value->priority)
			return;
		// performing the neccesary swapping
		node->value = child_entry;
		child->value = node_entry;
		// we move down one level
		node = child;
	}
}

// Insert a new (key, priority) pair into the heap
void heap_insert(Heap heap, int priority, char* key) {
	// Create the entry
	HeapEntry entry = create_heap_entry(key, priority);
	// insert it as the last item in the heap
	CBTree_insert(heap, entry);
	// heapify so the heap priority is preserved
	heapify_up(heap, get_node_by_index(heap, heap->size));
}

HeapNode get_nth_node(Heap heap, int i) {
	return get_node_by_index(heap, i);
}

// Pop the item of the heap with the biggest priority
HeapEntry pop(Heap heap) {
	// Get access to the root of the heap
	HeapNode root = heap->root;
	assert(root != NULL);
	// Hold the value we want to return
	HeapEntry to_return = root->value;
	// remove the last value of the heap, in order to place it in the root
	HeapEntry last = remove_last(heap);
	// if there are still items in the heap
	if (heap->size != 0) {
		// assign the last item into the root
		root->value = last;
		// heapify so the heap priority is preserved
		heapify_down(heap, heap->root);
	}
	// return the item with the biggest priority that we've collected
	return to_return;
}

void destroy_heap(Heap heap) {
	while (heap->size != 0) {
		HeapEntry entry = pop(heap);
		if (heap->destroy != NULL)
			heap->destroy(entry);
		free(entry);
	}
	free(heap);
}
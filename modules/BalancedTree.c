#include <stdlib.h>
#include "BalancedTree.h"
#include "Patient.h"
#include <string.h>
#include <stdio.h>

extern Pointer empty;

// Create a new tree entry
BalancedTreeEntry create_balanced_tree_entry(Date date, Pointer assgn) {
	BalancedTreeEntry entry = malloc(sizeof(*entry));
	assert(entry != NULL);
	entry->date = date;
	entry->assigned_patient = assgn;

	return entry;
}

// Create a new tree node
TreeNode create_tree_node(BalancedTreeEntry value) {
	return create_binary_node(value);
}

// Create an empty balanced tree
BalancedTree create_balanced_tree(CompareFunc compare, DestroyFunc destroy) {
	return create_binary_tree(compare, destroy);
}

// insert a node to the dree
TreeNode insert_node_to_tree(Tree tree, TreeNode node, BalancedTreeEntry entry) {
	return insert_binary_node(node, tree->compare, entry);
}

// Give the root of the tree, in order to insert a new entry to it.
void balanced_tree_insert(BalancedTree tree, BalancedTreeEntry value) {
	assert(value != NULL);
	tree->root = insert_node_to_tree(tree, tree->root, value);
	tree->size++;
}

// Find all the nodes in a tree that are grater than a const, and satisfy a given condition
int total_nodes_grater_than(BalancedTree tree, Pointer x, ConditionFunc cond, char* cond_item) {
	return grater_than(tree->root, x, tree->compare, cond, cond_item);
}

// Recursive function to find all the nodes in a tree that are grater than a const
int grater_than(TreeNode node, Pointer x, CompareFunc compare, ConditionFunc cond, char* cond_item) {
	int count = 0;
	if (node != NULL) {
		if (compare(x, node->value) <= 0) {
			count += grater_than(node->left, x, compare, cond, cond_item);
			if (cond == NULL)
				count++;
			else {
				if (cond(node->value, cond_item, NULL, NULL) == true)
					count++;
			}
		}
		count += grater_than(node->right, x, compare, cond, cond_item);
	}
	return count;
}

// Traverse a balanced tree, bt recursively calling node_traverse
int balanced_tree_cond_traverse(Pointer t, ConditionFunc cond, Pointer a, Pointer d1, Pointer d2) {
	BalancedTree tree = (BalancedTree)t;
	return node_cond_traverse(tree->root, cond, a, d1, d2);
}

// Return how many nodes satisfy the condition fuction passed to the func.
int node_cond_traverse(TreeNode node, ConditionFunc cond, Pointer a, Pointer d1, Pointer d2) {
	int count = 0;
	if (node != NULL) {
		count += node_cond_traverse(node->left, cond, a, d1, d2);
		if (cond(node->value, a, d1, d2) == true) {
			count++;
		}
		count += node_cond_traverse(node->right, cond, a, d1, d2);
	}
	return count;
}

// Traverse a balanced tree with a visit function
void balanced_tree_traverse(BalancedTree tree, VisitFunc visit, Pointer c, Pointer d1, Pointer d2, Pointer p) {
	node_traverse(tree->root, visit, c, d1, d2, p);
}

void node_traverse(TreeNode node, VisitFunc visit, Pointer c, Pointer d1, Pointer d2, Pointer p) {
	if (node != NULL) {
		node_traverse(node->left, visit, c, d1, d2, p);
		visit(node->value, c, d1, d2, p);
		node_traverse(node->right, visit, c, d1, d2, p);
	}
}

// Destroy a balanced tree, by freeing all the memory allocated
void balanced_tree_destroy(Pointer t) {
	BalancedTree tree = (BalancedTree)t;
	if (tree != empty) {
		destroy_node(tree->root, tree->destroy);
		free(tree);
	}
}


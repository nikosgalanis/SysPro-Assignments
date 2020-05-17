#include <stdlib.h>
#include "BalancedTree.h"
#include "Patient.h"
#include <string.h>
#include <stdio.h>
// Return the max value between 2 ints
int max(int a, int b) {
	return (a > b) ? a : b;
}

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

// Find the height of a balanced tree node
int height(TreeNode node) {
	return node != NULL ? node->height : 0;
}

// return the balance of the node (aka the difference between the 2 subtrees)
int node_balance(TreeNode node) {
	return height(node->left) - height(node->right);
}

// Update the node height
void update_node_height(TreeNode node) {
	node->height = max(height(node->right), height(node->left)) + 1;
}

//========Implementation of the 4 esential tree rotations=========//

TreeNode right_rotation(TreeNode node) {
	TreeNode left = node->left;
	TreeNode left_right = left->right;

	left->right = node;
	node->left = left_right;

	update_node_height(node);
	update_node_height(left);

	return left;
}

TreeNode left_rotation(TreeNode node) {
	TreeNode right = node->right;
	TreeNode right_left = right->left;

	right->left = node;
	node->right = right_left;

	update_node_height(node);
	update_node_height(right);

	return right;
}

TreeNode left_right_rotation(TreeNode node) {
	node->left = left_rotation(node->left);
	return right_rotation(node);
}

TreeNode right_left_rotation(TreeNode node) {
	node->right = right_rotation(node->right);
	return left_rotation(node);
}

// Make sure that the balanced property is maintained. If not, repair it
TreeNode maintain_property(TreeNode node) {
	update_node_height(node);
	int balance = node_balance(node);
	// If the right subtree is unbalanced
	if (balance < -1) {
		if (node_balance(node->right) >= 0) {
			return right_left_rotation(node);
		} else {
			return left_rotation(node);
		}
	} else if (balance > 1) { // If the left subtree is unbalanced
		if (node_balance(node->left) >= 0) {
			return right_rotation(node);
		} else {
			return left_right_rotation(node);
		}
	} else {   // Everything is balanced! Just return the node, no rotation needed
		return node;
	}
}

TreeNode insert_node_to_tree(Tree tree, TreeNode node, BalancedTreeEntry entry) {
	if (node == NULL) {
		// Base case, if we reach a leaf, insert the entry
		return create_binary_node(entry);
	}
	// Apply the rules of binary search
	if (tree->compare(entry, node->value) <= 0) {
		node->left = insert_node_to_tree(tree, node->left, entry);
	} else {
		node->right = insert_node_to_tree(tree, node->right, entry);
	}
	return maintain_property(node);
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


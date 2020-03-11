#include <stdlib.h>
#include "BalancedTree.h"
#include <string.h>

// Return the max value between 2 ints
static int max(int a, int b) {
	return (a > b) ? a : b ;
}

// Create a new tree entry
TreeEntry create_tree_entry(Date date, Pointer assgn) {
    TreeEntry entry = malloc(sizeof(*entry));

    entry->date = date;
    entry->assigned_patient = assgn;
}

// Create a new tree node
TreeNode create_tree_node(TreeEntry value) {
    TreeNode node = malloc(sizeof(*node));

    node->right = NULL; node->right = NULL;
    node-> value = value;
    node->height = 1; // Initially our height will be 1. We'll fix that later

    return node;
}

// Create an empty balanced tree
Tree create_tree(CompareFunc compare, DestroyFunc destroy) {
    Tree tree = malloc(sizeof(*tree));
    tree->root = NULL;
    tree->size = 0;
    tree->compare = compare;
    tree->destroy = destroy;

    return tree;
}

// Update the node height
static void update_node_height(TreeNode node) {
	node->height = 1 + int_max(node_height(node->left), node_height(node->right));
}

// Return the difference of the heights of the left & right subtree
static int node_balance(TreeNode node) {
	return node_height(node->left) - node_height(node->right);
}
//====Implementation of the 4 esential tree rotations====//

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
    TreeNode left = node->left;

    left = left_rotation(left);
    return right_rotation(node);
}

TreeNode right_left_rotation(TreeNode node) {
    TreeNode right = node->right;

    right = right_rotation(right);
    return left_rotation(node);
}

// Make sure that the balanced property is maintained. If not, repair it
TreeNode maintain_property(TreeNode node) {
    update_node_height(node);
    int balance = node_balance(node);
    // If the right subtree is unbalanced
    if (balance < -1) {
        if (node_balance(node->right) <=0) {
            return left_rotation(node);
        } else {
            return right_left_rotation(node);
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

Tree insert_node_to_tree(Tree tree, TreeNode node, TreeEntry entry) {
    if (node == NULL) {
        return create_tree_node(entry);
    }
    int result = tree->compare(node->value, entry);
    if (result <= 0) {
        node->left = insert_node_to_tree(tree, node->left, entry);
    } else {
        node->right = insert_node_to_tree(tree, node->right, entry);
    }

}

// Give the root of the tree, in order to insert a new entry to it.
void tree_insert(Tree tree, TreeEntry value) {
    tree->root = insert_node_to_tree(tree, tree->root, value);
    tree->size++;
}


#include <stdlib.h>
#include "BalancedTree.h"
#include "Patient.h"
#include <string.h>
#include <stdio.h>
// Return the max value between 2 ints
static int max(int a, int b) {
	return (a > b) ? a : b;
}

// Create a new tree entry
TreeEntry create_tree_entry(Date date, Pointer assgn) {
    TreeEntry entry = malloc(sizeof(*entry));

    entry->date = date;
    entry->assigned_patient = assgn;

    return entry;
}

// Create a new tree node
TreeNode create_tree_node(TreeEntry value) {
    TreeNode node = malloc(sizeof(*node));

    node->right = NULL; node->left = NULL;
    node->value = value;
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

static int node_height(TreeNode node) {
    return node != NULL ? node->height : 0;
}

// Update the node height
static void update_node_height(TreeNode node) {
	node->height = 1 + max(node_height(node->left), node_height(node->right));
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

TreeNode insert_node_to_tree(Tree tree, TreeNode node, TreeEntry entry) {
    if (node == NULL) {
        return create_tree_node(entry);
    }
    int result = tree->compare(entry, node->value);
    if (result <= 0) {
        node->left = insert_node_to_tree(tree, node->left, entry);
    } else {
        node->right = insert_node_to_tree(tree, node->right, entry);
    }
    return maintain_property(node);
}

// Give the root of the tree, in order to insert a new entry to it.
void tree_insert(Tree tree, TreeEntry value) {
    if (value != NULL) {
        tree->root = insert_node_to_tree(tree, tree->root, value);
        tree->size++;
    }
}

// Find all the nodes in a tree that are grater than a const, and satisfy a given condition
int total_nodes_grater_than(Tree tree, Pointer x, ConditionFunc cond, char* cond_item) {
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
                if (cond(node->value, cond_item) == true)
                    count++;
            }
        }
        count += grater_than(node->right, x, compare, cond, cond_item);
    }
    return count;
}

int tree_traverse(Tree tree, ConditionFunc cond) {
    return node_traverse(tree->root, cond);
}

int node_traverse(TreeNode node, ConditionFunc cond) {
    int count = 0;
    if (node != NULL) {
        count += node_traverse(node->left, cond);
        if (cond(node->value, NULL) == true) {
            count++;
        }
        count += node_traverse(node->right, cond);
    }
    return count;
}

void tree_destroy(Pointer t) {
    Tree tree = (Tree)t;
     destroy_node(tree->root, tree->destroy);
    free(tree);
}

void destroy_node(TreeNode node, DestroyFunc destroy) {
    if (node != NULL) {
        destroy_node(node->left, destroy);
        if (destroy != NULL)
            destroy(node->value);
        destroy_node(node->right, destroy);
    }
    free(node);
}

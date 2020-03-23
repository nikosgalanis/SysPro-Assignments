#include <stdlib.h>

#include "BinaryTree.h"
extern Pointer empty;
// Create a node for a binary tree
TreeNode create_binary_node(Pointer item) {
    TreeNode node = malloc(sizeof(*node));
    assert(node != NULL);
    node->value = item;
    node->left = node->right = NULL;

    node->height = 0; // In this implementation we do not care about the height

    return node;
}

// Insert a node into a binary tree, specialized in __binary search__
TreeNode insert_binary_node(TreeNode node, CompareFunc compare, Pointer item) {
    if (node == NULL) {
        // Base case, if we reach a leaf, insert the entry
        return create_binary_node(item);
    }
    // Apply the rules of binary search
    if (compare(item, node->value) > 0) {
        node->left = insert_binary_node(node->left, compare, item);
    } else {
        node->right = insert_binary_node(node->right, compare, item);
    }
    return node;
}

// Initialize the bt struct
BinaryTree create_binary_tree(CompareFunc compare, DestroyFunc destroy) {
   BinaryTree tree = malloc(sizeof(*tree));
   assert(tree != NULL);
   tree->root = NULL;
   tree->compare = compare;
   tree->destroy = destroy;
   tree->size = 0;

   return tree; 
}

// Insert to the binary tree, by recursively calling the insert function, starting from the tree's root
void binary_tree_insert(BinaryTree tree, Pointer item) {
    tree->root = insert_binary_node(tree->root, tree->compare, item);
    tree->size++;
}

// Destroy a binary tree, by applying a destroy function in ecah node
void destroy_node(TreeNode node, DestroyFunc destroy) {
    if (node != NULL) {
        destroy_node(node->left, destroy);
        destroy_node(node->right, destroy);
        if (destroy != NULL)
            destroy(node->value);
        free(node->value);
        free(node);
    }
}

#include <stdlib.h>

#include "BinaryTree.h"

TreeNode create_binary_node(Pointer item) {
    TreeNode node = malloc(sizeof(*node));
    node->value = item;
    node->left = NULL;
    node->right = NULL;
    node->height = 0; // In this implementation we do not care about the height

    return node;
}

TreeNode insert_binary_node(TreeNode node, CompareFunc compare, Pointer item) {
    if (node == NULL) {
        return create_binary_node(item);
    }
    if (compare(item, node->value) > 0) {
        node->left = insert_binary_node(node->left, compare, item);
    } else {
        node->right = insert_binary_node(node->right, compare, item);
    }
    return node;
}



BinaryTree create_binary_tree(CompareFunc compare, DestroyFunc destroy) {
   BinaryTree tree = malloc(sizeof(*tree));
   tree->root = NULL;
   tree->compare = compare;
   tree->destroy = destroy;
   tree->size = 0;

   return tree; 
}

void binary_tree_insert(BinaryTree tree, Pointer item) {
    tree->root = insert_binary_node(tree->root, tree->compare, item);
    tree->size++;
}

void destroy_node(TreeNode node, DestroyFunc destroy) {
    if (node != NULL) {
        destroy_node(node->left, destroy);
        destroy_node(node->right, destroy);
        if (destroy != NULL)
            destroy(node->value);
    }
    free(node);
}

#pragma once

#include "TreeTypes.h"

typedef struct bs_tree_node* TreeNode;

struct bs_tree_node {
    TreeNode left, right;
    Pointer value;
    uint height; 
};

typedef Tree BinaryTree;

TreeNode create_binary_node(Pointer item);
TreeNode insert_binary_node(TreeNode node, CompareFunc compare, Pointer item);
BinaryTree create_binary_tree(CompareFunc compare, DestroyFunc destroy);
void binary_tree_insert(BinaryTree tree, Pointer item);
void destroy_node(TreeNode node, DestroyFunc destroy);
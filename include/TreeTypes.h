#pragma once

#include "common_types.h"


typedef struct tree_node* TreeNode;
typedef struct tree_entry* TreeEntry;
// Our nodes have 2 child nodes, their value, and ther height in the tree
// in order for it to remain balanced
struct tree_node {
    TreeNode left, right;
    TreeEntry value;
    uint height; 
};

struct tree {
    TreeNode root;
    int size;
    CompareFunc compare;
    DestroyFunc destroy;
};

typedef struct tree* Tree;
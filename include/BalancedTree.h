#pragma once
#include "common_types.h"
#include "Dates.h"

typedef struct tree_node* TreeNode;
typedef struct tree_entry* TreeEntry;
typedef struct tree* Tree;
// Our entries to our bst will be of this form
struct tree_entry {
    Date date;
    Pointer assigned_patient;
};

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

TreeEntry create_tree_entry(Date date, Pointer assgn);
TreeNode create_tree_node(TreeEntry value);
Tree create_tree(CompareFunc compare, DestroyFunc destroy);
void tree_insert(Tree tree, TreeEntry value);
int total_nodes_grater_than(Tree tree, Pointer x);
int grater_than(TreeNode node, Pointer x, CompareFunc compare);
#pragma once
#include "BinaryTree.h"
#include "Dates.h"


// Our entries to our bst will be of this form
struct tree_entry {
	Date date;
	Pointer assigned_patient;
};
typedef struct tree_entry* TreeEntry;

typedef BinaryTree BalancedTree;
typedef TreeEntry BalancedTreeEntry;

BalancedTreeEntry create_balanced_tree_entry(Date date, Pointer assgn);
TreeNode create_tree_node(BalancedTreeEntry value);
BalancedTree create_balanced_tree(CompareFunc compare, DestroyFunc destroy);
void balanced_tree_insert(BalancedTree tree, BalancedTreeEntry value);
int total_nodes_grater_than(BalancedTree tree, Pointer x, ConditionFunc cond, char* cond_item);
int grater_than(TreeNode node, Pointer x, CompareFunc compare, ConditionFunc cond, char* cond_item);
int balanced_tree_cond_traverse(Pointer tree, ConditionFunc cond, Pointer a);
int node_cond_traverse(TreeNode node, ConditionFunc cond, Pointer a);
void balanced_tree_traverse(BalancedTree tree, VisitFunc visit, Pointer c, Pointer d1, Pointer d2, Pointer p);
void node_traverse(TreeNode node, VisitFunc visit, Pointer c, Pointer d1, Pointer d2, Pointer p);
void balanced_tree_destroy(Pointer tree);
void destroy_node(TreeNode node, DestroyFunc destroy);

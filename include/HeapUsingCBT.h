#pragma once

#include "BinaryTree.h"

// A Complete binary tree node, is just a node
typedef TreeNode CBTreeNode;

// As well as a complete binary tree
typedef Tree CBTree;

CBTree create_CBTree(CompareFunc compare, DestroyFunc destroy);
TreeNode get_node_by_index (CBTree tree, int i);
void CBTree_insert(CBTree tree, TreeEntry entry);
void CBTree_destroy(Pointer t);

// Our heap is just a complete binary tree
typedef CBTree Heap;


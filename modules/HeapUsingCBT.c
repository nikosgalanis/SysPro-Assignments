#include <stdlib.h>
#include "HeapUsingCBT.h"

//============= Complete binary tree functions ================//

// We are going to use those function in order to create a heap

CBTree create_CBTree(CompareFunc compare, DestroyFunc destroy) {
    return create_binary_tree(compare, destroy);
}

// Given a integer, return the node in the correct position of that tree
TreeNode get_node_by_index (CBTree tree, int i) {
    if (i == 1){
        return tree->root;
    } 
    else if (i % 2 == 0) {
        TreeNode node = get_node_by_index(tree, i / 2);
        return node->left;
    }
    else {
        TreeNode node = get_node_by_index(tree, i / 2);
        return node->right;
    }
}

// Insert a new entry into a complete binary tree
void CBTree_insert(CBTree tree, TreeEntry entry) {
    int pos = tree->size + 1;
    if (pos == 1) {
        tree->root = create_binary_node(entry);
    }
    else if (pos % 2 == 0) {
        TreeNode node = get_node_by_index(tree, pos / 2);
        node->left = create_binary_node(entry);
    }
    else {
        TreeNode node = get_node_by_index(tree, pos / 2);
        node->right = create_binary_node(entry);
    }
    tree->size++;
}

void CBTree_destroy(Pointer t) {
    CBTree tree = (CBTree)t;
    destroy_node(tree->root, tree->destroy);
}



//============= Binary heap functions =========================//
Heap create_heap(CompareFunc compare, DestroyFunc destroy) {
    return create_CBTree(compare, destroy);
}


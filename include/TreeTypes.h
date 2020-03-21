#pragma once

#include "common_types.h"

// Our nodes have 2 child nodes, their value, and ther height in the tree
// in order for it to remain balanced

struct tree {
    Pointer root;
    int size;
    CompareFunc compare;
    DestroyFunc destroy;
};

typedef struct tree* Tree;
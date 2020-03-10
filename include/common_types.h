#pragma once
#include <stdbool.h> 

// Usefull typedefs
typedef void* Pointer;
#include "HashTable.h"

typedef unsigned int uint;

typedef int (*CompareFunc)(Pointer a, Pointer b);
typedef void (*DestroyFunc)(Pointer value);
int nlines(FILE* input);

// Our global pointers to store the hash tables
HashTable diseaseHashTable;
HashTable countryHashTable;
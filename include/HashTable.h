#pragma once
#include "common_types.h"

typedef uint (*HashFunc)(Pointer);

struct hash_entry {
	char name[32];
	Pointer tree_root;
};

typedef struct hash_entry* HashEntry;

struct hash_node {
	HashEntry* bucket;
	struct hash_node* next;	
};

typedef struct hash_node* HashNode;


#define HASH_EOF (HashNode)0

struct hash_table {
	HashNode* array;
	int size;
	int items;
	int bucket_size;
	HashFunc hash_function;
};

typedef struct hash_table* HashTable;

HashTable hash_create(int size, HashFunc hash_fn, int bucket_size);
void hash_insert(HashTable ht, HashEntry new_entry);
HashEntry hash_search(HashTable ht, char* name);
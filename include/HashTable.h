#pragma once
#include "common_types.h"

typedef uint (*HashFunc)(Pointer);

struct hash_entry {
	char* key;
	Pointer item;
};

typedef struct hash_entry* HashEntry;

typedef struct hash_node* HashNode;

struct hash_node {
	HashEntry* bucket;
	HashNode next;	
};



#define HASH_EOF (HashNode)0

struct hash_table {
	HashNode* array;
	int size;
	int items;
	int bucket_size;
	HashFunc hash_function;
	DestroyFunc destroy_items;
};

typedef struct hash_table* HashTable;

HashEntry create_hash_entry(char* key, Pointer item);
HashTable hash_create(int size, HashFunc hash_fn, int bucket_size, DestroyFunc destroy);
void hash_insert(HashTable ht, HashEntry new_entry);
HashEntry hash_search(HashTable ht, char* name);
void hash_traverse(HashTable ht, PrintFunc print, Pointer d1, Pointer d2);
void hash_destroy(HashTable ht);
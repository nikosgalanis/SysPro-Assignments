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
	int bucket_max_entries;
	HashFunc hash_function;
	DestroyFunc destroy_items;
};

typedef struct hash_table* HashTable;

HashEntry create_hash_entry(char* key, Pointer item);
HashTable hash_create(int size, HashFunc hash_fn, int bucket_size, DestroyFunc destroy);
void hash_insert(HashTable ht, char* key, Pointer item);
HashEntry hash_search(HashTable ht, char* name);
void hash_update(HashTable ht, char* key, Pointer new_item);
void hash_traverse(HashTable ht, VisitFunc print, Pointer d1, Pointer d2, Pointer p);
void hash_destroy(HashTable ht);
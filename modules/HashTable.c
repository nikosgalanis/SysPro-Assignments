#include <stdlib.h>
#include "HashTable.h"
#include <string.h>

HashNode create_hash_node(int bucket_size) {
	HashNode new_node = malloc(sizeof(struct hash_node));
	// Create a bucket which contains "bucketsize" bytes
	new_node->bucket = malloc(bucket_size); //TODO: change to somthing that fits exactly n
	// initialize our entries to null TODO: Maybe use an empty bool variable
	for (int j = 0; j < bucket_size / sizeof(struct hash_entry); j++) {
		new_node->bucket[0]->tree_root = NULL;			
	}
	// We are going to use that as an overflow list pointer for each bucket
	new_node->next = NULL;
	return new_node;
}

// Create an empty hash table, with pre-determined size
HashTable hash_create(int size, HashFunc hash_fn, int bucket_size) {
	HashTable ht = malloc(sizeof(*ht));
	ht->array = malloc(size * sizeof(HashNode));
	for (int i = 0; i < size; i++) {
		ht->array[i] = create_hash_node(bucket_size);
	}
	// Store the size of the ht
	ht->size = size;
	// Initially the ht is empty
	ht->items = 0;
	// Hold the bucketsize, so that is a multiple of the size of the struct.
	ht->bucket_size = (bucket_size / sizeof(struct hash_entry)) * sizeof(struct hash_entry);
	// We deferentiate the hts, by passing each hash function into the struct
	ht->hash_function = hash_fn;
	return ht;
}

// Insert a new entry in the hash table
void hash_insert(HashTable ht, HashEntry new_entry) {
	// Use the hash function to determine where our new entry is gonna go
	int hash_id = ht->hash_function(new_entry->name) % ht->size;
	HashNode requested = ht->array[hash_id];
	int entries = ht->bucket_size / sizeof(struct hash_entry);
	// Traverse all the records in this bucket until u find an empty space
	int pos;
	for (pos = 0; pos < entries; pos++) {
		// If the item is not initiallized, then we terminate our search
		// and we are ready to insert the entry
		if (requested->bucket[pos]->tree_root == NULL) 
			break;
		// If we reach the end of the node, the we check if there is another node in the list
		if (pos == entries - 1) {
			// If there is, we go on to the next node
			if (requested->next != NULL) {
				requested = requested->next;
			}
			// If there is not, then we create one, so there is space to insert the new entry
			else {
				HashNode new_node = create_hash_node(ht->bucket_size);
				requested->next = new_node;
				requested = requested->next;

			}
			// Either way, we go back to the 0-th position on the bucket.
			pos = 0;
		}
	}
	// Insert the new entry in the correct position
	requested->bucket[pos] = new_entry;
}

HashEntry hash_search(HashTable ht, char* name) {
	// Use the hash function to determine where our new entry is gonna go
	int hash_id = ht->hash_function(name) % ht->size;
	HashNode requested = ht->array[hash_id];
	int entries = ht->bucket_size / sizeof(struct hash_entry);
	// Traverse all the records in this bucket until u find the desired record
	int pos;
	// TODO: SOS, uninitialized strings, beware!
	for (pos = 0; pos < entries; pos++) {
		// If the item is not initiallized, then we terminate our search
		// and return accordingly
		if (strcmp(requested->bucket[pos]->name, name) == 0)
			return requested->bucket[pos];
		// If we reach the end of the node, the we check if there is another node in the list
		if (pos == entries - 1) {
			// If there is, we go on to the next node
			if (requested->next != NULL) {
				requested = requested->next;
			}
			// If there is not, then we've reached a dead-end, so we return NULL
			else 
				return NULL;
			// Either way, we go back to the 0-th position on the bucket.
			pos = 0;
		}
	}
	// For whatever reason we are here, something is wrong, so return NULL
	return NULL;
}
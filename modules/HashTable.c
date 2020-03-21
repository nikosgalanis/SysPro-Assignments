#include <stdlib.h>
#include <stdio.h>
#include "HashTable.h"
#include <string.h>

HashEntry create_hash_entry(char* key, Pointer item) {
	HashEntry new_entry = malloc(sizeof(*new_entry));
	new_entry->key = key; 
	new_entry->item = item;

	return new_entry;
}

// Create a node for the hash table, given its bucket size
HashNode create_hash_node(int bucket_size) {
	HashNode new_node = malloc(sizeof(*new_node));
	assert(new_node != NULL);
	// Create a bucket which contains "bucketsize" bytes
	int n_buckets = (bucket_size - sizeof(Pointer)) / sizeof(struct hash_entry);
	new_node->bucket = malloc(n_buckets * sizeof(HashEntry));
	// initialize our entries to null
	for (int j = 0; j < n_buckets; j++) {
		new_node->bucket[j] = NULL;			
	}
	// We are going to use that as an overflow list pointer for each bucket
	new_node->next = NULL;
	
	return new_node;
}

// Create an empty hash table, with pre-determined size
HashTable hash_create(int size, HashFunc hash_fn, int bucket_size, DestroyFunc destroy) {
	HashTable ht = malloc(sizeof(*ht));
	assert(ht != NULL);
	// Hold the bucketsize, so that is a multiple of the size of the struct.
	ht->bucket_size = bucket_size;
	if (bucket_size / sizeof(struct hash_entry) < 1) {
		printf("Fatal error, bucketsize too small. Exiting the monitor\n");
		//TODO: Free the memory 
		exit(EXIT_FAILURE);
	}
	ht->array = malloc(size * sizeof(HashNode));
	for (int i = 0; i < size; i++) {
		ht->array[i] = create_hash_node(ht->bucket_size);
	}
	// Store the size of the ht
	ht->size = size;
	// Initially the ht is empty
	ht->items = 0;
	// We deferentiate the hts, by passing each hash function into the struct
	ht->hash_function = hash_fn;
	ht->destroy_items = destroy;
	return ht;
}

// Insert a new entry in the hash table
void hash_insert(HashTable ht, HashEntry new_entry) {
	// Use the hash function to determine where our new entry is gonna go
	int hash_id = ht->hash_function(new_entry->key) % ht->size;
	HashNode requested = ht->array[hash_id];
	int entries = (ht->bucket_size - sizeof(Pointer)) / sizeof(struct hash_entry);
	// Traverse all the records in this bucket until u find an empty space
	int pos;
	for (pos = 0;; pos++) {
		// If the item is not initiallized, then we terminate our search
		// and we are ready to insert the entry
		if (requested->bucket[pos] == NULL) 
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
				requested = new_node;
			}
			// Either way, we go back to the 0-th position on the bucket.
			pos = -1;
		}
	}
	// Insert the new entry in the correct position
	requested->bucket[pos] = new_entry;
	ht->items++;
}

// Search for a specific entry in the ht, given the key. If search fails, null is returned 
HashEntry hash_search(HashTable ht, char* key) {
	// Use the hash function to determine where our new entry is gonna go
	int hash_id = ht->hash_function(key) % ht->size;
	HashNode requested = ht->array[hash_id];
	int entries = (ht->bucket_size - sizeof(Pointer)) / sizeof(struct hash_entry);
	// Traverse all the records in this bucket until u find the desired record
	int pos;
	for (pos = 0; pos < entries; pos++) {
		// If the item is not initiallized, then we terminate our search
		// and return accordingly
		if (requested->bucket[pos] != NULL) {
			if (strcmp(requested->bucket[pos]->key, key) == 0)
				return requested->bucket[pos];
		}
		else {
			return NULL;
		}
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
			pos = -1;
		}
	}
	// For whatever reason we are here, something is wrong, so return NULL
	return NULL;
}
// Traverse the ht, by applying a visit function to each entry
void hash_traverse(HashTable ht, VisitFunc print, Pointer d1, Pointer d2, Pointer p) {
	HashNode current;
	int entries = (ht->bucket_size - sizeof(Pointer)) / sizeof(struct hash_entry);
	for (int i = 0; i < ht->size; i++) {
		int pos = 0;
		current = ht->array[i];
		for (pos = 0;; pos++) {
			if (current->bucket[pos] != NULL) {
				char* str = current->bucket[pos]->key;
				print(current->bucket[pos], d1, d2, str, p);
			}
			if (pos == entries - 1) {
				// If there is, we go on to the next node
				if (current->next != NULL) {
					current = current->next;
				}
				// If there is not, then we've reached a dead-end, so we break
				else
					break;
				// Either way, we go back to the 0-th position on the bucket.
				pos = -1;
			}
		}
	}
}

// Free all the memory allocated by a hash table
void hash_destroy(HashTable ht) {
	HashNode current;
	int entries = (ht->bucket_size - sizeof(Pointer)) / sizeof(struct hash_entry);
	for (int i = 0; i < ht->size; i++) {
		int pos = 0;
		current = ht->array[i];
		for (pos = 0;; pos++) {
			if (current->bucket[pos] != NULL && ht->destroy_items != NULL)
				ht->destroy_items(current->bucket[pos]);
			if (pos == entries - 1) {
				// If there is, we go on to the next node
				if (current->next != NULL) {
					current = current->next;
					//TODO: possibble leak at the ->next element
				}
				// If there is not, then we've reached a dead-end, so we break
				else
					break;
				// Either way, we go back to the 0-th position on the bucket.
				pos = -1;
			}
		}
		free(ht->array[i]);
	}
	free(ht->array);
	free(ht);
}
#include <stdlib.h>
#include <stdio.h>
#include "HashTable.h"
#include <string.h>

// Special pointer to indicate an empty entry
Pointer empty = &empty;

HashEntry create_hash_entry(char* key, Pointer item) {
	HashEntry new_entry = malloc(sizeof(*new_entry));
	assert(new_entry != NULL);
	new_entry->key = key; 
	new_entry->item = item;

	return new_entry;
}

// Create a node for the hash table, given its bucket size
HashNode create_hash_node(int n_buckets) {
	HashNode new_node = malloc(sizeof(*new_node));
	assert(new_node != NULL);
	new_node->bucket = malloc(n_buckets * sizeof(HashEntry));
	// initialize our entries to null
	for (int j = 0; j < n_buckets; j++) {
		new_node->bucket[j] = create_hash_entry(empty, empty);			
	}
	// We are going to use that as an overflow list pointer for each bucket
	new_node->next = NULL;
	
	return new_node;
}

// Create an empty hash table, with pre-determined size
HashTable hash_create(int size, HashFunc hash_fn, int bucket_size, DestroyFunc destroy) {
	HashTable ht = malloc(sizeof(*ht));
	assert(ht != NULL);
	assert(size >= 0);
	// Hold the bucketsize, so that is a multiple of the size of the struct.
	ht->bucket_size = bucket_size;
	ht->bucket_max_entries = (bucket_size - sizeof(Pointer)) / sizeof(struct hash_entry);
	if (bucket_size / sizeof(struct hash_entry) < 1) {
		printf("Fatal error, bucketsize too small. Exiting the monitor\n");
		exit(EXIT_FAILURE);
	}
	// allocate space for our array
	ht->array = malloc(size * sizeof(HashNode));
	// create the first nodes of each bucket's overflow list
	for (int i = 0; i < size; i++) {
		ht->array[i] = create_hash_node(ht->bucket_max_entries);
	}
	// Store the size of the ht
	ht->size = size;
	// Initially the ht is empty
	ht->items = 0;
	// We deferentiate the hts, by passing each hash & destroy function into the struct
	ht->hash_function = hash_fn;
	ht->destroy_items = destroy;
	return ht;
}

// Insert a new entry in the hash table
void hash_insert(HashTable ht, char* key, Pointer item) {
	// Use the hash function to determine where our new entry is gonna go
	int hash_id = ht->hash_function(key) % ht->size;
	int pos;
	bool found_empty = false;
	HashNode requested = ht->array[hash_id];
	// forever running loop until we find an empty space
	while (true) {
		for (pos = 0; pos < ht->bucket_max_entries; pos++) {
			// if we reach an empty entry, break our loop
			if (requested->bucket[pos]->item == empty) {
				found_empty = true;
				break;
			}
		}
		if (found_empty)
			break;
		// if we do not have an other element in our overflow list
		if (requested->next == NULL) {
			// create a new one
			HashNode new_node = create_hash_node(ht->bucket_max_entries);
			// and assign it to our node
			requested->next = new_node;
			requested = new_node;
		} else {
			// else, just proceed to the next member of the overflow list
			requested = requested->next;
		}
	}
	// Insert the new entry in the correct position
	requested->bucket[pos]->item = item;
	requested->bucket[pos]->key = key;
	// increase the ht items by one
	ht->items++;
}
// Search for a specific entry in the ht, given the key. If search fails, null is returned 
HashEntry hash_search(HashTable ht, char* key) {
	// Use the hash function to determine where our new entry is gonna go
	int hash_id = ht->hash_function(key) % ht->size;
	HashNode requested = ht->array[hash_id];
	// run the loop until there are no more elements in the bucket's overflow list
	while (requested != NULL) {
		// traverse all the entries in the hash node, trying to find the desired one
		for (int pos = 0; pos < ht->bucket_max_entries; pos++) {
			// if the entry has something in it, proceed, else we did not find what we wanted
			if (requested->bucket[pos]->item != empty) {
				if (strcmp(requested->bucket[pos]->key, key) == 0)
					return requested->bucket[pos];
			} else {
				return NULL;
			}
		}
		// proceed to the next member of the overflow list
		requested = requested->next;
	}
	// the entry was not found, so return null
	return NULL;
}

// Traverse the ht, by applying a visit function to each entry
void hash_traverse(HashTable ht, VisitFunc visit, Pointer d1, Pointer d2, Pointer p) {
	// traverse all the buckets in our hash table
	for (int i = 0; i < ht->size; i++) {
		HashNode current = ht->array[i];
		// run the loop until there are no more elements in the bucket's overflow list
		while (current != NULL) {
			// traverse all the entries in the hash node
			for (int pos = 0; pos < ht->bucket_max_entries; pos++) {
				// if the item is not empty, apply the visit function
				if (current->bucket[pos]->item != empty) {
					char* str = current->bucket[pos]->key;
					visit(current->bucket[pos], d1, d2, str, p);
				}
			}
			// proceed to the next member of the overflow list
			current = current->next;
		}
	}
}

// Free the memory allocated by the hash table by destroying the ht
void hash_destroy(HashTable ht) {
	// we want to free all the buckets
	for (int i = 0; i < ht->size; i++) {
		HashNode current = ht->array[i];
		// traverse the overflow list
		while (current != NULL) {
			HashNode next_node = current->next;
			// destroy all the entries in the node
			for (int pos = 0; pos < ht->bucket_max_entries; pos++) {
				if (current->bucket[pos]->item != empty)
					ht->destroy_items(current->bucket[pos]->item);
				free(current->bucket[pos]);
			}
			// free the pointers to the bucket, and the bucket itself
			free(current->bucket); 
			free(current);
			// proceed to the next member of the overflow list
			current = next_node;
		}
	}
	// free the array, and the hash table it self
	free(ht->array);
	free(ht);
}
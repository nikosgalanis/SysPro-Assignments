#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GlobalStructures.h"

//============= Hepful functions needed for the queries============//


uint hash_string(void* key) {
	char* str = (char*)key;
	int h = 0, a = 33;
	for (; *str != '\0'; str++)
		h = (a * h + *str);
	return h;
}

// Compare 2 tree nodes, depending on their date
int icompare(Pointer first, Pointer second) {
	BalancedTreeEntry entry1 = (BalancedTreeEntry)first;
	BalancedTreeEntry entry2 = (BalancedTreeEntry)second;
	assert(entry1 != NULL && entry2 != NULL);
		return compare_dates(entry1->date, entry2->date);
}

// Print the hash table entry key, allong with the total entries in the dates tree
void print_all(Pointer ent, Pointer date1, Pointer date2, Pointer dummy1, Pointer dummy2) {
	HashEntry entry = (HashEntry)ent;
	if (entry != NULL) {
		Tree tree = entry->item;
		printf("For the disease %s, %d patients were recorded\n", entry->key, tree->size);
	} else {
		printf("Disease not found\n");
	}
}

// Print the hash table entry key, allong with entries on the date tree that are in the range (date1, date2)
void print_specific(Pointer ent, Pointer date1, Pointer date2, Pointer dummy1, Pointer dummy2) {
	HashEntry entry = (HashEntry)ent;
	if (entry != NULL) {
		Date* d1 = (Date*)date1; 
		Date* d2 = (Date*)date2; 
		Tree tree = entry->item;
		int g_than = total_nodes_grater_than(tree, d1, NULL, NULL) - total_nodes_grater_than(tree, d2, NULL, NULL);
		printf("For the disease %s, %d patients were recorded\n", entry->key, g_than);
	} else {
		printf("Disease not found\n");
	}
}

// Visit function for topk-Diseases query
void visit_country_with_dates(Pointer t, Pointer count, Pointer date1, Pointer date2, Pointer h) {
	// Convert the types
	BalancedTreeEntry entry = (BalancedTreeEntry)t;
	Date* d1 = (Date*)date1;
	Date* d2 = (Date*)date2;
	char* country = (char*)count;
	HashTable ht = (HashTable)h;
	Patient* p = entry->assigned_patient;
	// Ensure that we are not dealing with null entries
	if (p != NULL && entry != NULL) {
		// If the tree entry meets these criteria
		if (!strcmp(p->country, country) && (compare_dates(*d1, p->entry_date) < 0) && (compare_dates(p->entry_date, *d2) < 0)) {
			// find the disease entry in the hash table
			HashEntry hash_res = hash_search(ht, p->disease);
			// if it is already there
			if (hash_res != NULL) {
				// increase the times we've found it by 1
				int* times = hash_res->item;
				(*times)++;
				// and store this value
				hash_res->item = times;
			} else {
				// else create space for the counter, and store the entry in the hash table
				int* times = malloc(sizeof(int));
				*times = 1;
				hash_insert(ht, p->disease, times);
			}
		}
	}
}

// Similar with function above, just not invloving dates
void visit_country_without_dates(Pointer t, Pointer count, Pointer dummy1, Pointer dummy2, Pointer h) {
	BalancedTreeEntry entry = (BalancedTreeEntry)t;
	char* country = (char*)count;
	HashTable ht = (HashTable)h;
	Patient* p = entry->assigned_patient;
	if (p != NULL && entry != NULL) {
		if (!strcmp(p->country, country)) {
			HashEntry hash_res = hash_search(ht, p->disease);
			if (hash_res != NULL) {
				int* times = hash_res->item;
				(*times)++;
				hash_res->item = times;
			} else {
				int* times = malloc(sizeof(int));
				*times = 1;
				hash_insert(ht, p->disease, times);
			}
		}
	}
}

void visit_disease_with_dates(Pointer t, Pointer dis, Pointer date1, Pointer date2, Pointer h) {
	BalancedTreeEntry entry = (BalancedTreeEntry)t;
	Date* d1 = (Date*)date1;
	Date* d2 = (Date*)date2;
	char* disease = (char*)dis;
	HashTable ht = (HashTable)h;
	Patient* p = entry->assigned_patient;
	if (p != NULL && entry != NULL) {
		if (!strcmp(p->disease, disease) && (compare_dates(*d1, p->entry_date) < 0) && (compare_dates(p->entry_date, *d2) < 0)) {
			HashEntry hash_res = hash_search(ht, p->country);
			if (hash_res != NULL) {
				int* times = hash_res->item;
				(*times)++;
				hash_res->item = times;
			} else {
				int* times = malloc(sizeof(int));
				*times = 1;
				hash_insert(ht, p->country, times);
			}
		}
	}
}

void visit_disease_without_dates(Pointer t, Pointer dis, Pointer dummy1, Pointer dummy2, Pointer h) {
	BalancedTreeEntry entry = (BalancedTreeEntry)t;
	char* disease = (char*)dis;
	HashTable ht = (HashTable)h;
	Patient* p = entry->assigned_patient;
	if (p != NULL && entry != NULL) {
		if (!strcmp(p->disease, disease)) {
			HashEntry hash_res = hash_search(ht, p->country);
			if (hash_res != NULL) {
				int* times = hash_res->item;
				(*times)++;
				hash_res->item = times;
			} else {
				int* times = malloc(sizeof(int));
				*times = 1;
				hash_insert(ht, p->country, times);
			}
		}
	}
}

// Insert a hash entry (key, priority) to a heap, for topk-* queries
void insert_entry_to_heap(Pointer t, Pointer dummy1, Pointer dummy2, Pointer dummy3, Pointer h) {
	HashEntry entry = (HashEntry)t;
	Heap heap = (Heap)h;
	int priority = *(int*)entry->item;
	if (entry != NULL) {
		heap_insert(heap, priority, entry->key);
	}
}

// condition function for a tree
bool check_same_country(Pointer ent, Pointer count) {
	char* country = (char*) count;
	BalancedTreeEntry entry = (BalancedTreeEntry)ent;
	if (entry!= NULL) {
		Patient* patient = entry->assigned_patient;
		if (!strcmp(patient->country, country))
			return true;
		else 
			return false;
	} else {
		printf("Disease not found\n");
		return false;
	}
}

// condition function for a tree
bool check_if_hospitalized(Pointer ent, Pointer dummy) {
	BalancedTreeEntry entry = (BalancedTreeEntry)ent;
	if (entry != NULL) {
		Patient* patient = entry->assigned_patient;
		if (check_if_null_date(patient->exit_date))
			return true;
		else 
			return false;
	} else {
		printf("Disease not found\n");
		return false;
	}
}

// Visit function to print all the patients that are hospitalized in a tree, member of a ht
void print_hospitalized(Pointer ent, Pointer dummy1, Pointer dummy2, Pointer dis, Pointer dummy3) {
	HashEntry entry = (HashEntry)ent;
	if (entry != NULL) {
		char* disease = (char*)dis;
		BalancedTree p = entry->item;
		int result = balanced_tree_cond_traverse(p, check_if_hospitalized);
		printf("%d patients are currently being hospitalized with disease %s\n", result, disease);
	} else {
		printf("Disease not found\n");
	}
}


//=========================================Queries for the monitor======================================================//

void globalDiseaseStats(char* info) {
	// Analyse the user input
	char delim[3] = " \n";
	char* day1 = strtok(info, delim);
	char* day2 = strtok(NULL, delim);
	Date d1 = string_to_date(day1);
	Date d2 = string_to_date(day2);
	// If a begin date is provided, check for a date 2.
	if (check_if_null_date(d1) == true) {
		if (check_if_null_date(d2) == true) {
			// Print all the patients for a disease, if no dates are provided
			hash_traverse(diseaseHashTable, print_all, NULL, NULL, NULL);
		} else {
			printf("You must provide 2 dates. Use as /globalDiseaseStats [date1 date2]\n");
		}
	} else {
		if (check_if_null_date(d2) == false) {
			// Print the total n of patients for the disease, if both dates are provided
			hash_traverse(diseaseHashTable, print_specific, &d1, &d2, NULL);
		} else {
			printf("You must provide 2 dates. Use as /globalDiseaseStats [date1 date2]\n");
		}
	} 
}

void diseaseFrequency(char* info) {
	// Analyse the user input
	char delim[3] = " \n";
	char* virus = strtok(info, delim);
	char* arg2 = strtok(NULL, delim);
	char* arg3 = strtok(NULL, delim);
	char* country = strtok(NULL, delim);
	if (virus == NULL || arg2 == NULL || arg3 == NULL) {
		printf("Use as /diseaseFrequency virusName date1 date2 [country]\n");
		return;
	}
	Date d1 = string_to_date(arg2);
	Date d2 = string_to_date(arg3);
	// If there are 3 arguments, we suppose that a country is not given
	if (country == NULL) {
		HashEntry entry = hash_search(diseaseHashTable, virus);
		if (entry == NULL) {
			printf("Desired disease not found\n");
		} else {
			// Same as the previous query
			Tree tree = entry->item;
			int g_than = total_nodes_grater_than(tree, &d1, NULL, NULL) - total_nodes_grater_than(tree, &d2, NULL, NULL);
			printf("For the virus %s, %d infected were found between the 2 given dates\n", virus, g_than);
		}
	} else {
		HashEntry entry = hash_search(diseaseHashTable, virus);
		if (entry == NULL) {
			printf("Desired disease not found\n");
		} else {
			// Insert the same country as a condition to the previous function.
			BalancedTree tree = entry->item;
			int g_than = total_nodes_grater_than(tree, &d1, check_same_country, country) - total_nodes_grater_than(tree, &d2, check_same_country, country);
			printf("For the virus %s, %d infected were found between the 2 given dates in %s\n", virus, g_than, country);
		}
	}
}

void topk_Diseases(char* info) {
	char delim[3] = " \n";
	if (info == NULL) {
		printf("Use ase /topk-Diseases k country [date1 date2]\n");
		return;
	}
	int k = atoi(strtok(info, delim));
	char* country = strtok(NULL, delim);
	char* day1 = strtok(NULL, delim);
	char* day2 = strtok(NULL, delim);
	if (country == NULL) {
		printf("Use ase /topk-Diseases k country [date1 date2]\n");
		return;
	}
	Date d1 = string_to_date(day1);
	Date d2 = string_to_date(day2);
	// Create a temporary ht, to store the diseases found in this country.
	HashTable diseases_ht = hash_create(num_diseases, hash_string, 100, free);
	// If a begin date is provided, check for a day 2.
	if (check_if_null_date(d1) == true) {
		if (check_if_null_date(d2) == true) {
			// Get access to the country 
			HashEntry ent = hash_search(countryHashTable, country);
			if (ent != NULL) {
				// and its tree
				BalancedTree tree = ent->item;
				// Insert every node in the temp ht, along with how many times we've found it
				balanced_tree_traverse(tree, visit_country_without_dates, country, NULL, NULL, diseases_ht);
				// Create a temp heap
				Heap heap = create_heap(NULL);
				// Store in the heap all the temp ht items
				hash_traverse(diseases_ht, insert_entry_to_heap, NULL, NULL, heap);
				printf("For the country %s, the top %d diseases are:\n", country, k);
				// Extract k items from the heap, and print them
				for (int i = 0; i < k && i < diseases_ht->items; i++) {
					HeapEntry ent = pop(heap);
					printf("%s with %d infected\n", ent->key, ent->priority);
					free(ent);
				}
				// Free the memory allocated
				destroy_heap(heap);
			} else {
				// If the search in the hash returns null, then no such country is found
				printf("Country not found\n");
				return;
			}
		} else {
			printf("You must provide 2 dates. Use as /topk-Diseases k country [date1 date2]\n");
		}
	} else {
		if (check_if_null_date(d2) == false) {
			HashEntry ent = hash_search(countryHashTable, country);
			if (ent != NULL) {
				BalancedTree tree = ent->item;
				balanced_tree_traverse(tree, visit_country_with_dates, country, &d1, &d2, diseases_ht);
				Heap heap = create_heap(NULL);
				hash_traverse(diseases_ht, insert_entry_to_heap, NULL, NULL, heap);

				printf("For the country %s, the top %d diseases for the desired dates are:\n", country, k);
				for (int i = 0; i < k && i < diseases_ht->items; i++) {
					HeapEntry ent = pop(heap);
					printf("%s with %d infected\n", ent->key, ent->priority);
					free(ent);
				}
				destroy_heap(heap);
			} else {
				printf("Country not found\n");
				return;
			}
		} else {
			printf("You must provide 2 dates. Use as /topk-Diseases k country [date1 date2]\n");
		}
	}
	hash_destroy(diseases_ht); 
}

void topk_Countries(char* info) {
	char delim[3] = " \n";
	if (info == NULL) {
		printf("Use as /topk-Countries k disease [date1 date2]\n");
		return;
	}
	int k = atoi(strtok(info, delim));
	char* disease = strtok(NULL, delim);
	char* day1 = strtok(NULL, delim);
	char* day2 = strtok(NULL, delim);
	if (disease == NULL) {
		printf("Use as /topk-Countries k disease [date1 date2]\n");
		return;
	}
	Date d1 = string_to_date(day1);
	Date d2 = string_to_date(day2);
	HashTable countries_ht = hash_create(num_diseases, hash_string, 100, free);
	// If a begin date is provided, check for a day 2.
	if (check_if_null_date(d1) == true) {
		if (check_if_null_date(d2) == true) {
			HashEntry ent = hash_search(diseaseHashTable, disease);
			if (ent != NULL) {
				BalancedTree tree = ent->item;
				balanced_tree_traverse(tree, visit_disease_without_dates, disease, NULL, NULL, countries_ht);
				Heap heap = create_heap(NULL);
				hash_traverse(countries_ht, insert_entry_to_heap, NULL, NULL, heap);

				printf("For the disease %s, the top %d countries are:\n", disease, k);
				for (int i = 0; i < k && i < countries_ht->items; i++) {
					HeapEntry ent = pop(heap);
					printf("%s with %d infected\n", ent->key, ent->priority);
					free(ent);
				}
				destroy_heap(heap);
			} else {
				printf("Disease not found\n");
				return;
			}
		} else {
			printf("You must provide 2 dates. Use as /topk-Countries k disease [date1 date2]\n");
		}
	} else {
		if (check_if_null_date(d2) == false) {
			HashEntry ent = hash_search(diseaseHashTable, disease);
			if (ent != NULL) {
				BalancedTree tree = ent->item;
				balanced_tree_traverse(tree, visit_disease_with_dates, disease, &d1, &d2, countries_ht);
				Heap heap = create_heap(NULL);
				hash_traverse(countries_ht, insert_entry_to_heap, NULL, NULL, heap);

				printf("For the diseases %s, the top %d countries for the desired dates are:\n", disease, k);
				for (int i = 0; i < k && i < countries_ht->items; i++) {
					HeapEntry ent = pop(heap);
					printf("%s with %d infected\n", ent->key, ent->priority);
					free(ent);
				}
				destroy_heap(heap);
			} else {
				printf("Disease not found\n");
				return;
			}
		} else {
			printf("You must provide 2 dates. Use as /topk-Countries k disease [date1 date2]\n");
		}
	} 
	hash_destroy(countries_ht);
}


void insertPatientRecord(char* info) {
	// Create a new patient record
	Patient* p = create_patient(info);
	if (p == NULL) {
		printf("Use as /insertPatientRecord recordID patientFirstName patientLastName diseaseID" \
		"country entryDate [exitDate]\n");
	}
	// The key for the balanced tree will be tha patient's entry date to the hospital
	Date tree_key = p->entry_date;
	HashEntry disease_search_result = hash_search(diseaseHashTable, p->disease);
	HashEntry country_search_result = hash_search(countryHashTable, p->country);
	// If we find the entry in the hash table, then we update
	// its tree, by inserting the new patient
	if(country_search_result != NULL) {
		BalancedTreeEntry new_tree_entry = create_balanced_tree_entry(tree_key, p);
		BalancedTree result_tree = country_search_result->item;
		balanced_tree_insert(result_tree, new_tree_entry);
	}
	// If we do not find the entry, then we insert it, with a tree with only one node as a key
	else  {
		// Create a new tree and store its 1st date
		// Attention: We pass NULL as destroy func, because we do not want the patients, neither 
		// the dates to be freed, cause we have 2 trees possibly pointing in the same node
		BalancedTree result_tree = create_balanced_tree(icompare, NULL);
		hash_insert(countryHashTable, p->country, result_tree);
		BalancedTreeEntry new_tree_entry = create_balanced_tree_entry(tree_key, p);
		balanced_tree_insert(result_tree, new_tree_entry);

	}
	// Same thing about the diseases hash table
	if(disease_search_result != NULL) {
		BalancedTreeEntry new_tree_entry = create_balanced_tree_entry(tree_key, p);
		BalancedTree result_tree = disease_search_result->item;
		balanced_tree_insert(result_tree, new_tree_entry);
	}
	// If we do not find the entry, then we insert it, with an empty tree as a key
	else  {
		BalancedTree result_tree = create_balanced_tree(icompare, NULL);
		hash_insert(diseaseHashTable, p->disease, result_tree);
		BalancedTreeEntry new_tree_entry = create_balanced_tree_entry(tree_key, p);
		balanced_tree_insert(result_tree, new_tree_entry);
	}
	// Search for the patient in the patients ht. If a patient with the same id is found
	// terminate the program. Else, just insert the pointer to the patient record in the hash.
	HashEntry patient_entry = hash_search(patients, p->id);
	if (patient_entry == NULL) {
		hash_insert(patients, p->id, p);
	} else {
		printf("Fatal error. Patient with id %s already exists. Terminating the monitor\n", p->id);
		exit_monitor();
		exit(EXIT_FAILURE);
	}
}

void recordPatientExit(char* info) {
	char delim[3] = " \n";
	char* r_id = strtok(NULL, delim);
	char* exit_d = strtok(NULL, delim);
	if (r_id == NULL || exit_d == NULL) {
		printf("Use as /recordPatientExit recordID exitDate\n");
	}
	Date exit_date = string_to_date(exit_d);

	HashEntry patient_entry = hash_search(patients, r_id);
	if (patient_entry == NULL) {
		printf("Patient with id %s not found.\n", r_id);
		return;
	}
	// The patient record can be easily found in the patients ht, where a pointer to the record is kept
	Patient* patient = (Patient*)patient_entry->item;
	// Check of the patient has already exited the hospital
	if (patient->exit_date.day != 0) {
		// Update the desired info
		patient->exit_date = exit_date;
	} else {
		printf("The desired patient has already exited the hospital.\n");
	}
}

void numCurrentPatients(char* info) {
	char delim[3] = " \n";
	char* virus = strtok(info, delim);
	if (empty_string(virus)) {
		hash_traverse(diseaseHashTable, print_hospitalized, NULL, NULL, NULL);
	} else {
		HashEntry entry = hash_search(diseaseHashTable, virus);
		if (entry == NULL) {
			printf("Desired disease not found\n");
		} else {
			BalancedTree p = entry->item;
			int result = balanced_tree_cond_traverse(p, check_if_hospitalized);
			printf("%d patients are currently being hospitalized with disease %s\n", result, virus);
		}
	}
}

void exit_monitor(void) {
	// free the hash tables
	hash_destroy(diseaseHashTable);
	hash_destroy(countryHashTable);
	hash_destroy(patients);
}
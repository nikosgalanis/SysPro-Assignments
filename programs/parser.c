#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GlobalStructures.h"


int compare(Pointer first, Pointer second) {
    TreeEntry entry1 = (TreeEntry)first;
    TreeEntry entry2 = (TreeEntry)second;
    return compare_dates(entry1->date, entry2->date);
}

uint hash_strings(void* key) {
	char* str = (char*)key;
    int h = 0, a = 33;
	for (; *str != '\0'; str++)
		h = (a * h + *str);
	return h;
}

// Count how many lines there are in a file
int nlines(FILE* input) {
    int lines = 0;
    while(!feof(input)) {
        char ch = fgetc(input);
        if(ch == '\n') {
            lines++;
        }
    }
    rewind(input);
    return lines;
}

void parse_input (char* file, int num_countries, int num_diseases, int bucket_size){
    // Initialize the hash tables, with destroy functions to delete the trees, as the items are going to be bsts
    printf("Collecting the data from the input file...\n");
    diseaseHashTable = hash_create(num_diseases, hash_strings, bucket_size, balanced_tree_destroy);
    countryHashTable = hash_create(num_countries, hash_strings, bucket_size, balanced_tree_destroy);
    // Open the input file //TODO:Close it
    FILE* input = fopen("input/large.txt", "r"); //TODO: Change to real file
    int lines = nlines(input);
    // We are going to store all the pointers to the strings that we are given, in order to 
    // free them afterwards
    all_strings_from_file = malloc(lines * sizeof(char*));
    // We are going to use one extra hashtable, in order to quickly search if the patient allready exists
    // We are going to pass destroy patient as a destroy func, in order to free all the memory occupied
    // by the patients when we are done.
    patients = hash_create(lines, hash_strings, bucket_size, destroy_patient);
    char* str;
    for (int i = 0; i < lines; i++) {
        str = malloc(100); //TODO: Change
        fgets(str, 100, input);
        // store the string
        all_strings_from_file[i] = str;
        // Create a new patient record
        Patient* p = create_patient(str);
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
            BalancedTree result_tree = create_balanced_tree(compare, NULL);
            HashEntry new_hash_entry = create_hash_entry(p->country, result_tree);
            hash_insert(countryHashTable, new_hash_entry);
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
            BalancedTree result_tree = create_balanced_tree(compare, NULL);
            HashEntry new_hash_entry = create_hash_entry(p->disease, result_tree);
            hash_insert(diseaseHashTable, new_hash_entry);
            BalancedTreeEntry new_tree_entry = create_balanced_tree_entry(tree_key, p);
            balanced_tree_insert(result_tree, new_tree_entry);
        }
        // Search for the patient in the patients ht. If a patient with the same id is found
        // terminate the program. Else, just insert the pointer to the patient record in the hash.
        HashEntry patient_entry = hash_search(patients, p->id);
        if (patient_entry == NULL) {
            HashEntry new_entry = create_hash_entry(p->id, p);
            hash_insert(patients, new_entry);
        } else {
            printf("Fatal error. Patient with id %s already exists. Terminating the monitor\n", p->id);
            //TODO: Free all memory
            exit(EXIT_FAILURE);
        }
    }

}

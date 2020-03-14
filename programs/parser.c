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
    // Initialize the hash tables
    printf("bsize %d\n", bucket_size);
    diseaseHashTable = hash_create(num_diseases, hash_strings, bucket_size);
    countryHashTable = hash_create(num_countries, hash_strings, bucket_size);

    FILE* input = fopen("input/small.txt", "r"); //TODO: Change to real file
    int lines = nlines(input);
    // We are going to use one extra hashtable, in order to quickly search if the patient allready exists
    patients = hash_create(lines, hash_strings, bucket_size);
    for (int i = 0; i < lines; i++) {
        char* str = malloc(100); //TODO: Change
        fgets(str, 100, input);
        // Create a new patient record
        Patient p = create_patient(str);
        // The key for the balanced tree will be tha patient's entry date to the hospital
        Date tree_key = p.entry_date;
        HashEntry disease_search_result = hash_search(diseaseHashTable, p.disease);
        HashEntry country_search_result = hash_search(countryHashTable, p.country);
        // If we find the entry in the hash table, then we update
        // its tree, by inserting the new patient
        if(country_search_result != NULL) {
            TreeEntry new_tree_entry = create_tree_entry(tree_key, &p);
            Tree result_tree = country_search_result->item;
            tree_insert(result_tree, new_tree_entry);
        }
        // If we do not find the entry, then we insert it, with a tree with only one node as a key
        else  {
            Tree result_tree = create_tree(compare, free);
            HashEntry new_hash_entry = create_hash_entry(p.country, result_tree);
            hash_insert(countryHashTable, new_hash_entry);
            TreeEntry new_tree_entry = create_tree_entry(tree_key, &p);
            tree_insert(result_tree, new_tree_entry);

        }
        // Same thing about the diseases hash table
        if(disease_search_result != NULL) {
            TreeEntry new_tree_entry = create_tree_entry(tree_key, &p);
            Tree result_tree = disease_search_result->item;
            tree_insert(result_tree, new_tree_entry);
        }
        // If we do not find the entry, then we insert it, with an empty tree as a key
        else  {
            Tree result_tree = create_tree(compare, free);
            HashEntry new_hash_entry = create_hash_entry(p.disease, result_tree);
            hash_insert(diseaseHashTable, new_hash_entry);
            TreeEntry new_tree_entry = create_tree_entry(tree_key, &p);
            tree_insert(result_tree, new_tree_entry);
        }
        // Search for the patient in the patients ht. If a patient with the same id is found
        // terminate the program. Else, just insert the pointer to the patient record in the hash.

        HashEntry patient_entry = hash_search(patients, p.id);
        if (patient_entry == NULL) {
            HashEntry new_entry = create_hash_entry(p.id, &p);
            hash_insert(patients, new_entry);
        } else {
            printf("Fatal error. Patient with id %s already exists. Terminating the monitor\n", p.id);
            //TODO: Free all memory
            exit(EXIT_FAILURE);
        }
    }


}

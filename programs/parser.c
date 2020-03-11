#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GlobalStructures.h"


void parse_input (char* file, int num_countries, int num_diseases, int bucket_size){
    // Initialize the hash tables
    diseaseHashTable = hash_create(num_diseases, hash_strings, bucket_size);
    countryHashTable = hash_create(num_countries, hash_strings, bucket_size);


    FILE* input = fopen("../inputs/in", "w+"); //TODO: Change to real file
    int lines = nlines(input);
    for (int i = 0; i < lines; i++) {
        char* str = malloc(100); //TODO: Change
        fscanf(input, "%s\n", &str);
        // Create a new patient record
        Patient p = create_patient(str);
        // The key for the balanced tree will be tha patient's entry date to the hospital
        Date tree_key = p.entry_date;
        HashEntry country_search_result = hash_search(diseaseHashTable, p.disease);
        HashEntry diseaase_search_result = hash_search(countryHashTable, p.country);
        // If we find the entry in the hash table, then we update
        // its tree, by inserting the new patient
        if(country_search_result != NULL) {
            TreeEntry new_tree_entry = create_tree_entry(p.entry_date, &p);
            Tree result_tree = country_search_result->item;
            tree_insert(result_tree, new_tree_entry);
        }
        // If we do not find the entry, then we insert it, with an empty tree as a key
        else  {
            Tree result_tree = create_tree(compare_dates, free);
            HashEntry new_hash_entry = create_hash_entry(p.country, result_tree);
            hash_insert(countryHashTable, new_hash_entry);
        }
       // Same thing about the diseases hash table
        if(diseaase_search_result != NULL) {
            TreeEntry new_tree_entry = create_tree_entry(p.entry_date, &p);
            Tree result_tree = diseaase_search_result->item;
            tree_insert(result_tree, new_tree_entry);
        }
        // If we do not find the entry, then we insert it, with an empty tree as a key
        else  {
            Tree result_tree = create_tree(compare_dates, free);
            HashEntry new_hash_entry = create_hash_entry(p.country, result_tree);
            hash_insert(countryHashTable, new_hash_entry);
        }
    }


}

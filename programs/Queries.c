#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GlobalStructures.h"

void insertPatientRecord(char* info) {
    char delim[2] = " ";
    char* query = strtok(info, delim);
    if (strcmp(query, "/insertPatientRecord")) {
        printf("Unexpected error occured during the query\n");
        return;
    }    
    char* id = strtok(NULL, delim);
    char* first_name = strtok(NULL, delim);
    char* last_name = strtok(NULL, delim);
    char* disease = strtok(NULL, delim);
    char* country = strtok(NULL, delim);
    char* entry_date = strtok(NULL, delim);
    char* exit_date = strtok(NULL, delim);
    Date entryd = string_to_date(entry_date);
    Date exitd = string_to_date(exit_date);

    // Free the initial string so we do not have leaks
    free(info); //TODO: Propably change
    // Create and return the desired struct
    Patient p = {id, first_name, last_name, disease, country, entryd, exitd};

    // The key for the balanced tree will be tha patient's entry date to the hospital
    Date tree_key = p.entry_date;
    HashEntry country_search_result = hash_search(diseaseHashTable, p.disease);
    HashEntry diseaase_search_result = hash_search(countryHashTable, p.country);
    // If we find the entry in the hash table, then we update
    // its tree, by inserting the new patient
    if(country_search_result != NULL) {
        TreeEntry new_tree_entry = create_tree_entry(tree_key, &p);
        Tree result_tree = country_search_result->item;
        tree_insert(result_tree, new_tree_entry);
    }
    // If we do not find the entry, then we insert it, with an empty tree as a key
    else  {
        Tree result_tree = create_tree(compare, free);
        HashEntry new_hash_entry = create_hash_entry(p.country, result_tree);
        hash_insert(countryHashTable, new_hash_entry);
    }
    // Same thing about the diseases hash table
    if(diseaase_search_result != NULL) {
        TreeEntry new_tree_entry = create_tree_entry(tree_key, &p);
        Tree result_tree = diseaase_search_result->item;
        tree_insert(result_tree, new_tree_entry);
    }
    // If we do not find the entry, then we insert it, with an empty tree as a key
    else  {
        Tree result_tree = create_tree(compare, free);
        HashEntry new_hash_entry = create_hash_entry(p.country, result_tree);
        hash_insert(countryHashTable, new_hash_entry);
    }
    // Search for the patient in the patients ht. If a patient with the same id is found
    // terminate the program. Else, just insert the pointer to the patient record in the hash.
    HashEntry patient_entry = hash_search(patients, p.id);
    if (patient_entry == NULL) {
        HashEntry new_entry = create_hash_entry(p.id, &p);
        hash_insert(patients, new_entry);
    } else {
        printf("Fatal error. Patient with id %s already exists. Terminating the monitor\n");
        //TODO: Free all memory
        exit(EXIT_FAILURE);
    }

}
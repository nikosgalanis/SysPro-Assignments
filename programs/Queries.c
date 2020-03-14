#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GlobalStructures.h"

//============= Hepful functions needed for the queries============//

// Compare 2 tree nodes, depending on their date
int icompare(Pointer first, Pointer second) {
    TreeEntry entry1 = (TreeEntry)first;
    TreeEntry entry2 = (TreeEntry)second;
    return compare_dates(entry1->date, entry2->date);
}

// Print the hash table entry key, allong with the total entries in the dates tree
void print_all(HashEntry entry, Date* d1, Date* d2) {
	Tree tree = entry->item;
	printf("%s %d\n", entry->key, tree->size);
}

// Print the hash table entry key, allong with entries on the date tree that are in the range (date1, date2)
void print_specific(HashEntry entry, Date* d1, Date* d2) {
    Tree tree = entry->item;
    int g_than = total_nodes_grater_than(tree, d1) - total_nodes_grater_than(tree, d2);
	printf("%s %d\n", entry->key, g_than);
}

void insertPatientRecord(char* info) {
    char delim[2] = " ";
    // Use strtok_r to keep the rest of the string in order to initialize the patient, as it is given
    // in the exaxt same order as before.
    char* query = strtok_r(info, delim, &info);
    if (strcmp(query, "/insertPatientRecord")) {
        printf("Unexpected error occured during the query\n");
        return;
    }
    // Create the patient record with the new string.
    Patient p = create_patient(info);

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
        Tree result_tree = create_tree(icompare, free);
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
        Tree result_tree = create_tree(icompare, free);
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
        printf("Fatal error. Patient with id %s already exists. Terminating the monitor\n", p.id);
        //TODO: Free all memory
        exit(EXIT_FAILURE);
    }
}

void recordPatientExit(char* info) {
    char delim[2] = " ";
    // Use strtok_r to keep the rest of the string in order to initialize the patient, as it is given
    // in the exaxt same order as before.
    char* query = strtok(info, delim);
    char* r_id = strtok(NULL, delim);
    char* exit_d = strtok(NULL, delim);
    Date exit_date = string_to_date(exit_d);

    if (strcmp(query, "/recordPatientExit")) {
        printf("Unexpected error occured during the query\n");
        return;
    }
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
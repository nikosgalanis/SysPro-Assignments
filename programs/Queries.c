#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GlobalStructures.h"

//============= Hepful functions needed for the queries============//

// Compare 2 tree nodes, depending on their date
int icompare(Pointer first, Pointer second) {
    BalancedTreeEntry entry1 = (BalancedTreeEntry)first;
    BalancedTreeEntry entry2 = (BalancedTreeEntry)second;
    return compare_dates(entry1->date, entry2->date);
}

// Print the hash table entry key, allong with the total entries in the dates tree
void print_all(Pointer ent, Pointer date1, Pointer date2, Pointer dummy1, Pointer dummy2) {
    HashEntry entry = (HashEntry)ent;
    if (entry != NULL) {
        Tree tree = entry->item;
        printf("%s %d\n", entry->key, tree->size);
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
        printf("%s %d\n", entry->key, g_than);
    } else {
        printf("Disease not found\n");
    }
}

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

// Print the hash table entry key, allong with the total entries in the dates tree
void get_topk_countries_no_dates(Pointer ent, Pointer date1, Pointer date2, Pointer dummy, Pointer h) {
    Heap heap = (Heap)h;
    HashEntry entry = (HashEntry)ent;
    if (entry != NULL) {
        Tree tree = entry->item;
        heap_insert(heap, tree->size, entry->key);
    } else {
        printf("Disease not found\n");
    }
}

// Print the hash table entry key, allong with entries on the date tree that are in the range (date1, date2)
void get_topk_countries_with_dates(Pointer ent, Pointer date1, Pointer date2, Pointer dummy, Pointer h) {
    HashEntry entry = (HashEntry)ent;
    Heap heap = (Heap)h;
    if (entry != NULL) {
        Date* d1 = (Date*)date1; 
        Date* d2 = (Date*)date2; 
        Tree tree = entry->item;
        int g_than = total_nodes_grater_than(tree, d1, NULL, NULL) - total_nodes_grater_than(tree, d2, NULL, NULL);
        heap_insert(heap, g_than, entry->key);
    } else {
        printf("Disease not found\n");
    }
}

void print_hospitalized(Pointer ent, Pointer dummy1, Pointer dummy2, Pointer dis) {
    HashEntry entry = (HashEntry)ent;
    if (entry != NULL) {
        char* disease = (char*)dis;    
        BalancedTree p = entry->item;
        int result = balanced_tree_traverse(p, check_if_hospitalized);
        printf("%d patients are currently being hospitalized with disease %s\n", result, disease);
    } else {
        printf("Disease not found\n");
    }
}
//===============Queries for the monitor========================//

void globalDiseaseStats(char* info) {
    // Analyse the user input
    char delim[2] = " ";
    char* day1 = strtok(info, delim);
    char* day2 = strtok(NULL, delim);
    Date d1 = string_to_date(day1);
    Date d2 = string_to_date(day2);
    // If a begin date is provided, check for a day 2.
    if (check_if_null_date(d1) == true) {
        if (check_if_null_date(d2) == true) {
            // Print the total n of patients for the disease
            hash_traverse(diseaseHashTable, print_all, NULL, NULL, NULL);
        } else {
            printf("You must provide 2 dates. Use as /globalDiseaseStats [date1 date2]\n");
        }
    } else {
        if (check_if_null_date(d2) == false) {
            hash_traverse(diseaseHashTable, print_specific, &d1, &d2, NULL);
        } else {
            printf("You must provide 2 dates. Use as /globalDiseaseStats [date1 date2]\n");
        }
    } 
}

void diseaseFrequency(char* info) {
    // Analyse the user input
    char delim[2] = " ";
    char* virus = strtok(info, delim);
    char* arg2 = strtok(NULL, delim);
    char* arg3 = strtok(NULL, delim);
    Date d1 = string_to_date(arg2);
    Date d2 = string_to_date(arg3);
    char* country = strtok(NULL, delim);
    // If there are 3 arguments, we suppose that a country is not given
    if (country == NULL) {
        HashEntry entry = hash_search(diseaseHashTable, virus);
        if (entry == NULL) {
            printf("Desired disease not found\n");
        } else {
            Tree tree = entry->item;
            int g_than = total_nodes_grater_than(tree, &d1, NULL, NULL) - total_nodes_grater_than(tree, &d2, NULL, NULL);
            printf("For the virus %s, %d infected were found between the 2 given dates\n", virus, g_than);
        }
    } else {
        HashEntry entry = hash_search(diseaseHashTable, virus);
        if (entry == NULL) {
            printf("Desired disease not found\n");
        } else {
            BalancedTree tree = entry->item;
            int g_than = total_nodes_grater_than(tree, &d1, check_same_country, country) - total_nodes_grater_than(tree, &d2, check_same_country, country);
            printf("For the virus %s, %d infected were found between the 2 given dates in %s\n", virus, g_than, country);
        }
    }
}

void topk_diseases(char* info) {
    char delim[2] = " ";
    int k = atoi(strtok(info, delim));
    char* country = strtok(NULL, delim);
    char* day1 = strtok(info, delim);
    char* day2 = strtok(NULL, delim);
    Date d1 = string_to_date(day1);
    Date d2 = string_to_date(day2);
    Heap heap = create_heap(free);
    // If a begin date is provided, check for a day 2.
    if (check_if_null_date(d1) == true) {
        if (check_if_null_date(d2) == true) {
            HashEntry country_entry = hash_search(countryHashTable, country);
            printf("For the country %s, the top %d diseases are:\n", country, k);
            for (int i = 0; i < k; i++) {

            }
        } else {
            printf("You must provide 2 dates. Use as /globalDiseaseStats [date1 date2]\n");
        }
    } else {
        if (check_if_null_date(d2) == false) {
            hash_traverse(diseaseHashTable, get_topk_countries_with_dates, &d1, &d2, heap);
        } else {
            printf("You must provide 2 dates. Use as /globalDiseaseStats [date1 date2]\n");
        }
    } 
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
    Patient* p = create_patient(info);

    // The key for the balanced tree will be tha patient's entry date to the hospital
    Date tree_key = p->entry_date;
    HashEntry country_search_result = hash_search(diseaseHashTable, p->disease);
    HashEntry disease_search_result = hash_search(countryHashTable, p->country);
    // If we find the entry in the hash table, then we update
    // its tree, by inserting the new patient
    if(country_search_result != NULL) {
        BalancedTreeEntry new_tree_entry = create_balanced_tree_entry(tree_key, p);
        BalancedTree result_tree = country_search_result->item;
        balanced_tree_insert(result_tree, new_tree_entry);
    }
    // If we do not find the entry, then we insert it, with an empty tree as a key
    else  {
        BalancedTree result_tree = create_balanced_tree(icompare, free);
        HashEntry new_hash_entry = create_hash_entry(p->country, result_tree);
        hash_insert(countryHashTable, new_hash_entry);
    }
    // Same thing about the diseases hash table
    if(disease_search_result != NULL) {
        BalancedTreeEntry new_tree_entry = create_balanced_tree_entry(tree_key, p);
        Tree result_tree = disease_search_result->item;
        balanced_tree_insert(result_tree, new_tree_entry);
    }
    // If we do not find the entry, then we insert it, with an empty tree as a key
    else  {
        BalancedTree result_tree = create_balanced_tree(icompare, free);
        HashEntry new_hash_entry = create_hash_entry(p->country, result_tree);
        hash_insert(countryHashTable, new_hash_entry);
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

void numCurrentPatients(char* info) {
    char delim[2] = " ";
    char* virus = strtok(info,delim);
    printf("%s\n", virus);
    if (empty_string(virus)) {
        hash_traverse(diseaseHashTable, print_hospitalized, NULL, NULL, NULL);
    } else {
        HashEntry entry = hash_search(diseaseHashTable, virus);
        if (entry == NULL) {
            printf("Desired disease not found\n");
        } else {
            BalancedTree p = entry->item;
            int result = balanced_tree_traverse(p, check_if_hospitalized);
            printf("%d patients are currently being hospitalized with disease %s\n", result, virus);
        }
    }
}

void exit_monitor(void) {
    // free the hash tables
    hash_destroy(diseaseHashTable);
    hash_destroy(countryHashTable);
    hash_destroy(patients);
    // // and the pointers to them
    // free(diseaseHashTable);
    // free(countryHashTable);
    // free(patients);
    
    // free the strings
    for (int i = 0; i < lines; i++) {
        free(all_strings_from_file[i]);
    }
    // // and the pointer to their array
    // free(all_strings_from_file);
}
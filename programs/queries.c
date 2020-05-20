#include "Queries.h"

//=================================== Hepful functions needed for the queries ===============================//

// Compare function for strings
int compare_ids (Pointer a, Pointer b) {
	return strcmp((char*) a, (char*)b);
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

bool check_bigger_entry_date(Pointer p, Pointer d, Pointer dummy1, Pointer dummy2) {
	Patient* patient = (Patient*)p;
	Date* date = (Date*)d;
	return (compare_dates(patient->entry_date, *date) > 0);

}

bool check_bigger_exit_date(Pointer p, Pointer d, Pointer dummy1, Pointer dummy2) {
	Patient* patient = (Patient*)p;
	Date* date = (Date*)d;
	return (compare_dates(patient->exit_date, *date) > 0);

}

bool check_age_group(Pointer p, Pointer a, Pointer d1, Pointer d2) {
	Patient* patient = (Patient*)p;
	Date* date1 = (Date*)d1;
	Date* date2 = (Date*)d2;
	int age = atoi((char*)a);
	return (patient->age < age && compare_dates(patient->entry_date, *date1) > 0 && compare_dates(*date2, patient->entry_date));
}
//========================================= Queries for the monitor =========================================//

// Given a patient record, we want to mark his exit date. On success we return true, otherwise false
bool recordPatientExit(char* info, HashTable patients, char* exit_d) {
	char delim[3] = " \n";
	char* r_id = strtok(info, delim);
	if (r_id == NULL || exit_d == NULL) {
		return false;
	}
	// Mark the exit date
	Date exit_date = string_to_date(exit_d);
	// Search for the patient
	HashEntry patient_entry = hash_search(patients, r_id);
	if (patient_entry == NULL) {
		return false;
	}
	// The patient record can be easily found in the patients ht, where a pointer to the record is kept
	Patient* patient = (Patient*)patient_entry->item;
	// Check of the patient has already exited the hospital
		if (check_valid_dates(patient->entry_date, exit_date)) {
			// Update the desired info
			patient->exit_date = exit_date;
			return true;
		} else {
			return false;
		}
}

// This query is called by each worker for each country, thus the country argument will always be present
int disease_frequency(char* virus, char* arg2, char* arg3, char* country, HashTable diseases_hash) {

	Date d1 = string_to_date(arg2);
	Date d2 = string_to_date(arg3);
	if (!check_valid_dates(d1, d2)) {
		fprintf(stderr, "Wrong dates\n");
		return FAILED;
	}
	HashEntry entry = hash_search(diseases_hash, virus);
	if (entry == NULL) {
		printf("Desired disease not found\n");
		return FAILED;
	} else {
		// Insert the same country as a condition to the previous function.
		BalancedTree tree = entry->item;
		int g_than = total_nodes_grater_than(tree, &d1, check_same_country, country) - total_nodes_grater_than(tree, &d2, check_same_country, country);
		// printf("For the virus %s, %d infected were found between the 2 given dates in %s\n", virus, g_than, country);
		// printf("%s %d\n", virus, g_than);
		return g_than;
	}
}

char* search_patient_record(char* r_id, HashTable patients) {
	HashEntry result = hash_search(patients, r_id);
	if (result == NULL) 
		return;
	Patient* p = result->item;
	char* entry_date = date_to_string(p->entry_date);
	char* exit_date = date_to_string(p->exit_date);
	char* patient = malloc((sizeof(*p) + 10) * strlen(patient));
	snprintf(patient, "%s %s %s %s %s %s %s", r_id, p->first_name, p->last_name, p->disease, p->age, entry_date, exit_date);
	return patient;
}

char* num_patient_admissions(char* disease, char* arg2, char* arg3, char* country, HashTable diseases_hash) {
	// Convert the input strings to dates
	Date d1 = string_to_date(arg2);
	Date d2 = string_to_date(arg3);
	BalancedTree disease_tree = hash_search(diseases_hash, disease)->item;
	// If there is no such entry in the disease ht, then we do not have any patients here
	if (disease_tree == NULL) {
		return 0;
	}
	// all the ones that are after date 2, except those that are after date 1
	int res = balanced_tree_cond_traverse(disease_tree, check_bigger_entry_date, &d1, NULL, NULL) - balanced_tree_cond_traverse(disease_tree, check_bigger_entry_date, &d2, NULL, NULL);
	char* to_return = malloc(strlen(country) + strlen(itoa(res)) + 3);
	snprintf(to_return, "%s %d\n", country, res);
	return to_return;
}

char* num_patient_discharges(char* disease, char* arg2, char* arg3, char* country, HashTable diseases_hash) {
	// Convert the input strings to dates
	Date d1 = string_to_date(arg2);
	Date d2 = string_to_date(arg3);
	BalancedTree disease_tree = hash_search(diseases_hash, disease)->item;
	// If there is no such entry in the disease ht, then we do not have any patients here
	if (disease_tree == NULL) {
		return 0;
	}
	// all the ones that are after date 2, except those that are after date 1
	int res = balanced_tree_cond_traverse(disease_tree, check_bigger_exit_date, &d1, NULL, NULL) - balanced_tree_cond_traverse(disease_tree, check_bigger_exit_date, &d2, NULL, NULL);
	char* to_return = malloc(strlen(country) + strlen(itoa(res)) + 3);
	snprintf(to_return, "%s %d\n", country, res);
	return to_return;
}

void topk_age_ranges(int k, char* country, char* disease, char* day1, char* day2, HashTable diseases_hash) { //TODO: Fix the return type
	// Convert the input strings to dates
	Date d1 = string_to_date(day1);
	Date d2 = string_to_date(day2);
	BalancedTree disease_tree = hash_search(diseases_hash, disease)->item;
	int age_gr1 = balanced_tree_cond_traverse(disease_tree, check_age_group, "20", &d1, &d2);
	int age_gr2 = balanced_tree_cond_traverse(disease_tree, check_age_group, "40", &d1, &d2);
	int age_gr3 = balanced_tree_cond_traverse(disease_tree, check_age_group, "60", &d1, &d2);
	int age_gr4 = balanced_tree_cond_traverse(disease_tree, check_age_group, "120", &d1, &d2);
	int total = age_gr1 + age_gr2 + age_gr3 + age_gr4;
	Heap heap = create_heap(NULL);
	heap_insert(heap, age_gr1, "0-20");
	heap_insert(heap, age_gr2, "20-40");
	heap_insert(heap, age_gr3, "40-60");
	heap_insert(heap, age_gr4, "60+");

	for (int i = 0; i < k && i < 4; i++) {
		HeapEntry ent = pop(heap);
		printf("%s: %d%%\n", ent->key, 100 * ent->priority / total);
		free(ent);
	}
	destroy_heap(heap);
}

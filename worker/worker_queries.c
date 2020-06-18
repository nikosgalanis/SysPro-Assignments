#include "Queries.h"

//=================================== Hepful functions needed for the queries ===============================//

// Compare function for strings
int compare_ids (Pointer a, Pointer b) {
	return strcmp((char*) a, (char*)b);
}
// condition function for a tree
bool check_same_country(Pointer ent, Pointer count, Pointer dummy1, Pointer dummy2) {
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

bool check_bigger_entry_date(Pointer ent, Pointer d, Pointer country, Pointer dummy2) {
	BalancedTreeEntry entry = (BalancedTreeEntry)ent;
	Patient* patient = entry->assigned_patient;
	Date* date = (Date*)d;
	return (compare_dates(patient->entry_date, *date) > 0 && !strcmp(country, patient->country));

}

bool check_bigger_exit_date(Pointer ent, Pointer d, Pointer country, Pointer dummy2) {
	BalancedTreeEntry entry = (BalancedTreeEntry)ent;
	Patient* patient = entry->assigned_patient;
	Date* date = (Date*)d;
	return (compare_dates(patient->exit_date, *date) > 0) && !strcmp(country, patient->country);

}

bool check_age_group(Pointer ent, Pointer a, Pointer d1, Pointer d2) {
	BalancedTreeEntry entry = (BalancedTreeEntry)ent;
	Patient* patient = entry->assigned_patient;
	Date* date1 = (Date*)d1;
	Date* date2 = (Date*)d2;
	int age = atoi((char*)a);
	return (patient->age < age && compare_dates(patient->entry_date, *date1) > 0 && compare_dates(*date2, patient->entry_date));
}
//========================================= Queries for the monitor =========================================//

// Given a patient record, we want to mark his exit date. On success we return true, otherwise false
bool recordPatientExit(char* info, HashTable patients, char* exit_str) {
	char* exit_d = strdup(exit_str);
	char delim[3] = " \n";
	char* r_id = strtok(info, delim);
	if (r_id == NULL || exit_d == NULL) {
		return false;
	}
	// Mark the exit date
	Date exit_date = string_to_date(exit_d);
	free(exit_d);
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
	char* a2 = strdup(arg2);
	char* a3 = strdup(arg3);

	Date d1 = string_to_date(a2);
	Date d2 = string_to_date(a3);
	free(a2); free(a3);
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
		return g_than;
	}
}

char* search_patient_record(char* r_id, HashTable patients) {
	// search for the wanted record in the patients hash
	HashEntry result = hash_search(patients, r_id);
	// if we do not find it just return NULL
	if (result == NULL) 
		return "-";
	// else, construct the appropriate string to store the patient record
	Patient* p = result->item;
	char* entry_date = date_to_string(p->entry_date);
	char* exit_date = date_to_string(p->exit_date);
	char* patient = malloc((sizeof(*p) + 10) * sizeof(*patient));
	snprintf(patient, (sizeof(*p) + 10), "%s %s %s %s %d %s %s", r_id, p->first_name, p->last_name, p->disease, p->age, entry_date, exit_date);
	// return that string
	return patient;
}

char* num_patient_admissions(char* disease, char* arg2, char* arg3, char* country, HashTable diseases_hash) {
	// Convert the input strings to dates
	char* a2 = strdup(arg2);
	char* a3 = strdup(arg3);
	Date d1 = string_to_date(a2);
	Date d2 = string_to_date(a3);
	free(a2); free(a3);
	BalancedTree disease_tree = hash_search(diseases_hash, disease)->item;
	// If there is no such entry in the disease ht, then we do not have any patients here
	if (disease_tree == NULL) {
		return NULL;
	}
	// all the ones that are after date 2, except those that are after date 1
	int res = balanced_tree_cond_traverse(disease_tree, check_bigger_entry_date, &d1, country, NULL) - balanced_tree_cond_traverse(disease_tree, check_bigger_entry_date, &d2, country, NULL);

	int len = strlen(country) + strlen(itoa(res)) + 3;
	char* to_return = malloc(len * sizeof(*to_return));
	sprintf(to_return, "%15s %d\n", country, res);
	return to_return;
}

char* num_patient_discharges(char* disease, char* arg2, char* arg3, char* country, HashTable diseases_hash) {
	// Convert the input strings to dates
	char* a2 = strdup(arg2);
	char* a3 = strdup(arg3);
	Date d1 = string_to_date(a2);
	Date d2 = string_to_date(a3);
	free(a2); free(a3);
	BalancedTree disease_tree = hash_search(diseases_hash, disease)->item;
	// If there is no such entry in the disease ht, then we do not have any patients here
	if (disease_tree == NULL) {
		return NULL;
	}
	// all the ones that are after date 2, except those that are after date 1
	int res = balanced_tree_cond_traverse(disease_tree, check_bigger_exit_date, &d1, country, NULL) - balanced_tree_cond_traverse(disease_tree, check_bigger_exit_date, &d2, country, NULL);
	int len = strlen(country) + strlen(itoa(res)) + 3;
	char* to_return = malloc(len * sizeof(*to_return));
	sprintf(to_return, "%15s %d\n", country, res);
	return to_return;
}

void topk_age_ranges(int k, char* country, char* disease, char* day1, char* day2, HashTable diseases_hash, int writing) {
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
	// define how many times we will write in the pipes, and send this number so the parent is aware too
	
	int k_ret = (k < 4) ? k : 4;
	char* result = itoa(k_ret);
	write_to_socket(writing, result, strlen(result));
	char* ret = malloc(13 * sizeof(*ret));
	for (int i = 0; i <k_ret; i++) {
		HeapEntry ent = pop(heap);
		snprintf(ret, 13,"%5s: %3d%%\n", ent->key, 100 * ent->priority / total); 
        write_to_socket(writing, ret, strlen(ret));
		free(ent);
	}
	free(ret);
	destroy_heap(heap);
}

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
int disease_frequency(char* info, HashTable diseases_hash) {
	// Analyse the user input
	char delim[3] = " \n";
	char* virus = strtok(info, delim);
	char* arg2 = strtok(NULL, delim);
	char* arg3 = strtok(NULL, delim);
	char* country = strtok(NULL, delim);
	if (virus == NULL || arg2 == NULL || arg3 == NULL) {
		fprintf(stderr, "Use as /diseaseFrequency virusName date1 date2 [country]\n");
		return FAILED;
	}
	if (country == NULL) {
		fprintf(stderr, "Error: No country provided\n");
		return FAILED;
	}
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
		return NULL;
	Patient* p = result->item;
	char* entry_date = date_to_string(p->entry_date);
	char* exit_date = date_to_string(p->exit_date);
	// compute the string length that we are gonna allocate and return. We want to be as accurate ass possible, because of the predetermined buff size.
	int string_length = strlen(r_id) + strlen(p->first_name) + strlen(p->last_name) + strlen(p->disease) + strlen(entry_date) + strlen(exit_date) + 8;
	char* patient = malloc(string_length * sizeof(*patient));
	// Smart way to concat many strings in C
	snprintf(patients, "%s %s %s %s %s %s %s", r_id, p->first_name, p->last_name, p->disease, p->age, entry_date, exit_date);
	return patient;
}
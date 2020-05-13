#include "common_types.h"
#include "HashTable.h"
#include "BalancedTree.h"
#include "Patient.h"
#include <dirent.h> 
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

void recordPatientExit(char* info) {
	char delim[3] = " \n";
	char* r_id = strtok(info, delim);
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
	//if (check_if_null_date(patient->exit_date)) {
		if (check_valid_dates(patient->entry_date, exit_date)) {
			// Update the desired info
			patient->exit_date = exit_date;
			printf("Record updated\n");
		} else {
			printf("Exit day before entry date\n");
			return;
		}
	//} else {
	// 	printf("The desired patient has already exited the hospital.\n");
	// }
}

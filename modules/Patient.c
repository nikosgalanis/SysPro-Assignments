#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Patient.h"
#include "Dates.h"

extern Pointer empty;
// Given a string, create a new patient record
Patient* create_patient(char* str, char* country, char* entry_date) {
	Patient* p = malloc(sizeof(*p));
	assert(p != NULL);
	// Our delimiter will be the space character
	char delim[3] = " \n";
	
	char* id = strdup(strtok(str, delim));
	char* action = strdup(strtok(NULL, delim));
	// just ignore the action, we've checked it earlier
	free(action);
	char* first_name = strdup(strtok(NULL, delim));
	char* last_name = strdup(strtok(NULL, delim));
	char* disease = strdup(strtok(NULL, delim));
	int age = atoi(strtok(NULL, delim));
	if (id == NULL || first_name == NULL || last_name == NULL
	 || disease == NULL || country == NULL || entry_date == NULL ||
	 	age < 0 || age > 120) {
		 free(p);
		 return NULL;
	 }
	Date entry = string_to_date(entry_date);

	p->id = id;
	p->first_name = first_name;
	p->last_name = last_name;
	p->disease = disease;
	p->country = country;
	p->age = age;
	p->entry_date = entry;
	// the exit day is null for the time being
	p->exit_date = string_to_date("-");
	// Return the desired struct
	return p;
}

// Destroy a patient, in order to free the allocated memory
void destroy_patient(Pointer p) {
	Patient* patient = (Patient*)p;
	if (patient != empty && patient != NULL) {
		free (patient->id);
		free(patient->first_name);
		free(patient->last_name);
		free(patient->disease);
		free(patient->country);
		
		free(patient);
	}
}
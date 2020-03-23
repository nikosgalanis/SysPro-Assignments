#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Patient.h"
#include "Dates.h"

extern Pointer empty;
// Given a string, create a new patient record
Patient* create_patient(char* str) {
    Patient* p = malloc(sizeof(*p));
    assert(p != NULL);
    // Our delimiter will be the space character
    char delim[3] = " \n";
    
    char* id = strdup(strtok(str, delim));
    char* first_name = strdup(strtok(NULL, delim));
    char* last_name = strdup(strtok(NULL, delim));
    char* disease = strdup(strtok(NULL, delim));
    char* country = strdup(strtok(NULL, delim));
    char* entry_date = strdup(strtok(NULL, delim));
    char* exit_date = strdup(strtok(NULL, delim));
    Date entry = string_to_date(entry_date);
    Date exitd = string_to_date(exit_date);
    p->id = id;
    p->first_name = first_name;
    p->last_name = last_name;
    p->disease = disease;
    p->country = country;
    p->entry_date = entry;
    p->exit_date = exitd;
    // free the temp strings allocated
    free(entry_date);
    free(exit_date);
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
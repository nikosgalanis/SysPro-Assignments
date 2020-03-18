#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Patient.h"
#include "Dates.h"

// Given a string, create a new patient record
Patient* create_patient(char* str) {
    Patient* p = malloc(sizeof(*p));
    // Our delimiter will be the space character
    char delim[2] = " ";
    
    char* id = strtok(str, delim);
    char* first_name = strtok(NULL, delim);
    char* last_name = strtok(NULL, delim);
    char* disease = strtok(NULL, delim);
    char* country = strtok(NULL, delim);
    char* entry_date = strtok(NULL, delim);
    char* exit_date = strtok(NULL, delim);
    Date entry = string_to_date(entry_date);
    Date exitd = string_to_date(exit_date);
    p->id = id;
    p->first_name = first_name;
    p->last_name = last_name;
    p->disease = disease;
    p->country = country;
    p->entry_date = entry;
    p->exit_date = exitd;
    // // Free the initial string so we do not have leaks
    // free(str);
    // Return the desired struct
    return p;
}

void destroy_patient(Pointer p) {
    Patient* patient = (Patient*)p;
    free(patient);
}
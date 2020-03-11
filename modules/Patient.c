#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Patient.h"
#include "Dates.h"

// Given a string, create a new patient record
Patient create_patient(char* str) {
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
    Date exit = string_to_date(exit_date);

    // Free the initial string so we do not have leaks
    free(str);
    // Create and return the desired struct
    Patient new_patient = {id, first_name, last_name, disease, country, entry, exit};
    return new_patient;
}
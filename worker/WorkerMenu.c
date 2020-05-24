#include "common_types.h"
#include "WorkerMenu.h"
#include "common_functions.h"

//TODO: The point is for this func to return a char* that we're gonna send to the pipe

char* worker_menu(char* query, List dirs, HashTable patients, HashTable diseases_hash) {
    char delim[3] = " \n";
    if (strstr(query, "/diseaseFrequency")) { //TODO: Check for correct call of strstr
        if (n_words(query) < 4 || n_words(query) > 5) {
            fprintf(stderr, "error\n");
            return;
        }
        char* q = strtok(query, delim);
        char* virus = strtok(NULL, delim);
        char* arg2 = strtok(NULL, delim);
        char* arg3 = strtok(NULL, delim);
        char* country = strtok(NULL, delim);
        // if a country is specified, then just return the number
        if (country) {
            int res = disease_frequency(virus, arg2, arg3, country, diseases_hash);
            return itoa(res);
        }
        else {
            int res = 0;
            for (int i = 0; i < dirs->size; i++) {
                char* curr_country = list_nth(dirs,i);
                res += disease_frequency(virus, arg2, arg3, curr_country, diseases_hash);
            }
            return itoa(res);
        }
    }
    else if (strstr(query, "/topk-AgeRanges")) {
        if (n_words(query) != 6) {
            fprintf(stderr, "error\n");
            return;
        }
        char* q = strtok(query, delim);
        int k = atoi(strtok(NULL, delim));
        // Analyze the input
        char* country = strtok(NULL, delim);
        char* disease = strtok(NULL, delim);
        char* day1 = strtok(NULL, delim);
        char* day2 = strtok(NULL, delim);
        char* ret = topk_age_ranges(k, country, disease, day1, day2, diseases_hash); //TODO: Change type
        
    }
    else if (strstr(query, "/searchPatientRecord")) {
        if (n_words(query) != 2) {
            fprintf(stderr, "error\n");
            return;
        }
        char* q = strtok(query, delim);
        char* r_id = strtok(NULL, delim);
        char* res = search_patient_record(r_id, patients);
        //TOOD: Write on the pipe
    }
    else if (strstr(query, "/numPatientAdmissions")) {
        if (n_words(query) < 4 || n_words(query) > 5) {
            fprintf(stderr, "error\n");
            return;
        }
        char* q = strtok(query, delim);
        char* virus = strtok(NULL, delim);
        char* arg2 = strtok(NULL, delim);
        char* arg3 = strtok(NULL, delim);
        char* country = strtok(NULL, delim);
        if (country) {
            int res = num_patient_admissions(virus, arg2, arg3, country, diseases_hash);
            // TOOD: Write on the pipe
        }
        else {
             // allocate a string to sore our response
            char* ret = malloc(STRING_SIZE * sizeof(*ret));
            for (int i = 0; i < dirs->size; i++) {
                char* curr_country = list_nth(dirs, i);
                // find the n of cases
                int res = disease_frequency(virus, arg2, arg3, curr_country, diseases_hash);
                // append the result in the return string  //TODO: Check if snprintf works the way we want to 
                snprintf(ret, "%s %d\n", curr_country, res);
            }
        }
    }
    else if (strstr(query, "/numPatientDischarges")) {
        if (n_words(query) < 4 || n_words(query) > 5) {
            fprintf(stderr, "error\n");
            return;
        }
        char* q = strtok(query, delim);
        char* virus = strtok(NULL, delim);
        char* arg2 = strtok(NULL, delim);
        char* arg3 = strtok(NULL, delim);
        char* country = strtok(NULL, delim);
        if (country) {
            int res = num_patient_discharges(virus, arg2, arg3, country, diseases_hash);
            // TOOD: Write on the pipe
        }
        else {
            for (int i = 0; i < dirs->size; i++) {
                char* curr_country = list_nth(dirs, i);
                int res = num_patient_discharges(virus, arg2, arg3, curr_country, diseases_hash);
                // TOOD: Write on the pipe			
            }
        }
    }
    else {
        fprintf(stderr, "Query not recognized\n");
        return NULL;
    }
}
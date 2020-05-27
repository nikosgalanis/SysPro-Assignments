#include "common_types.h"
#include "WorkerMenu.h"
#include "common_functions.h"

char* worker_menu(char* query, List dirs, HashTable patients, HashTable diseases_hash) {
    char delim[3] = " \n";
    if (strstr(query, "/diseaseFrequency")) {
        if (n_words(query) < 4 || n_words(query) > 5) {
            fprintf(stderr, "error\n");
            return NULL;
        }
        char* q = strtok(query, delim);
        if (strcmp(q, "/diseaseFrequency"))
            return NULL;
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
            return NULL;
        }
        char* q = strtok(query, delim);
        if (strcmp(q, "/topk-AgeRanges"))
            return NULL;
        // Analyze the input
        int k = atoi(strtok(NULL, delim));
        char* country = strtok(NULL, delim);
        char* disease = strtok(NULL, delim);
        char* day1 = strtok(NULL, delim);
        char* day2 = strtok(NULL, delim);
        return topk_age_ranges(k, country, disease, day1, day2, diseases_hash);
        
    }
    else if (strstr(query, "/searchPatientRecord")) {
        if (n_words(query) != 2) {
            fprintf(stderr, "error\n");
            return NULL;
        }
        char* q = strtok(query, delim);
        if (strcmp(q, "/searchPatientRecord"))
            return NULL;
        char* r_id = strtok(NULL, delim);
        return search_patient_record(r_id, patients);
    }
    else if (strstr(query, "/numPatientAdmissions")) {
        if (n_words(query) < 4 || n_words(query) > 5) {
            fprintf(stderr, "error\n");
            return NULL;
        }
        char* q = strtok(query, delim);
        if (strcmp(q, "/numPatientAdmissions"))
            return NULL;
        char* virus = strtok(NULL, delim);
        char* arg2 = strtok(NULL, delim);
        char* arg3 = strtok(NULL, delim);
        char* country = strtok(NULL, delim);
        if (country) {
            return num_patient_admissions(virus, arg2, arg3, country, diseases_hash);
        }
        else {
             // allocate a string to sore our response
            char* ret = malloc(STRING_SIZE * sizeof(*ret));
            for (int i = 0; i < dirs->size; i++) {
                char* curr_country = list_nth(dirs, i);
                // append the result in the return string 
                sprintf(ret, "%s", num_patient_admissions(virus, arg2, arg3, curr_country, diseases_hash));
            }
            return ret;
        }
    }
    else if (strstr(query, "/numPatientDischarges")) {
        if (n_words(query) < 4 || n_words(query) > 5) {
            fprintf(stderr, "error\n");
            return NULL;
        }
        char* q = strtok(query, delim);
        if (strcmp(q, "/numPatientDischarges"))
            return NULL;
        char* virus = strtok(NULL, delim);
        char* arg2 = strtok(NULL, delim);
        char* arg3 = strtok(NULL, delim);
        char* country = strtok(NULL, delim);
        if (country) {
            return num_patient_discharges(virus, arg2, arg3, country, diseases_hash);
        }
        else {
             // allocate a string to sore our response
            char* ret = malloc(STRING_SIZE * sizeof(*ret));
            for (int i = 0; i < dirs->size; i++) {
                char* curr_country = list_nth(dirs, i);
                // append the result in the return string 
                sprintf(ret, "%s", num_patient_discharges(virus, arg2, arg3, curr_country, diseases_hash));
            }
            return ret;
        }
    }
    else {
        fprintf(stderr, "Query not recognized\n");
        return NULL;
    }
}
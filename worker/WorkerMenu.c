#include "common_types.h"
#include "WorkerMenu.h"
#include "common_functions.h"

bool worker_menu(char* qu, List dirs, HashTable patients, HashTable diseases_hash, int write, int buffsize) {
    char* query = strdup(qu);

    char delim[3] = " \n";
    if (strstr(query, "/diseaseFrequency")) {
        if (n_words(query) < 4 || n_words(query) > 5) {
            fprintf(stderr, "error\n");
            return false;
        }
        char* q = strtok(query, delim);
        if (strcmp(q, "/diseaseFrequency")) {
            free(qu);
            return false;
        }
        char* virus = strtok(NULL, delim);
        char* arg2 = strtok(NULL, delim);
        char* arg3 = strtok(NULL, delim);
        char* country = strtok(NULL, delim);
        int res = 0;
        // if a country is specified, then just return the number
        if (country) {
            res = disease_frequency(virus, arg2, arg3, country, diseases_hash);
            // return itoa(res);
        }
        else {
            for (int i = 0; i < dirs->size; i++) {
                char* curr_country = list_nth(dirs,i);
                res += disease_frequency(virus, arg2, arg3, curr_country, diseases_hash);
            }
            free(qu);
            // return itoa(res);
        }
        write_to_pipe(write, buffsize, itoa(res));
        return true;
    }
    else if (strstr(query, "/topk-AgeRanges")) {
        if (n_words(query) != 6) {
            fprintf(stderr, "error\n");
            free(qu);
            return false;
        }
        char* q = strtok(query, delim);
        if (strcmp(q, "/topk-AgeRanges")) {
            free(qu);
            return false;
        }
        // Analyze the input
        int k = atoi(strtok(NULL, delim));
        char* country = strtok(NULL, delim);
        char* disease = strtok(NULL, delim);
        char* day1 = strtok(NULL, delim);
        char* day2 = strtok(NULL, delim);
        topk_age_ranges(k, country, disease, day1, day2, diseases_hash, write, buffsize);   
        return true;
    }
    else if (strstr(query, "/searchPatientRecord")) {
        if (n_words(query) != 2) {
            fprintf(stderr, "error\n");
            free(qu);
            return false;
        }
        char* q = strtok(query, delim);
        if (strcmp(q, "/searchPatientRecord")) {
            free(qu);
            return false;
        }
        char* r_id = strtok(NULL, delim);
        char* result = search_patient_record(r_id, patients);
        write_to_pipe(write, buffsize, search_patient_record(r_id, patients));
        if (!strcmp(result, "-"))
            return false;
        else 
            return true;
    }
    else if (strstr(query, "/numPatientAdmissions") || strstr(query, "/numPatientDischarges")) {
        fprintf(stderr, "query in child is %s\n", query);
        if (n_words(query) < 4 || n_words(query) > 5) {
            fprintf(stderr, "error\n");
            free(qu);
            return false;
        }
        char* q = strtok(query, delim);
        if (strcmp(q, "/numPatientAdmissions")) {
            free(qu);
            return false;
        }
        char* virus = strtok(NULL, delim);
        char* arg2 = strtok(NULL, delim);
        char* arg3 = strtok(NULL, delim);
        char* country = strtok(NULL, delim);
        // If the query is specified for 1 country
        if (country) {
            // just write it to the pipe
            char* result = num_patient_admissions(virus, arg2, arg3, country, diseases_hash);
            write_to_pipe(write, buffsize, result);
            free(result);
            return true;
        }
        else {
            // if we wna to send many responses, inform the parent how many of them we're gonna store
            write_to_pipe(write, buffsize, itoa(dirs->size));
            // for each one of them
            for (int i = 0; i < dirs->size; i++) {
                // call the query function with a specific country
                char* curr_country = list_nth(dirs, i);
                char* result = num_patient_admissions(virus, arg2, arg3, curr_country, diseases_hash);
                write_to_pipe(write, buffsize, result);
                free(result);
            }
            free(qu);
            return true;
        }
    } else {
        fprintf(stderr, "Query not recognized\n");
        free(qu);
        return false;
    }
}
#include "common_types.h"
#include "WorkerMenu.h"
#include "common_functions.h"
#include "Queries.h"
bool worker_menu(char* qu, List dirs, HashTable patients, HashTable diseases_hash, int writing) {
	char* query = strdup(qu);

	char delim[3] = " \n";
	if (strstr(query, "/diseaseFrequency")) {
		if (n_words(query) < 4 || n_words(query) > 5) {
			fprintf(stderr, "error\n");
			write_to_socket(writing, "-", strlen("-"));
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
		}
		else {
			for (int i = 0; i < dirs->size; i++) {
				char* curr_country = list_nth(dirs,i);
				res += disease_frequency(virus, arg2, arg3, curr_country, diseases_hash);
			}
			free(qu);
		}
		char* result = itoa(res);
		write_to_socket(writing, result, strlen(result));
		free(result);
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
		topk_age_ranges(k, country, disease, day1, day2, diseases_hash, writing);   
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
		write_to_socket(writing, result, strlen(result));
		if (!strcmp(result, "-"))
			return false;
		else 
			return true;
	}
	else if (strstr(query, "/numPatientAdmissions")) {
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
			int sum = num_patient_admissions(virus, arg2, arg3, country, diseases_hash);
			char* result = itoa(sum);
			write_to_socket(writing, result, strlen(result));
			free(result);
			return true;
		}
		else {
			int result = 0;
			// for each one of the countries
			for (int i = 0; i < dirs->size; i++) {
				// call the query function with a specific country
				char* curr_country = list_nth(dirs, i);
				// increase the sum by the amount of patients in each call
				result += num_patient_admissions(virus, arg2, arg3, curr_country, diseases_hash);
			}
			char* final = itoa(result);
			write_to_socket(writing, final, strlen(final));
			free(final);
			free(qu);
			return true;
		}
	} 
	else if (strstr(query, "/numPatientDischarges")) {
		if (n_words(query) < 4 || n_words(query) > 5) {
			fprintf(stderr, "error\n");
			free(qu);
			return false;
		}
		char* q = strtok(query, delim);
		if (strcmp(q, "/numPatientDischarges")) {
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
			int sum = num_patient_admissions(virus, arg2, arg3, country, diseases_hash);
			char* result = itoa(sum);
			write_to_socket(writing, result, strlen(result));
			free(result);
			return true;
		}
		else {
			int result = 0;
			// for each one of the countries
			for (int i = 0; i < dirs->size; i++) {
				// call the query function with a specific country
				char* curr_country = list_nth(dirs, i);
				// increase the sum by the amount of patients in each call
				result += num_patient_admissions(virus, arg2, arg3, curr_country, diseases_hash);
			}
			char* final = itoa(result);
			write_to_socket(writing, final, strlen(final));
			free(final);
			free(qu);
			return true;
		}
	} else {
		fprintf(stderr, "Query not recognized by worker: %s\n", qu);
		free(qu);
		return false;
	}
}
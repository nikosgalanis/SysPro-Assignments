#include "common_types.h"
#include "common_functions.h"

// List countries query, designated for the monitor, as he is the onnly one who has all the info ensembled
void list_countries(HashTable hash) {
    hash_traverse(hash, print_list_contents, NULL, NULL, NULL);
}

void menu(int* reading, int* writing, int n_workers, int buff_size, HashTable hash) {
	char instruction[STRING_SIZE];
	char delim[3] = " \n";
	printf("Enter a new query\n");
	while (fgets(instruction, STRING_SIZE, stdin) != NULL) {
        if(strstr(instruction, "/listCountries")) {
            list_countries(hash);
        }
		else if(strstr(instruction, "/diseaseFrequency")) {
            // inform all the workers that we request a diseasefreq query
            for (int i = 0; i < n_workers; i++) {
                write_to_pipe(writing[i], buff_size, instruction);
            }
            int result = 0;
            for (int i = 0; i < n_workers; i++) { //TODO: Add select()
                // read the worker's response
                char* worker_response = read_from_pipe(reading[i]);
                // add this to the total result
                result += atoi(worker_response);
            }
            // inform the user about the result
            fprintf(stdout, "%d\n", result);
		}
		else if(strstr(instruction, "/topk-AgeRanges")) {
            // inform all the workers that we request a topkageranges query
            for (int i = 0; i < n_workers; i++) {
                write_to_pipe(writing[i], buff_size, instruction);
            }
            for (int i = 0; i < n_workers; i++) { //TODO: Add select()
                // read the worker's response
                char* worker_response = read_from_pipe(reading[i]);
                // find the worker that will NOT return -
                if (strcmp(worker_response, "-")) {
                    // print this result to the user
                    fprintf(stdout, "%s\n", worker_response);
                }
            }
		}
		else if(strstr(instruction, "/searchPatientRecord")) {
            // inform all the workers that we request a searchpatient query
            for (int i = 0; i < n_workers; i++) {
                write_to_pipe(writing[i], buff_size, instruction);
            }
            for (int i = 0; i < n_workers; i++) { //TODO: Add select()
                // read the worker's response
                char* worker_response = read_from_pipe(reading[i]);
                // find the worker that will NOT return -
                if (strcmp(worker_response, "-")) {
                    // print this result to the user
                    fprintf(stdout, "%s\n", worker_response);
                }
            }
		}
		else if(strstr(instruction, "/numPatientAdmissions")) {
            // if 4 args are provided then the last is the country.
            bool single_country = (n_words(info) == 4);
            // inform all the workers that we request an admission query
            for (int i = 0; i < n_workers; i++) {
                write_to_pipe(writing[i], buff_size, instruction);
            }
            for (int i = 0; i < n_workers; i++) { //TODO: Add select()
                // read the worker's response
                char* worker_response = read_from_pipe(reading[i]);
                // find the worker that will NOT return -
                if (strcmp(worker_response, "-")) {
                    // print this result to the user
                    if (single_country) {
                        // if only one country is provided, then we want just the number, not the country
                        // the number is the 2nd word on the string
                        fprintf(stdout, "%s\n", nth_word(worker_response, 2));
                    } else {
                        // else just print all the responses
                        fprintf(stdout, "%s\n", worker_response);
                    }
                }
            }
		}		
        else if(strstr(instruction, "/numPatientDischarges")) {
            // if 4 args are provided then the last is the country.
            bool single_country = (n_words(info) == 4);
            // inform all the workers that we request a discharge query
            for (int i = 0; i < n_workers; i++) {
                write_to_pipe(writing[i], buff_size, instruction);
            }
            for (int i = 0; i < n_workers; i++) { //TODO: Add select()
                // read the worker's response
                char* worker_response = read_from_pipe(reading[i]);
                // find the worker that will NOT return -
                if (strcmp(worker_response, "-")) {
                    // print this result to the user
                    if (single_country) {
                        // if only one country is provided, then we want just the number, not the country
                        // the number is the 2nd word on the string
                        fprintf(stdout, "%s\n", nth_word(worker_response, 2));
                    } else {
                        // else just print all the responses
                        fprintf(stdout, "%s\n", worker_response);
                    }
                }
            }
		}
		else if(strstr(instruction, "/exit")) {
			exit_monitor();
			return;
		}
		else {
			printf("Query not recognized. Choose one of the default options\n");
		}
		printf("\nEnter a new query\n");
	}    
}
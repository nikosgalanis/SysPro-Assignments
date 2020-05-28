#include "common_types.h"
#include "common_functions.h"

// List countries query, designated for the monitor, as he is the onnly one who has all the info ensembled
void list_countries(HashTable hash) {
    hash_traverse(hash, print_list_contents, NULL, NULL, NULL);
}

void menu(int* reading, int* writing, int n_workers, int* workers_ids, int buff_size, HashTable hash) {
	char instruction[STRING_SIZE];
	printf("Enter a new query\n");
	while (fgets(instruction, STRING_SIZE, stdin) != NULL) {
        if(strstr(instruction, "/listCountries")) {
            list_countries(hash);
        }
		else if(strstr(instruction, "/diseaseFrequency")) {
            bool single_country = (n_words(instruction) == 5);
            //if one country is provided
            if (single_country) {
                char* country = nth_word(instruction, 5);
                // search for the worker that has taken this country
                HashEntry ent = hash_search(hash, country);
                if (ent) {
                    // get the pid of the worker
                    int id = *(int*)ent->item;
                    // and find the his id in our arrays
                    int pos = get_pos_from_pid(id, workers_ids, n_workers);
                    if (pos != FAILED) {
                        fprintf(stderr, "heeeere\n");
                        // inform __only__ this worker for the query
                        write_to_pipe(writing[pos], buff_size, instruction);
                        // get his response and print it
                        char* response = read_from_pipe(reading[pos], buff_size);
                        fprintf(stdout, "%d\n", atoi(response));
                    } else {
                        fprintf(stderr, "Error with country provided");
                        continue;
                    }
                }
                else {
                    fprintf(stderr, "Country provided not listed in input dir\n");
                    continue;
                }
            }
            // else the user wants all the countries, thus we must inform all workers 
            else {
                // write into every pipe
                for (int i = 0; i < n_workers; i++) {
                    write_to_pipe(writing[i], buff_size, instruction);
                }
                int result = 0;
                // read from all the pipes
                for (int i = 0; i < n_workers; i++) { //TODO: Add select()
                    // read the worker's response
                    char* worker_response = read_from_pipe(reading[i], buff_size);
                    // add this to the total result
                    result += atoi(worker_response);
                }
                // inform the user about the result
                fprintf(stdout, "%d\n", result);
            }
        }
		else if(strstr(instruction, "/topk-AgeRanges")) {
            // the country is the 2nd word in our string
            char* country = nth_word(instruction, 3);
            // search for the worker that has taken this country
            HashEntry ent = hash_search(hash, country);
            if (ent) {
                // get the pid of the worker
                int id = *(int*)ent->item;
                // and find the his id in our arrays
                int pos = get_pos_from_pid(id, workers_ids, n_workers);
                if (pos != FAILED) {
                    // inform __only__ this worker for the query
                    write_to_pipe(writing[pos], buff_size, instruction);
                    // find how many times we will read from the pipe
                    int k = atoi(read_from_pipe(reading[pos], buff_size));
                    // get his response and print it
                    for (int i = 0; i < k; i++) {
                        char* response = read_from_pipe(reading[pos], buff_size);
                        fprintf(stderr, "%s", response);
                    }
                } else {
                    fprintf(stderr, "Error with country provided");
                    continue;
                }
            }
            else {
                fprintf(stderr, "Country provided not listed in input dir\n");
                continue;
            }
		}
		else if(strstr(instruction, "/searchPatientRecord")) {
            // inform all the workers that we request a searchpatient query
            for (int i = 0; i < n_workers; i++) {
                write_to_pipe(writing[i], buff_size, instruction);
            }
            for (int i = 0; i < n_workers; i++) { //TODO: Add select()
                // read the worker's response
                char* worker_response = read_from_pipe(reading[i], buff_size);
                // find the worker that will NOT return -
                if (strcmp(worker_response, "-")) {
                    // print this result to the user
                    fprintf(stdout, "%s\n", worker_response);
                }
            }
		}
		else if(strstr(instruction, "/numPatientAdmissions") || strstr(instruction, "/numPatientDischarges")) {
            // if 5 args are provided then the last is the country.
            bool single_country = (n_words(instruction) == 5);
            if (single_country) {
                // the country is the 5th word in our string
                char* country = nth_word(instruction, 5);
                // search for the worker that has taken this country
                HashEntry ent = hash_search(hash, country);
                if (ent) {
                    // get the pid of the worker
                    int id = *(int*)ent->item;
                    // and find the his id in our arrays
                    int pos = get_pos_from_pid(id, workers_ids, n_workers);
                    if (pos != FAILED) {
                        // inform __only__ this worker for the query
                        write_to_pipe(writing[pos], buff_size, instruction);
                        // get his response and print it
                        char* response = read_from_pipe(reading[pos], buff_size);
                        fprintf(stdout, "%s\n", response);
                    } else {
                        fprintf(stderr, "Error with country provided");
                        continue;
                    }
                }
            } else {
                fprintf(stderr, "heeeere\n");
                // inform all the workers that we request an admission query
                for (int i = 0; i < n_workers; i++) {
                    write_to_pipe(writing[i], buff_size, instruction);
                }
                for (int i = 0; i < n_workers; i++) { //TODO: Add select()
                    // read how many responses the worker is gonna send
                    int n_res = atoi(read_from_pipe(reading[i], buff_size));
                    // for each of the string that the worker wants to print
                    for (int j = 0; j < n_res; j++) {
                        // read and print the worker's response
                        char* res = read_from_pipe(reading[i], buff_size);
                        fprintf(stdout, "%s", res);
                        free(res);
                    }
                }
            }
		}
		else if(strstr(instruction, "/exit")) {
            // inform every user that we are ready to kill them
            for (int i = 0; i < n_workers; i++) {
                write_to_pipe(writing[i], buff_size, instruction);
            }
            // TODO: send a sigkill to everyone
			return;
		}
		else {
			printf("Query not recognized. Choose one of the default options\n");
		}
		printf("\nEnter a new query\n");
	}    
}
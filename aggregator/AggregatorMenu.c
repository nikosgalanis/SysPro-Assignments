#include "common_types.h"
#include "common_functions.h"
#include <sys/select.h>
#include <sys/stat.h>
#include <signal.h>
#include "Handlers.h"

extern volatile sig_atomic_t sig_int_raised;

// List countries query, designated for the monitor, as he is the onnly one who has all the info ensembled
void list_countries(HashTable hash) {
	hash_traverse(hash, print_list_contents, NULL, NULL, NULL);
}

void menu(int* reading, int* writing, int n_workers, int* workers_ids, int buff_size, HashTable hash) {
    // static struct sigaction act;
    // act.sa_handler = catch_int;
    // sigfillset(&(act.sa_mask));

    // sigaction(SIGINT, &act, NULL);
	char instruction[STRING_SIZE];
	// keep stats for 
	int success = 0, failed = 0;
	printf("Enter a new query\n");
	while (fgets(instruction, STRING_SIZE, stdin) != NULL) {
		// handle possible signals before the parent starts another query
		// if (sig_int_raised) {
			//TODO: Do somehting to notify the workers
			// fprintf(stderr, "heeeeere\n");
			// // Send a SIGKILL to kill all the workers
			// for (int i = 0; i < n_workers; i++) {
        	// 	kill(workers_ids[i], SIGKILL);
			// }
			// // create a directory to store our log files
			// mkdir("output", PERMS);
			// // create a log file to store what we've achieved
			// char* f_name = concat("../logs/log_file.", itoa(getpid()));
			// // open the file
			// FILE* log_file = fopen(f_name, "w+");
			// // free(f_name);
			// if (log_file == NULL) {
			// 	perror("creating");
			// 	exit(EXIT_FAILURE);
			// }
			// // print each country from the country ht to the file
			// hash_traverse(hash, print_countries, log_file, NULL, NULL);
			// // write the total query stats
			// fprintf(log_file, "\nTOTAL %d\nSUCCESS %d\n FAILED %d\n", (success + failed), success, failed);
			// // close the log_file
			// fclose(log_file);
			// // return to the main function to free everything before exiting
			// return;
		// }

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
						// inform __only__ this worker for the query
						write_to_pipe(writing[pos], buff_size, instruction);
						// get his response and print it
						char* response = read_from_pipe(reading[pos], buff_size);
						fprintf(stdout, "%d\n", atoi(response));
						free(response);
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
				fd_set active, read;
				// initialize the sets of the fds
				FD_ZERO(&active);
				for (int i = 0; i < n_workers; i++) {
					FD_SET(reading[i], &active);
				}
				int done = 0;
				// run until input is recieved by every worker
				while (done < n_workers) {
					// Block until an input arrives from one of the workers
					read = active;
					// find out how many workers are ready
					if (select(FD_SETSIZE, &read, NULL, NULL, NULL) < 0) {
						perror("select");
						exit(EXIT_FAILURE);
					}
					// run through each fd that exists in our set
					for (int i = 0; i < n_workers; i++) {
						// check if the i-th fd is ready for us to read
						if (FD_ISSET(reading[i], &read)) {
							// read the worker's response
							char* worker_response = read_from_pipe(reading[i], buff_size);
							// add this to the total result
							result += atoi(worker_response);
							free(worker_response);
							done++;
						}	
					}
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
						fprintf(stdout, "%s", response);
						free(response);
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
			// read from all the pipes
			fd_set active, read;
			// initialize the sets of the fds
			FD_ZERO(&active);
			for (int i = 0; i < n_workers; i++) {
				FD_SET(reading[i], &active);
			}
			int done = 0;
			// run until input is recieved by every worker
			while (done < n_workers) {
				// Block until an input arrives from one of the workers
				read = active;
				// find out how many workers are ready
				if (select(FD_SETSIZE, &read, NULL, NULL, NULL) < 0) {
					perror("select");
					exit(EXIT_FAILURE);
				}
				// run through each fd that exists in our set
				for (int i = 0; i < n_workers; i++) {
					// check if the i-th fd is ready for us to read
					if (FD_ISSET(reading[i], &read)) {
						// read the worker's response
						char* worker_response = read_from_pipe(reading[i], buff_size);
						// find the worker that will NOT return -
						if (strcmp(worker_response, "-")) {
							// print this result to the user
							fprintf(stdout, "%s\n", worker_response);
						}
						done++;
					}
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
						free(response);
					} else {
						fprintf(stderr, "Error with country provided");
						continue;
					}
				}
			} else {
				// inform all the workers that we request an admission query
				for (int i = 0; i < n_workers; i++) {
					write_to_pipe(writing[i], buff_size, instruction);
				}
				// read from all the pipes
				fd_set active, read;
				// initialize the sets of the fds
				FD_ZERO(&active);
				for (int i = 0; i < n_workers; i++) {
					FD_SET(reading[i], &active);
				}
				int done = 0;
				// run until input is recieved by every worker
				while (done < n_workers) {
					// Block until an input arrives from one of the workers
					read = active;
					// find out how many workers are ready
					if (select(FD_SETSIZE, &read, NULL, NULL, NULL) < 0) {
						perror("select");
						exit(EXIT_FAILURE);
					}
					// run through each fd that exists in our set
					for (int i = 0; i < n_workers; i++) {
						// check if the i-th fd is ready for us to read
						if (FD_ISSET(reading[i], &read)) {
							char* n = read_from_pipe(reading[i], buff_size);
							int n_countries = atoi(n);
							for (int j = 0; j < n_countries; j++) {
								// read and print the worker's response
								char* res = read_from_pipe(reading[i], buff_size);
								fprintf(stdout, "%s", res);
								free(res);
							}
							done++;
						}
					}
				}
			}
		}
		else if(strstr(instruction, "/exit")) {
			// inform every user that we are ready to kill them
			for (int i = 0; i < n_workers; i++) {
				write_to_pipe(writing[i], buff_size, instruction);
			}
			sleep(2);
			// TODO: send a sigkill to everyone
			return;
		}
		else {
			printf("Query not recognized. Choose one of the default options\n");
		}
		printf("\nEnter a new query\n");
	}    
}

#include "common_types.h"
#include "common_functions.h"
#include "HashTable.h"
#include "BalancedTree.h"
#include "Patient.h"
#include "LinkedList.h"
#include "WorkerMenu.h"
#include <dirent.h> 
#include "Queries.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parser.h"

int main(int argc, char* argv[]) {
	// Get the pipe names from the args, and open them:
	// 1st for writing, 2nd for reading
	int reading, writing;
	int buff_size = atoi(argv[3]);
	char* input_dir = concat(argv[4], "/");
	writing = open(argv[1], O_WRONLY, 0666);
	reading = open(argv[2], O_RDONLY, 0666);
	// Variables to stroe statistics for records.
	int success = 0; int failed = 0;
	// Create a hash table to store all the different diseases
	HashTable diseases_hash = hash_create(HASH_SIZE, hash_strings, BUCKET_SIZE, balanced_tree_destroy);
	// We are going to use one extra hashtable, in order to quickly search if the patient allready exists
	// We are going to pass destroy patient as a destroy func, in order to free all the memory occupied
	// by the patients when we are done.
	HashTable patients = hash_create(HASH_SIZE, hash_strings, sizeof(Patient) + sizeof(Pointer), destroy_patient);
	// create a list to store all the directories that the worker must handle
	List dirs = create_list(compare_strings, free); //TODO: Maybe NULL for destroy
	// create a list to hold all the files that are parsed, so we dont read them again when the user sends a SIGUSR1
	List parsed_files = create_list(compare_strings, NULL); //TODO: Maybe free
	char* str;
	// read the dirs from the pipe
	while (true) {
		str = read_from_pipe(reading, buff_size);
		// break when "end" is sent by the parent
		if (! strcmp(str, "end"))
			break;
		list_insert(dirs, str);

	}
	// proceed only if the worker is assigned with at least one country
	if (!is_empty(dirs)) {
		// call the function to parse the whole input and send the stats back to the parent
		parser(input_dir, buff_size, dirs, parsed_files, writing, patients, diseases_hash, &success, &failed);
		// read queries until we break
		while (true) {
			// read the instruction from the pipe
			char* query = read_from_pipe(reading, buff_size);
			// check if an exit command is given
			if (strstr(query, "/exit")) {
				// TODO: Maybe add to a function instead
				// free the memory occupied by our data structures
				hash_destroy(diseases_hash);
				// hash_destroy(patients);

				// create a directory to store our log files
				mkdir("output", PERMS);

				// create a log file to store what we've achieved
				char* f_name = concat("../logs/log_file.", itoa(getpid()));
				FILE* log_file = fopen(f_name, "w+");
				// free(f_name);
				if (log_file == NULL) {
					perror("creating");
					exit(EXIT_FAILURE);
				}
				// print all the dirs that we handled
				for (int i = 0; i < dirs->size; i++) {
					char* country = list_nth(dirs, i);
					fprintf(log_file, "%s\n", country);
				}
				// write the total query stats
				fprintf(log_file, "\nTOTAL %d\nSUCCESS %d\n FAILED %d\n", (success + failed), success, failed);
				// close the log file descriptor
				fclose(log_file);
				// close the pipes
				close(reading); close(writing);
				// free the list of the countries given
				destroy_list(dirs);
				// Finally, wait forever until the parent sends a SIGKILL that will exit the worker
				while(true);
			}
			// call the menu to analyze the query and return the result
			bool result = worker_menu(query, dirs, patients, diseases_hash, writing, buff_size);
			// check if an error has occured (returned null), thus we must not write in the pipe
			if (result) {
				success++;
			} else {
				failed++;
			}
		} 
	} else {
		// if no countries are assigned, just wait until the parent sends a SIGKILL
		while(true);
	}
}
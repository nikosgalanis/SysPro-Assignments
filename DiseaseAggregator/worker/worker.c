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
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parser.h"

volatile sig_atomic_t sig_int_raised;
volatile sig_atomic_t sig_usr1_raised;

void catch_int(int signo) {
    sig_int_raised = signo;
}

void catch_usr1(int signo) {
    sig_usr1_raised = signo;
}

int main(int argc, char* argv[]) {
	fprintf(stderr, "child %d\n", getpid());
	// iniialize a signal set
	static struct sigaction act_int;
	// handle our signals with our function, in order to catch them
    act_int.sa_handler = catch_int;
    sigfillset(&(act_int.sa_mask));
	// want to handle SIGINT and SIGQUIT with this function
    sigaction(SIGINT, &act_int, NULL);
    sigaction(SIGQUIT, &act_int, NULL);
	// we must also handle a SIGUSR1
	static struct sigaction act_usr;
	// handle our signals with our function, in order to catch them
    act_usr.sa_handler = catch_usr1;
    sigfillset(&(act_usr.sa_mask));
	// want to handle SIGINT and SIGQUIT with this function
    sigaction(SIGUSR1, &act_usr, NULL);

	// Get the pipe names from the args, and open them:
	// 1st for writing, 2nd for reading
	int reading, writing;
	int buff_size = atoi(argv[3]);
	char* input_dir = concat(argv[4], "/");
	writing = open(argv[1], O_WRONLY, 0666);
	reading = open(argv[2], O_RDONLY, 0666);
	// check for possible errors while opening the pipes
	if (reading == -1 || writing == -1) {
		perror("open");
	}
	// if "init is given, then during parsing we must print the stats"
	bool print_stats = (strcmp(argv[5], "init") == 0);
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
		fprintf(stderr, "%s\n", str);
		list_insert(dirs, str);

	}
	// proceed only if the worker is assigned with at least one country
	if (!is_empty(dirs)) {
		// call the function to parse the whole input and send the stats back to the parent
		parser(input_dir, buff_size, dirs, parsed_files, writing, patients, diseases_hash, &success, &failed, print_stats, false);
		// read queries until we break
		while (true) {
			// read the instruction from the pipe
			char* query = read_from_pipe(reading, buff_size);
			// check for a possible signal
			// If a sigint or sigquit are caught
			if (sig_int_raised) {
				fprintf(stderr, "int caught in main\n");
				// Just goto the exiting procedure
				goto EXIT_IF;
			}
			// if a sigusr1 is raised
			if (sig_usr1_raised) {
				fprintf(stderr, "usr1 caught in main\n");
				// parse the correct files, by giving the parsed list so we do not read all the files again
				parser(input_dir, buff_size, dirs, parsed_files, writing, patients, diseases_hash, &success, &failed, true, true);
				// inform the parent that we've read stuff by sending a sigusr2
				kill(getppid(), SIGUSR2);
				// restore the value of the gloval signal variable
				sig_usr1_raised = 0;
			}
			if (query != NULL) {
				// check if an exit command is given
				if (strstr(query, "/exit")) {
				EXIT_IF: 	fprintf(stderr, "Child exiting\n");
					// TODO: Maybe add to a function instead
					// free the memory occupied by our data structures
					hash_destroy(diseases_hash);
					// hash_destroy(patients);

					// create a directory to store our log files
					mkdir("./logs", PERMS);

					// create a log file to store what we've achieved
					char* f_name = concat("./logs/log_file.", itoa(getpid()));
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
					// free the list of the countries given
					destroy_list(dirs);
					// inform the parent that we've ended our job
					write_to_pipe(writing, buff_size, "ready");
					// close the pipes
					close(reading); close(writing);
					// if we are here because of an interupt, just exit
					if (sig_int_raised)
						exit(EXIT_SUCCESS);
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
		} 
	} else {
		// if no countries are assigned, just wait until the parent sends a SIGKILL
		while(true);
	}
}
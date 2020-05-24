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
#include <fcntl.h>

void print_todays_stats(Pointer ent, Pointer buffer_size, Pointer f_desc, Pointer dummy1, Pointer dummy2) {
	HashEntry entry = (HashEntry)ent;
	int fd = *(int*)f_desc;
	int buff_size = *(int*)buffer_size; 
	write_to_pipe(fd, buff_size, entry->key);
	int* age_groups = (int*)entry->item;
	char group1 [40];
	snprintf(group1, 40, "Age range 0-20 years: %d cases\n", age_groups[0]);
	char group2 [40];
	snprintf(group2, 40, "Age range 2-40 years: %d cases\n", age_groups[1]);
	char group3 [40];
	snprintf(group3, 40, "Age range 40-60 years: %d cases\n", age_groups[2]);
	char group4 [40];
	snprintf(group4, 40, "Age range 60+ years: %d cases\n", age_groups[3]); 
}


int main(int argc, char* argv[]) {
	// Get the pipe names from the args, and open them:
	// 1st for writing, 2nd for reading
	fprintf(stderr, "here\n");
	int reading, writing;
	int buff_size = atoi(argv[3]);
	writing = open(argv[1], O_WRONLY, 0666);
	reading = open(argv[2], O_RDONLY | O_NONBLOCK, 0666);
	// Variables to stroe statistics for records.
	int total = 0; int failed = 0;
	// Create a hash table to store all the different diseases
	HashTable diseases_hash = hash_create(HASH_SIZE, hash_strings, BUCKET_SIZE, balanced_tree_destroy);
	// We are going to use one extra hashtable, in order to quickly search if the patient allready exists
	// We are going to pass destroy patient as a destroy func, in order to free all the memory occupied
	// by the patients when we are done.
	HashTable patients = hash_create(HASH_SIZE, hash_strings, sizeof(Patient) + sizeof(Pointer), destroy_patient);
	// create a list to store all the directories that the worker must handle
	List dirs = create_list(compare_strings, free); //TODO: Maybe NULL for destroy
	char* str;
	// read the dirs from the pipe
	// sleep (3);
	while (true) {
		str = read_from_pipe(reading);
		fprintf(stderr, "%s\n", str);
		// break when "end" is sent by the parent
		if (! strcmp(str, "end"))
			break;
		list_insert(dirs, str);
	}
	fprintf(stderr,"yesss\n");
	// for every directory/country that the worker must parse
	for (int i = 0; i < dirs->size; i++) {
		// find the dir name
		char* directory = list_nth(dirs, i);
		// open it to access its data
		DIR* dir;
		if ((dir = opendir(directory)) == NULL) {
			fprintf(stderr, "Worker %d unable to locate directory %s", getpid(), directory);
			exit(EXIT_FAILURE);
		}
		struct dirent* file_to_scan;
		// Traverse all the files in the directory 
		while ((file_to_scan = readdir(dir)) != NULL) {
			// Ignore the . and .. dirs
			if (!strcmp(file_to_scan->d_name, ".") || !strcmp(file_to_scan->d_name, ".."))
				continue;
			// create a hash table to store thbe statistics needed for each file.
			HashTable todays_diseases = hash_create(HASH_SIZE, hash_strings, BUCKET_SIZE, free);
			int fd;
			// open the file that we want to parse
			if ((fd = open(file_to_scan->d_name, O_RDONLY | O_WRONLY)) == -1) {
				perror("open");
				exit(EXIT_FAILURE);
			}
			// Begin thje writing of the stats in the pipe
			write_to_pipe(writing, buff_size, file_to_scan->d_name);
			write_to_pipe(writing, buff_size, directory);
			// allocate size for the string that will temporary store the records
			char* record = malloc(STRING_SIZE * sizeof(*record));
			// read all the contents of the files
			while (read(fd, record, STRING_SIZE) > 0) {
				// If we have a patient that wants to enter
				if (strstr(record, "ENTER")) {
					// Create the entry, and store the age group that it belongs
					Patient* p = create_patient(record, directory, file_to_scan->d_name); //TODO: Propably need to combine dir + file names
					// Check if everything is ok
					if (p) {
						Date tree_key = p->entry_date;
						HashEntry disease_search_result = hash_search(diseases_hash, p->disease);
						if(disease_search_result != NULL) {
							BalancedTreeEntry new_tree_entry = create_balanced_tree_entry(tree_key, p);
							BalancedTree result_tree = disease_search_result->item;
							balanced_tree_insert(result_tree, new_tree_entry);
						}
						// If we do not find the entry, then we insert it, with an empty tree as a key
						else  {
							BalancedTree result_tree = create_balanced_tree(compare, NULL);
							hash_insert(diseases_hash, p->disease, result_tree);
							BalancedTreeEntry new_tree_entry = create_balanced_tree_entry(tree_key, p);
							balanced_tree_insert(result_tree, new_tree_entry);
						}
						// Search for the patient in the patients ht. If a patient with the same id is found
						// print an error. Else, just insert the pointer to the patient record in the hash.
						HashEntry patient_entry = hash_search(patients, p->id);
						if (patient_entry == NULL) {
							hash_insert(patients, p->id, p);
						} else {
							fprintf(stderr, "error\n");
							failed++;
						}
						// search for the disease for the stats
						HashEntry todays_disease_result = hash_search(todays_diseases, p->disease);
						int* age_groups;
						if (todays_disease_result != NULL) {
							//if we find it then we want access to the age groups array
							age_groups = (int*)todays_disease_result->item;
						} else {
							//if it is the first time that we see that disease then we must initialize a lot of stuff
							age_groups = malloc(4 * sizeof(*age_groups));
							for (int k = 0; k < 4; k++)
								age_groups[k] = 0;
							
							hash_insert(todays_diseases, p->disease, age_groups);
						}
						// update the stats
						if (p->age < 20) 
							age_groups[0]++;
						else if (p->age < 40)
							age_groups[1]++;
						else if (p->age < 60)
							age_groups[2]++;
						else 
							age_groups[3]++;
					}
				} else {
					// else, the patient must exit, with exit date the name of the file
					if (recordPatientExit(record, patients, file_to_scan->d_name) == false) {
						fprintf(stderr, "error");
						failed++;
					}
				}
				total++;
			}
			hash_traverse(todays_diseases, print_todays_stats, &buff_size, &writing, NULL);
			hash_destroy(todays_diseases);
		}
	}
	int total_queries = 0, success_queries = 0;
	while (true) {
		char* query = read_from_pipe(reading);
		
		char* result = worker_menu(query, dirs, patients, diseases_hash); //TODO: Change ret type
		if (result) {
			success_queries++;
		}
		total_queries++;
	} 
}
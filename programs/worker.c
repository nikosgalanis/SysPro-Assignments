#include "common_types.h"
#include "HashTable.h"
#include "BalancedTree.h"
#include "Patient.h"
#include "LinkedList.h"
#include <dirent.h> 
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int compare_strings (Pointer a, Pointer b) {
	return strcmp((char*) a, (char*)b);
}

// compare function for our entries
int compare(Pointer first, Pointer second) {
	TreeEntry entry1 = (TreeEntry)first;
	TreeEntry entry2 = (TreeEntry)second;
	return compare_dates(entry1->date, entry2->date);
}

uint hash_strings(void* key) {
	char* str = (char*)key;
	int h = 0, a = 33;
	for (; *str != '\0'; str++)
		h = (a * h + *str);
	return h;
}

// Count how many lines there are in a file
int nlines(FILE* input) {
	int n_lines = 0;
	while (!feof(input)) {
		char ch = fgetc(input);
		if (ch == '\n') {
			n_lines++;
		}
	}
	rewind(input);
	return n_lines;
}

void worker() {
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
	//TODO: get the info from the pipe on which directories to read
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
			}
			total++;
			// TODO: Send the data back to the parent
			hash_destroy(todays_diseases);
		}
	}
}
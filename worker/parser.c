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

void print_todays_stats(Pointer ent, Pointer buffer_size, Pointer f_desc, Pointer dummy1, Pointer dummy2) {
	HashEntry entry = (HashEntry)ent;
	int fd = *(int*)f_desc;
	int buff_size = *(int*)buffer_size; 
	// write the disease to the pipe
	write_to_pipe(fd, buff_size, entry->key);
	int* age_groups = (int*)entry->item;
	// collect data and add them to a string
	char* group1  = malloc(40 * sizeof(*group1));
	sprintf(group1, "Age range 0-20 years: %d cases\n", age_groups[0]);
	char* group2  = malloc(40 * sizeof(*group2));
	sprintf(group2, "Age range 2-40 years: %d cases\n", age_groups[1]);
	char* group3  = malloc(40 * sizeof(*group3));
	sprintf(group3, "Age range 40-60 years: %d cases\n", age_groups[2]);
	char* group4  = malloc(40 * sizeof(*group4));
	sprintf(group4, "Age range 60+ years: %d cases\n", age_groups[3]);
	int len = strlen(group1) + strlen(group2) + strlen(group3) + strlen(group4);
	char final [len];
	// collect all the data in one string
	sprintf(final, "%s%s%s%s\n", group1, group2, group3, group4);
	// free the temp strings allocated
	free(group1); free(group2); free(group3); free(group4);
	// write the final string in the pipe
	write_to_pipe(fd, buff_size, final);
}

void parser(char* input_dir, int buff_size, List dirs, List parsed_files, int writing, HashTable patients, HashTable diseases_hash, int* success, int* failed, bool print_stats) {
    // inform the parent how many stat strings he will read
    int n_files = n_files_in_worker(input_dir, dirs);
    if (print_stats)
        write_to_pipe(writing, buff_size, itoa(n_files));
    // for every directory/country that the worker must parse
    for (int i = 0; i < dirs->size; i++) {
        // find the dir name
        char* temp_dir = concat(input_dir, list_nth(dirs, i));
        char* directory = concat(temp_dir, "/");
        // open it to access its data
        DIR* dir;
        if ((dir = opendir(directory)) == NULL) {
            fprintf(stderr, "Worker %d unable to locate directory %s\n", getpid(), directory);
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
            // hold the old name of the file as the patients' country
            char* temp_name = file_to_scan->d_name;
            // get a full path to the file to open it
            char* file_name = concat(directory, temp_name);
            // parse the file if it has not allready been parsed
            if (!in_list(parsed_files, file_name)) {
                // insert the file into that list so we remember it for later
                list_insert(parsed_files, file_name);
            }
            // open the file that we want to parse
            FILE* curr_file = fopen(file_name, "r");
            if (curr_file == NULL) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            // Begin thje writing of the stats in the pipe
            if (print_stats)
                write_to_pipe(writing, buff_size, temp_name);
            char* country_to_send = list_nth(dirs, i);
            if (print_stats)
                write_to_pipe(writing, buff_size, country_to_send);
            // allocate size for the string that will temporary store the records
            char* record = malloc(STRING_SIZE * sizeof(*record));
            
            // read all the contents of the files
            while (fgets(record, STRING_SIZE, curr_file) != NULL) {
                // If we have a patient that wants to enter
                if (strstr(record, "ENTER")) {
                    // Create the entry, and store the age group that it belongs
                    Patient* p = create_patient(record, country_to_send, temp_name);
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
                            // fprintf(stderr, "error\n");
                            (*failed)++;
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
                        // update the hash entry
                        hash_update(todays_diseases, p->disease, age_groups);
                        (*success)++;
                    } else {
                        (*failed)++;
                    }
                } else {
                    // else, the patient must exit, with exit date the name of the file
                    if (recordPatientExit(record, patients, temp_name) == false) {
                        // fprintf(stderr, "error\n");
                        (*failed)++;
                    } else {
                        (*success)++;
                    }
                }
            }
            // inform the parent how many diseases to read
            char* n_dis = itoa(todays_diseases->items);
            if (print_stats) {
                write_to_pipe(writing, buff_size, n_dis);
                // traverse the ht to send the stats to the pipe
                hash_traverse(todays_diseases, print_todays_stats, &buff_size, &writing, NULL);
            }
            // close the file that we just parsed
            fclose(curr_file);
            // destroy the ht for this file, in order to avoid leaks
            hash_destroy(todays_diseases);
        }
        closedir(dir);
    }
}
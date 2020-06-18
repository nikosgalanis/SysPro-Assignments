#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "common_types.h"
#include "common_functions.h"
#include "HashTable.h"
#include "LinkedList.h"
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include "stats.h"

volatile sig_atomic_t sig_int_raised;

// sigint handler
void catch_int(int signo) {
    sig_int_raised = signo;
}

void renew_lists(Pointer ent, Pointer write_fd, Pointer old, Pointer dummy, Pointer new) {
	HashEntry entry = (HashEntry)ent;
	int writing = *(int*)write_fd;
	int old_worker = *(int*)old;
	int item = *(int*)entry->item; 
	if (item == old_worker) {
		// pass the dir to the new worker
		write_to_pipe(writing, 10, entry->key);
		// change the hash info
		entry->item = new;	
	}
}

void operation(int n_workers, int buff_size, char* input_dir, char* server_ip, char* server_port) {
	// iniialize a signal set
	static struct sigaction act_int;
	// handle our signals with our function, in order to catch them
    act_int.sa_handler = catch_int;
    sigfillset(&(act_int.sa_mask));
	// want to handle SIGINT and SIGQUIT with this function
    sigaction(SIGINT, &act_int, NULL);
	sigaction(SIGQUIT, &act_int, NULL);

	// hold a pointer for the subdirs that we are going to check
	struct dirent* sub_dir;
	// Open the input dir given by the user
	DIR* input = opendir(input_dir);
	// Check if any error occurs
	if (input == NULL) {
		fprintf(stderr, "Problem with the given imput directory. Exiting...\n");
		exit(EXIT_FAILURE);
	}
	// Create n_workers child processes with fork, and then exec to redirect to another executable
	pid_t pid;
	// array to store the file descs coming from the pipes for writing
	int writing[n_workers];
	// array to store the pids of the childs
	pid_t workers_ids[n_workers];
	mkdir("../tmp", S_IRWXU | S_IRWXG | S_IRWXO);
	char* names_1[n_workers];
	char* names_2[n_workers];
	for (int i = 0; i < n_workers; i++) {
		pid = fork();
		// hold the name of the 2 named pipes
		char* str_i = itoa(i);
		names_2[i] = concat("../tmp/fifo_2_", str_i);
		free(str_i);
		if (pid > 0) {
			// the parent saves the child's pid
			workers_ids[i] = pid;
			if (mkfifo(names_2[i], 0666) == -1) {
				if (errno != EEXIST) {
					perror("reciever: mkfifo");
					exit(EXIT_FAILURE);
				}
			}
		} else {
			// The child does the rest
			// call an exec function, in order to redirect the child in the worker file
			execl("../worker/worker", "worker", names_2[i], itoa(buff_size), input_dir, "init", NULL);
			// if we reach this point, then exec has returned, so sthg wrong has happened
			perror("execl");
			exit(EXIT_FAILURE);
		}
	}
	// open the pipes and store the descriptors in the arrays that we have allocated
	for (int i = 0; i < n_workers; i++) {
		char* str_i = itoa(i);
		char* name2 = concat("../tmp/fifo_2_", str_i);
		free(str_i);
		if ((writing[i] = open(name2, O_WRONLY, 0666)) == -1) {
			perror("creating");
			exit(EXIT_FAILURE);
		}
		free(name2);
	}
	int n_dirs = 0;
	// Check how many directories(aka countries) are in the parent dir
	while ((sub_dir = readdir(input)) != NULL) {
		n_dirs++;
	}
	rewinddir(input);
	// store the directories in an array
	// We aer gonna remove . and .. dirs
	n_dirs -= 2;
	char* dirs[n_dirs]; 
	for (int i = 0; i < n_dirs; i++) {
		sub_dir = readdir(input);
		// Exclude the "." and ".." dirs
			if (strcmp(sub_dir->d_name, ".") && strcmp(sub_dir->d_name, ".."))
				dirs[i] = strdup(sub_dir->d_name);
			else 
				i--;
	}
	// Create a hash table to store which dirs are held by which workers (key: pid, item: list of countries)
	HashTable dirs_to_workers = hash_create(n_workers, hash_strings, BUCKET_SIZE, NULL);
	// find out the correct way to split the dirs
	int split_no = n_dirs / n_workers;
	int remainder = n_dirs % n_workers;
	int count = 0;
	// assign in each worker `split_no` dirs 
	for (int i = 0; i < n_workers; i++) {
		for (int j = 0; j < split_no; j++) {
			// assign the dir by informing the child process through the pipe
			write_to_pipe(writing[i], buff_size, dirs[count]);
			// save the tuple in the hash table
			hash_insert(dirs_to_workers, dirs[count], &workers_ids[i]);
			count++;
		}
	}
	//assign one dir to each worker, starting from worker 1
	int i = 0;
	// while there are countries remaining
	while (count < n_dirs) {
		// assign the dir by informing the child process through the pipe=
		write_to_pipe(writing[i], buff_size, dirs[count]);
		// save the tuple in the hash table
		hash_insert(dirs_to_workers, dirs[count], &workers_ids[i]);
		// adjust the pointers
		i++; count++;
	}
	for (int i = 0; i < n_workers; i++) {
		// write this into every pipe so the workers know when the dirs that they are gonna parse end
		write_to_pipe(writing[i], buff_size, "end");
		// also inform the worker about the server's ip and port
		write_to_pipe(writing[i], buff_size, server_ip);
		write_to_pipe(writing[i], buff_size, server_port);	
	}

	
	// update the nworkers variable in case that the dirs are less than the workers
	int active_workers = (split_no == 0) ? remainder : n_workers;

	// stay alive and check for possibly dead children
	while (true) {
		pid_t dead_child;
		// run a loop for all the childs(-1), to see if they have died. 
		// A dead child's pid is returned to dead_child var, so we know what we must do from here
		while ((dead_child = waitpid(-1, NULL, WNOHANG)) > 0) {
			fprintf(stderr, "Worker %d died\n", dead_child);
			// we must replace the dead child with a new one
			// get he position in the arrays
			int pos = get_pos_from_pid(dead_child, workers_ids, active_workers);
			pid_t new_pid = fork();
			// new names for new pipes
			char* str_i = itoa(pos);
			char* new_name1 = concat("./tmp/fifo_new_1_", str_i);
			char* new_name2 = concat("./tmp/fifo_new_2_", str_i);
			names_1[pos] = new_name1;
			names_2[pos] = new_name2;
			free(str_i);
			if (new_pid > 0) {
				// we are in the parent
				// Create __two__ new named pipes, so the process can write in one of them
				if (mkfifo(new_name1, 0666) == -1) {
					if (errno != EEXIST) {
						perror("reciever: mkfifo");
						exit(EXIT_FAILURE);
					}
				}

				if (mkfifo(new_name2, 0666) == -1) {
					if (errno != EEXIST) {
						perror("reciever: mkfifo");
						exit(EXIT_FAILURE);
					}
				}
			} else {
				// we are in the child, so call exec to go to the worker's code
				execl("./worker/worker", "worker", new_name1, new_name2, itoa(buff_size), input_dir, "new", NULL);
				// if we reach this point, then exec has returned, so sthg wrong has happened
				perror("execl");
				exit(EXIT_FAILURE);
			}
			// child has gone to another executable. Only the parent is here
			// close the old file descriptors
			close(writing[pos]);
			if ((writing[pos] = open(new_name2, O_WRONLY, 0666)) == -1) {
				perror("creating");
				exit(EXIT_FAILURE);
			}
			// send info to the new child 
			hash_traverse(dirs_to_workers, renew_lists, &writing[pos], &workers_ids[pos], &new_pid);
			// send an "end" so the new child knows that the directories have ended
			write_to_pipe(writing[pos], buff_size, "end");
			// update the pid
			workers_ids[pos] = new_pid;
		}
		// check for a possible sigint given
		if (sig_int_raised) {
			// free the hash table of the workers-countries
			hash_destroy(dirs_to_workers);
			// send a SIGKILL to the workers to end them
			for (int i = 0; i < n_workers; i++) {
				kill(workers_ids[i], SIGKILL);
			}
			// Delete all the pipes that we've opened
			for (int i = 0; i < n_workers; i++) {
				unlink(names_1[i]);
				unlink(names_2[i]);
			
				free(names_1[i]);
				free(names_2[i]);
			}
			// free stuff
			for (int i = 0; i < n_dirs; i++) {
				free(dirs[i]);
			}
			// wait until all the children are dead
			for (int i = 0; i < n_workers; i++) {
				wait(&workers_ids[i]);
			}
			// we are now ready to return to the main function to wrap it up
			return;
		}	
	}
}
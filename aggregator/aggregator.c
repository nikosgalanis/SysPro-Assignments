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
#include "Handlers.h"
#include <sys/select.h>
#include "stats.h"


// declaration of the menu function
void menu(int* reading, int* writing, int n_workers, int* workers_ids, int buff_size, HashTable hash, char** names1, char** names2, char* input_dir);

void aggregator(int n_workers, int buff_size, char* input_dir) {
    // static struct sigaction act;
    // act.sa_handler = catch_int;
    // sigfillset(&(act.sa_mask));

    // sigaction(SIGINT, &act, NULL);
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
    // sleep(5);
    // if (sig_int_raised) {
    //     fprintf(stderr, "here\n");
    // }
    // arrays to store the file descs coming from the pipes for reading and writing
    int reading[n_workers]; int writing[n_workers];
    // array to store the pids of the childs
    pid_t workers_ids[n_workers];
    char* names_1[n_workers];
    char* names_2[n_workers];
    for (int i = 0; i < n_workers; i++) {
        pid = fork();
        // hold the name of the 2 named pipes
        char* str_i = itoa(i);
        names_1[i] = concat("../tmp/fifo_1_", str_i);
        names_2[i] = concat("../tmp/fifo_2_", str_i);
        free(str_i);
        if (pid > 0) {
            // the parent saves the child's pid
            workers_ids[i] = pid;
            // Create __two__ named pipes, so each process can write in one of them
            if (mkfifo(names_1[i], 0666) == -1) {
                if (errno != EEXIST) {
                    perror("reciever: mkfifo");
                    exit(EXIT_FAILURE);
                }
            }

            if (mkfifo(names_2[i], 0666) == -1) {
                if (errno != EEXIST) {
                    perror("reciever: mkfifo");
                    exit(EXIT_FAILURE);
                }
            }
        } else {
            // The child does the rest
            // call an exec function, in order to redirect the child in the worker file
            execl("../worker/worker", "worker", names_1[i], names_2[i], itoa(buff_size), input_dir, "init", NULL);
            // if we reach this point, then exec has returned, so sthg wrong has happened
            perror("execl");
            exit(EXIT_FAILURE);
        }
    }
    // open the pipes and store the descriptors in the arrays that we have allocated
    for (int i = 0; i < n_workers; i++) {
        char* str_i = itoa(i);
        char* name1 = concat("../tmp/fifo_1_", str_i);
        char* name2 = concat("../tmp/fifo_2_", str_i);
        free(str_i);
        if ((reading[i] = open(name1, O_RDONLY, 0666)) == -1) {
            perror("creating");
            exit(EXIT_FAILURE);
        }
        if ((writing[i] = open(name2, O_WRONLY, 0666)) == -1) {
            perror("creating");
            exit(EXIT_FAILURE);
        }
        free(name1);
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
    // write this into every pipe so the workers know when the dirs that they are gonna parse end
    for (int i = 0; i < n_workers; i++) {
        write_to_pipe(writing[i], buff_size, "end");
    }
    // update the nworkers variable in case that the dirs are less than the workers
    int active_workers = (split_no == 0) ? remainder : n_workers;	
    // call the print function to prin the stats from the incoming files		
    print_stats(active_workers, buff_size, reading);
    // close the input directory
    closedir(input);
    // call the menu to get queries from the user and pass them to the workers
    menu(reading, writing, active_workers, workers_ids, buff_size, dirs_to_workers, names_1, names_2, input_dir);
    //the menu returns when the user has requested to exit the program

    //free the hash table of the workers-countries
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
}
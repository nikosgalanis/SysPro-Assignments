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

// declaration of the menu function
void menu(int* reading, int* writing, int n_workers, int* workers_ids, int buff_size, HashTable hash);

void aggregator(int n_workers, int buff_size, char* input_dir) {
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
    // arrays to store the file descs coming from the pipes for reading and writing
    int reading[n_workers]; int writing[n_workers];
    // array to store the pids of the childs
    pid_t workers_ids[n_workers];
    char* names_1[n_workers];
    char* names_2[n_workers];
    for (int i = 0; i < n_workers; i++) {
        pid = fork();
        // hold the name of the 2 named pipes
        names_1[i] = concat("../tmp/fifo_1_", itoa(i));
        names_2[i] = concat("../tmp/fifo_2_", itoa(i));
        if (pid != 0) {
            printf("parent\n");
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
        } else {
            // The child does the rest
            }
            fprintf(stderr, "damn it\n");
            // call an exec function, in order to redirect the child in the worker file
            execl("../worker/worker", "worker", names_1[i], names_2[i], itoa(buff_size), input_dir, NULL);
            // if we reach this point, then exec has returned, so sthg wrong has happened
            perror("execl");
            exit(EXIT_FAILURE);
        }
    }

    // open the pipes and store the descriptors in the arrays that we have allocated
    for (int i = 0; i < n_workers; i++) {
        char* name1 = concat("../tmp/fifo_1_", itoa(i));
        char* name2 = concat("../tmp/fifo_2_", itoa(i));
        if ((reading[i] = open(name1, O_RDONLY, 0666)) == -1) {
            perror("creating");
            exit(EXIT_FAILURE);
        }
        if ((writing[i] = open(name2, O_WRONLY, 0666)) == -1) {
            perror("creating");
            exit(EXIT_FAILURE);
        }
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
    HashTable dirs_to_workers = hash_create(n_workers, hash_strings, BUCKET_SIZE, free); //TODO: Check destroy fn
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
    while (remainder) {
        // assign the dir by informing the child process through the pipe=
        write_to_pipe(writing[i], buff_size, dirs[count]);
        // save the tuple in the hash table
        hash_insert(dirs_to_workers, dirs[count], &workers_ids[i]);
        i++; count++;
    }
    // write this into every pipe so the workers know when the dirs that they are gonna parse end
    for (int i = 0; i < n_workers; i++) {
        write_to_pipe(writing[i], buff_size, "end");
    }
    // print the incoming stats from each worker
    for (int i = 0; i < n_workers; i++) {
        char* n = read_from_pipe(reading[i], buff_size);
        int n_files = atoi(n);
        for (int j = 0; j < n_files; j++) {
            char* name = read_from_pipe(reading[i], buff_size);
            char* country = read_from_pipe(reading[i], buff_size);
            int n_diseases = atoi(read_from_pipe(reading[i], buff_size));
            // fprintf(stdout, "%s\n%s\n", name, country);
            for (int k = 0; k < n_diseases; k++) {
                char* info = read_from_pipe(reading[i], buff_size);
                // fprintf(stdout, "%s\n", info);
            }
        // fprintf(stdout, "\n\n");
        }
    }
    // close the input directory
    fprintf(stderr, "here\n");
    closedir(input);
    // // call the menu to get queries from the user and pass them to the workers
    // menu(reading, writing, n_workers, workers_ids, buff_size, dirs_to_workers);
    // the menu returns when the user has requested to exit the program
    // free the hash table of the workers-countries
    hash_destroy(dirs_to_workers);
    // for (int i = 0; i < n_workers; i++) {
    //     kill(workers_ids[i], SIGKILL);
    // }
    //TODO: Send sigkill to all the workers and wait for them to end
    // for (int i = 0; i < n_workers; i++)
    //     wait(NULL);

    // Delete all the pipes that we've opened
    for (int i = 0; i < n_workers; i++) {
        // close(reading[i]);
        // close(writing[i]);
        fprintf(stderr, "----------------------------------%s-------------------------- \n", names_1[i]);
        // unlink(names_1[i]);
        // unlink(names_2[i]);

    }
    wait(&workers_ids[0]);
    // we are now ready to return to the main function to wrap it up
}
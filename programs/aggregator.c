#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "common_types.h"
#include <errno.h>

void aggregator(int n_workers, int buff_size, char* input_dir) {
    // hold a pointer for the subdirs that we are going to check
    struct dirent *sub_dir;
    // Open the input dir given by the user
    printf("%s\n", input_dir);
    DIR* input = opendir(input_dir);
    // Check if any error occurs
    if (input == NULL) {
        fprintf(stderr, "Problem with the given imput directory. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    // Create n_workers child processes with fork, and then exec to redirect to another executable
    pid_t pid;
    for (int i = 0; i < n_workers; i++) {
        pid = fork();
        // Create __two__ named pipes, so each process can write in one of them
        char* name1 = strcat("fifo_1_", itoa(i));
        if (mkfifo(name1, 0666) == -1) {
            if (errno != EEXIST) {
                perror("reciever: mkfifo");
                exit(EXIT_FAILURE);
            }
        }

        char* name2 = strcat("fifo_2_", itoa(i));
        if (mkfifo(name2, 0666) == -1) {
            if (errno != EEXIST) {
                perror("reciever: mkfifo");
                exit(EXIT_FAILURE);
            }
        }
        execl("worker", name1, name2, itoa(buff_size), NULL);
        // if we reach this point, then exec has returned, so sthg wrong has happened
        perror("execl");
        exit(EXIT_FAILURE);
    }
    int n_dirs = 0;
    // Check how many directories(aka countries) are in the parent dir
    while ((sub_dir = readdir(input)) != NULL) {
        n_dirs++;
    }
    // Exclude the "." and ".." dirs
    n_dirs -= 2; //TODO: Find a better way to do that
    printf("%d dirs\n", n_dirs);
    rewinddir(input);
    int split_no = n_dirs / n_workers;
    int remainder = n_dirs % n_workers;
    for (int i = 0; i < n_workers; i++) {
        // assign in each worker split_no dirs 
    }
    while (remainder) {
        //assign one dir to each worker, starting from worker 1
    }

}
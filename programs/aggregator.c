#include <dirent.h> 
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "common_types.h"

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
    char* directory;
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
        
    }
    char* filename; //concat the name of the directory and the file with in_file->d_name
    int file_desc = open(filename, O_RDONLY);
    if (file_desc == -1) {
        printf("Unexpected error occured\n");
        exit(EXIT_FAILURE);
    }

}
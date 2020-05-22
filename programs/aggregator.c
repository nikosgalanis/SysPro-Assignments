#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "common_types.h"
#include "common_functions.h"
#include "HashTable.h"
#include "LinkedList.h"
#include <errno.h>
#include <string.h>

// List countries query, designated for the monitor, as he is the onnly one who has all the info ensembled
void list_countries(HashTable hash) {
    hash_traverse(hash, print_list_contents, NULL, NULL, NULL);
}

void aggregator(int n_workers, int buff_size, char* input_dir) {
    // hold a pointer for the subdirs that we are going to check
    struct dirent* sub_dir;
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
    int workers_ids[n_workers];
    for (int i = 0; i < n_workers; i++) {
        pid = fork();
        if (pid != 0) {
            workers_ids[n_workers] = pid;
        } else {
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
    }
    // arrays to store the file descs coming from the pipes for reading and writing
    int reading[n_workers]; int writing[n_workers];
    // open the creaded pipes. Their names are fifo_1|2_i
    for (int i = 0; i < n_workers; i++) {
        reading[i] = open(strcat("fifo_1_", itoa(i)), O_RDONLY);
        writing[i] = open(strcat("fifo_2_", itoa(i)), O_WRONLY);
        if (writing[i] < 0 || reading[i] < 0) {
            perror("fifo open error");
            // TODO: kill the children
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
    char* dirs[n_dirs]; 
    for (int i = 0; i < n_dirs; i++) {
        sub_dir = readdir(input);
        // Exclude the "." and ".." dirs
        if (sub_dir->d_name != "." && sub_dir->d_name != "..")
            dirs[i] = strdup(sub_dir->d_name);
    }
    // We removed . and .. dirs
    n_dirs -= 2;
    // Create a hash table to store which dirs are held by which workers (key: pid, item: list of countries)
    HashTable dirs_to_workers = hash_create(n_workers, hash_strings, BUCKET_SIZE, destroy_list); //TODO: Check destroy fn
    for (int i = 0; i < n_workers; i++) {
        // Initialize the item: an (for now) empty list
        List list = create_list(compare_strings, NULL); //TODO : Possible leaks
        printf("%s\n", itoa(i));
        hash_insert(dirs_to_workers, itoa(workers_ids[i]), list);
    }
    int split_no = n_dirs / n_workers;
    int remainder = n_dirs % n_workers;
    // assign in each worker `split_no` dirs 
    int count = 0;
    for (int i = 0; i < n_workers; i++) {
        for (int j = 0; j < split_no; j++) {
            // assign the dir by informing the child process through the pipe
            write_to_pipe(writing[i], buff_size, dirs[count]);
            // get access to the child's current dirs
            HashEntry entry = hash_search(dirs_to_workers, itoa(workers_ids[i]));
            if (entry) {
                List curr_list = entry->item;
                // add the newly assigned country to the list
                list_insert(curr_list, dirs[count]);
            }
            count++;
        }
    }
    //assign one dir to each worker, starting from worker 1
    int i = 0;
    // while there are countries remaining
    while (remainder) {
        // assign the dir by informing the child process through the pipe=
        write_to_pipe(writing[i], buff_size, dirs[count]);
        // get access to the child's current dirs
        List curr_list = hash_search(dirs_to_workers, itoa(workers_ids[i]))->item;
        // add the newly assigned country to the list
        list_insert(curr_list, dirs[count]);
        // increase the bounds-checking counters
        i++; count++;
    }
}
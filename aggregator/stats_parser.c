#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include "common_functions.h"

void print_stats(int active_workers, int buff_size, int* reading) {
        // read from all the pipes
    fd_set active, read;
    // initialize the sets of the fds
    FD_ZERO(&active);
    for (int i = 0; i < active_workers; i++) {
        FD_SET(reading[i], &active);
    }
    int done = 0;
    while (done < active_workers) {
        // print the incoming stats from each worker
        // Block until an input arrives from one of the workers
        read = active;
        // find out how many workers are ready
        if (select(FD_SETSIZE, &read, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < active_workers; i++) {
            if (FD_ISSET(reading[i], &read)) {
                char* n = read_from_pipe(reading[i], buff_size);
                int n_files = atoi(n);
                for (int j = 0; j < n_files; j++) {
                    char* name = read_from_pipe(reading[i], buff_size);
                    char* country = read_from_pipe(reading[i], buff_size);
                    char* n_dis = read_from_pipe(reading[i], buff_size);
                    int n_diseases = atoi(n_dis);
                    fprintf(stdout, "%s\n%s\n", name, country);
                    for (int k = 0; k < n_diseases; k++) {
                        char* disease = read_from_pipe(reading[i], buff_size);
                        fprintf(stdout, "%s\n", disease);
                        free(disease);
                        char* info = read_from_pipe(reading[i], buff_size);
                        fprintf(stdout, "%s\n", info);
                        free(info);
                    }
                    fprintf(stdout, "\n");
                    free(name); 
                    free(country);
                    free(n_dis);
                }
                free(n);
                done++;
            }
        }
    }
}
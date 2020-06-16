#include "buffer.h"
#include "common_functions.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "buffer.h"

// global variables for our buffer mutexes and condition variables
pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;

// declaration of the menu function for the server
void menu(int fd, HashTable hash, char** names1, char** names2);

// function for the slave threads
Pointer slave_thread_operate(Pointer buff) {
	Buffer buffer = (Buffer)buff;
	//TODO: Lock semaphores
	int fd = obtain(buffer);
	char type;
	// find out the type of the incoming info: "s" for stats, "q" for query
	read(fd, &type, sizeof(char));
	if (type == "s") {
		// if a stats string is read, then print all the stats provided by the worker
		// first thing: how many files the worker reported
		char* n = read_from_socket(fd);
		int n_files = atoi(n);
		for (int j = 0; j < n_files; j++) {
			// each file has a name, a country, and some diseases
			char* name = read_from_socket(fd);
			char* country = read_from_socket(fd);
			char* n_dis = read_from_socket(fd);
			int n_diseases = atoi(n_dis);
			fprintf(stdout, "%s\n%s\n", name, country);
			// for each disease
			for (int k = 0; k < n_diseases; k++) {
				// parse the stats
				char* disease = read_from_socket(fd);
				fprintf(stdout, "%s\n", disease);
				free(disease);
				char* info = read_from_socket(fd);
				fprintf(stdout, "%s\n", info);
				free(info);
			}
			fprintf(stdout, "\n");
			// no leaks!
			free(name); 
			free(country);
			free(n_dis);
		}
		free(n);
	} else {
		// else, a query string is given from a client
		
	}
}

void server_operation(char* query_port, char* stats_port, int buffer_size, int num_threads) {
	// create a cyclic buffer to store the file descriptors from the connections that we are going to serve
	Buffer buff = initialize_buffer(buffer_size);
	// allocate space to store the thread ids
	pthread_t* thread_ids = malloc(num_threads * sizeof(*thread_ids));
	if (thread_ids == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	// first things first: create n threads in order to serve simulatneously
	for (int i = 0; i < num_threads; i++) {
		int res = pthread_create(&thread_ids[i], NULL, slave_thread_operate, buff);
	}	
}
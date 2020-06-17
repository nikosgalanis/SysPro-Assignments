#include "buffer.h"
#include "common_types.h"
#include "common_functions.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

// global variables for our buffer mutexes and condition variables
pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
pthread_mutex_t printing;

// global hash to store the distribution of the directories to the workers
HashTable dirs_to_workers;

// declaration of the menu function for the server
void menu(int fd, HashTable hash, char** names1, char** names2);

// signal global variable
volatile sig_atomic_t sig_int_raised;

// sigint handler
void catch_int(int signo) {
    sig_int_raised = signo;
}

// function for the slave threads
Pointer slave_thread_operate(Pointer buff) {
	Buffer buffer = (Buffer)buff;
	// we run forever
	while (true) {
		// obtain a fd from the buffer 
		int fd = obtain(buffer);
		// the buffer is not full anymore (if it already was)
		pthread_cond_signal(&cond_nonfull);
		char type;
		// find out the sender of the incoming info: "w" for worker, "c" for client
		read(fd, &type, sizeof(char));
		if (type == 'w') {
			int port;
			char* p = malloc(5 * sizeof(char));
			// read the port from the worker
			read(fd, &port, sizeof(int));
			// int port = atoi(p);
			// read the dirs from the worker
			int n_dirs;
			read(fd, &n_dirs, sizeof(int));
			for (int i = 0; i < n_dirs; i++) {
				char* country = read_from_socket(fd);
				hash_insert(dirs_to_workers, country, &port);
			}
			// print all the stats provided by the worker
			// first thing: how many files the worker reported
			int n_files;
			read(fd, &n_files, sizeof(int));
			fprintf(stderr, "%d\n", n_files);
			// lock the printing semaphore, so we do not have many threads printing in the stdout
			pthread_mutex_lock(&printing);
			for (int j = 0; j < n_files; j++) {
				// each file has a name, a country, and some diseases
				char* name = read_from_socket(fd);
				char* country = read_from_socket(fd);
				int n_diseases;
				read(fd, &n_diseases, sizeof(int));
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
			}
			// critical section for printing is over
			pthread_mutex_unlock(&printing);

		} else {
			// else, a query string is given from a client
			
		}
		// close the socket fd
		close(fd);
	}
	return NULL;
}

void server_operation(char* query_port, char* stats_port, int buffer_size, int num_threads) {
	// create a cyclic buffer to store the file descriptors from the connections that we are going to serve
	Buffer buff = initialize_buffer(buffer_size);
	// initialize the hash table of the directories
	dirs_to_workers = hash_create(HASH_SIZE, hash_strings, BUCKET_SIZE, free);
	// allocate space to store the thread ids
	pthread_t* thread_ids = malloc(num_threads * sizeof(*thread_ids));
	if (thread_ids == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	// first things first: create n threads in order to serve simulatneously
	for (int i = 0; i < num_threads; i++) {
		int res = pthread_create(&thread_ids[i], NULL, slave_thread_operate, buff);
		if (res) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}
	// initialize our service. We are going to hear in 2 ports: one for stats and one for queries
	struct sockaddr_in server, client;
	socklen_t client_len;
	struct sockaddr* serverptr = (struct sockaddr*) &server;
	struct sockaddr* clientptr = (struct sockaddr*) &client;
	struct hostent* rem;
	int q_port = atoi(query_port);
	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(q_port);
	if (bind(sock, serverptr, sizeof(server)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	if (listen(sock, 10) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "Server listening....\n");
	// serve forever
	while (true) {
		int newsock;
		if ((newsock = accept(sock, clientptr, &client_len)) < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		// place the new socket fd in our buffer
		place(buff, newsock);
		// the buffer is not empty anymore (if it was)
		pthread_cond_signal(&cond_nonempty);
	}
}
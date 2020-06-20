#include "buffer.h"
#include "common_types.h"
#include "common_functions.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
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

// global list to store all the workers' ports
List workers;

// gloval variable for the ip that our workers are at
struct sockaddr_in workers_ip;

// declaration of the menu function for the server
void menu(char* instruction, int fd);

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
			// read the port from the worker
			read(fd, &port, sizeof(int));
			int* w_port = malloc(sizeof(*w_port));
			*w_port = htons(port);
			// add the port to the list
			list_insert(workers, w_port);
			// read the dirs from the worker
			int n_dirs;
			read(fd, &n_dirs, sizeof(int));
			for (int i = 0; i < n_dirs; i++) {
				char* country = read_from_socket(fd);
				hash_insert(dirs_to_workers, country, w_port);
			}
			// print all the stats provided by the worker
			// first thing: how many files the worker reported
			int n_files;
			read(fd, &n_files, sizeof(int));
			// lock the printing semaphore, so we do not have many threads printing in the stdout
			pthread_mutex_lock(&printing);
			for (int j = 0; j < n_files; j++) {
				// each file has a name, a country, and some diseases
				char* name = read_from_socket(fd);
				char* country = read_from_socket(fd);
				int n_diseases;
				read(fd, &n_diseases, sizeof(int));
				fprintf(stderr, "%s\n%s\n", name, country);
				// for each disease
				for (int k = 0; k < n_diseases; k++) {
					// parse the stats
					char* disease = read_from_socket(fd);
					fprintf(stderr, "%s\n", disease);
					free(disease);
					char* info = read_from_socket(fd);
					fprintf(stderr, "%s\n", info);
					free(info);
				}
				fprintf(stderr, "\n");
				// no leaks!
				free(name); 
				free(country);
			}
			// critical section for printing is over
			pthread_mutex_unlock(&printing);

		} else if (type == 'c')	{
			// else, a query string is given from a client
			char* query = read_from_socket(fd);
			// call the menu to answer the query, and to write the result back to the client
			menu(query, fd);
		}
		// close the socket fd
		close(fd);
		fd = -1;
		if (sig_int_raised) {
			fprintf(stderr, "Exiting thread");
			pthread_exit(NULL);
		}
	}
	return NULL;
}

void server_operation(char* query_port, char* stats_port, int buffer_size, int num_threads) {
	// create a cyclic buffer to store the file descriptors from the connections that we are going to serve
	Buffer buff = initialize_buffer(buffer_size);
	// initialize the hash table of the directories
	dirs_to_workers = hash_create(HASH_SIZE, hash_strings, BUCKET_SIZE, free);
	// initialize the workers' list
	workers = create_list(compare_ints, NULL);
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
	struct sockaddr_in s_server, s_client;
	socklen_t s_client_len = sizeof(struct sockaddr_in);
	struct sockaddr* s_serverptr = (struct sockaddr*) &s_server;
	struct sockaddr* s_clientptr = (struct sockaddr*) &s_client;
	int s_port = atoi(stats_port);
	int s_sock;
	if ((s_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	s_server.sin_family = AF_INET;
	s_server.sin_addr.s_addr = htonl(INADDR_ANY);
	s_server.sin_port = htons(s_port);
	if (bind(s_sock, s_serverptr, sizeof(s_server)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	// we are going to allow up to 10 connections
	if (listen(s_sock, SOMAXCONN) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "Server listening for statistics....\n");
	// no for the queries...
	struct sockaddr_in q_server, q_client;
	socklen_t q_client_len = sizeof(struct sockaddr_in);
	struct sockaddr* q_serverptr = (struct sockaddr*) &q_server;
	struct sockaddr* q_clientptr = (struct sockaddr*) &q_client;
	int q_port = atoi(query_port);
	int q_sock;
	if ((q_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	q_server.sin_family = AF_INET;
	q_server.sin_addr.s_addr = htonl(INADDR_ANY);
	q_server.sin_port = htons(q_port);
	if (bind(q_sock, q_serverptr, sizeof(q_server)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	// we are going to allow up to 10 connections
	if (listen(q_sock, SOMAXCONN) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "Server listening for queries....\n");
	// we are going to use select to see which socket to take info from
	fd_set active, read;
	// initialize the sets of the fds
	FD_ZERO(&active);
	// add the 2 file desc in our set
	FD_SET(q_sock, &active);
	FD_SET(s_sock, &active);
	// serve forever
	while (true) {
		read = active;
		// select the file desc containing data
		if (select(FD_SETSIZE, &read, NULL, NULL, NULL) < 0) {
			perror("select:");
			exit(EXIT_FAILURE);
		}
		int newsock;
		// queery fd is ready
		if (FD_ISSET(q_sock, &read)) {
			// accept the connection
			if ((newsock = accept(q_sock, q_clientptr, &q_client_len)) < 0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}
		}
		// stats fd is ready
		if (FD_ISSET(s_sock, &read)) {
			// accept the connection
			if ((newsock = accept(s_sock, s_clientptr, &s_client_len)) < 0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}
			// the stats connection comes from a worker, and we want to learn his ip
			socklen_t len = sizeof(workers_ip);
			getpeername(newsock, (struct sockaddr *) &workers_ip, &len);
		}
		// first thing: catch a possible signal
		if (sig_int_raised) {
			fprintf(stderr, "Exiting the server...");
			// destroy the buffer
			destroy_buffer(buff);
			// join all the threads
			for (int i = 0; i < num_threads; i++) {
				int res = pthread_join(thread_ids[i], NULL);
				if (res) {
					perror2("pthread_join", res);
					exit(EXIT_FAILURE);
				}
			}
			// return to the main function to wrap it up
			return;
		}
		// place the new socket fd in our buffer
		place(buff, newsock);
		// the buffer is not empty anymore (if it was)
		pthread_cond_signal(&cond_nonempty);
	}
}
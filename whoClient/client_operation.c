#include "common_types.h"
#include "common_functions.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// struct that contains the info needed for each thread
struct init_info {
	char* server_ip;
	char* server_port;
	char* query;
};

typedef struct init_info* info;

// global barrier variable that is accessible from all of the threads
pthread_barrier_t barrier;
// use a macro to initialize our mutex
pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;

Pointer thread_operate(Pointer q) {
	// wait for all threads to initialize
	pthread_barrier_wait(&barrier);
	// lock the mutex so we are sure that only this thread is connexting to the server
	pthread_mutex_lock(&counter_lock);
	info wrap = (info)q;
	struct sockaddr_in server;
	struct sockaddr* server_ptr = (struct sockaddr*)&server;
	struct hostent* rem;
	int sock;
	// create a socket and check for a possible error
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	// find the server
	if ((rem = gethostbyname(wrap->server_ip)) == NULL) {
		herror("gethostbyname");
		exit(EXIT_FAILURE);
	}
	// convert the port to an integer for later
	int port = atoi(wrap->server_port);
	// specify that we are talking about an internet domain
	server.sin_family = AF_INET;
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(port);
	// try to connect
	if (connect(sock, server_ptr, sizeof(server)) < 0) {
		perror("connect");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "Connection to server initialized by thread %ld", pthread_self());
	// write the instruction to the server
	int res = write(sock, wrap->query, strlen(wrap->query) + 1);
	if (res < 0) {
		perror("write");
		exit(EXIT_FAILURE);
	}
	char answer[50];
	// read the answer from the server
	res = read(sock, &answer, 50);
	// print the answer;
	fprintf(stdout, "%s\n", answer);
	// operation done! unlock the mutex
	pthread_mutex_unlock(&counter_lock);
	// the thread can now exit
	// TODO: Maybe pass a return parameter to inform main
	pthread_exit(NULL);
}

void client_operation(int n_threads, char* query_file, char* server_ip, char* server_port) {
	// open the query file
	FILE* queries = fopen(query_file, "r");
	int lines = nlines(queries);
	// specify how many loops must be done
	int n_loops = lines / n_threads;
	// and how many threads we will create in the last loop
	int remainder = lines % n_threads;
	// do n_loops full loops by creating all the threads
	for (int i = 0; i < n_loops + 1; i++) {
		int curr_threads;
		// if we are in the first n-1 loops, we want the maximum looops
		if (i < n_loops) {
			curr_threads = n_threads;
		}
		// else, only the remaining ones
		else {
			// check if there is at least one
			if (remainder)
				curr_threads = remainder;
			// else just breadk the loop
			else 
				break;
		}
		// allocate space to store the thread ids
		pthread_t* thread_ids = malloc(curr_threads * sizeof(*thread_ids));
		if (thread_ids == NULL) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		pthread_barrier_init(&barrier, NULL, curr_threads);
		char* query;
		for (int j = 0; j < curr_threads; j++) {
			info wrap = malloc(sizeof(*wrap));
			// read the query from the file given
			int read = getline(&query, NULL, queries);
			// catch a possible error
			if (!read) {
				perror("getline:");
				exit(EXIT_FAILURE);
			}
			// initialize the info thar we're gonna send to the threads
			wrap->query = query;
			wrap->server_ip = server_ip;
			wrap->server_port = server_port;
			// create a new thread
			int res = pthread_create(&thread_ids[i], NULL, thread_operate, wrap);
			// catch a possible error
			if (res) {
				perror2("pthread_create", res);
				exit(EXIT_FAILURE);
			}
		}
		free(query);
		// wait for each thread to finish its execution
		for (int j = 0; j < curr_threads; j++) {
			int res = pthread_join(thread_ids[i], NULL);
			if (res) {
				perror2("pthread_join", res);
				exit(EXIT_FAILURE);
			}
		}
		pthread_barrier_destroy(&barrier);
	}
	fclose(queries);
	// everything is perfect! return to wrap it up!
	return;
}

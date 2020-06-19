#include "common_types.h"
#include "common_functions.h"
#include "HashTable.h"
#include "BalancedTree.h"
#include "Patient.h"
#include "LinkedList.h"
#include "WorkerMenu.h"
#include <dirent.h> 
#include "Queries.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// definition of parsing function
void parser(char* input_dir, List dirs, int writing, HashTable patients, HashTable diseases_hash);

// global variable to catch a signal
volatile sig_atomic_t sig_int_raised;

// catch an insterupt
void catch_int(int signo) {
	sig_int_raised = signo;
}

// function to send a signal to parent if the connection to the server fails
void perror_and_sig(char* err) {
	// print the error
	perror(err);
	// send a signal to the master
	kill(getppid(), SIGUSR1);
	// wait to be killed
	while (true);
}

int main(int argc, char* argv[]) {
	// iniialize a signal set
	static struct sigaction act_int;
	// handle our signals with our function, in order to catch them
	act_int.sa_handler = catch_int;
	sigfillset(&(act_int.sa_mask));
	// want to handle SIGINT and SIGQUIT with this function
	sigaction(SIGINT, &act_int, NULL);
	sigaction(SIGQUIT, &act_int, NULL);

	// Get the pipe names from the args, and open them:
	// 1st for writing, 2nd for reading
	int reading;
	int buff_size = atoi(argv[2]);
	char* input_dir = concat(argv[3], "/");
	reading = open(argv[1], O_RDONLY, 0666);
	// check for possible errors while opening the pipes
	if (reading == -1) {
		perror_and_sig("open");
	}
	// Create a hash table to store all the different diseases
	HashTable diseases_hash = hash_create(HASH_SIZE, hash_strings, BUCKET_SIZE, balanced_tree_destroy);
	// We are going to use one extra hashtable, in order to quickly search if the patient allready exists
	// We are going to pass destroy patient as a destroy func, in order to free all the memory occupied
	// by the patients when we are done.
	HashTable patients = hash_create(HASH_SIZE, hash_strings, sizeof(Patient) + sizeof(Pointer), destroy_patient);
	// create a list to store all the directories that the worker must handle
	List dirs = create_list(compare_strings, free); //TODO: Maybe NULL for destroy
	char* str;
	// read the dirs from the pipe
	while (true) {
		str = read_from_pipe(reading, buff_size);
		// break when "end" is sent by the parent
		if (! strcmp(str, "end")) 
			break;
		list_insert(dirs, str);
	}
	// Now, we must open a new conection as a server, in order to answer to queries
	struct sockaddr_in q_server, q_client;
	struct sockaddr* q_serverptr = (struct sockaddr*)&q_server;
	struct sockaddr* q_clientptr = (struct sockaddr*)&q_client;
	int q_sock;
	// create the socket for the queries
	if ((q_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror_and_sig("socket");
		exit(EXIT_FAILURE);
	}
	// initialize our internet domain
	q_server.sin_family = AF_INET;
	q_server.sin_addr.s_addr = htonl(INADDR_ANY);
	// initialize with port 0, so we let the os determine which port it is gonna give us
	q_server.sin_port = htons(0);
	// bind the socket to the address
	if (bind(q_sock, q_serverptr, sizeof(q_server)) < 0) {
		perror_and_sig("bind");
	}
	// listen for incoming connections
	if (listen(q_sock, MAX_ALIVE_CONNECTIONS) < 0) {
		perror_and_sig("listen");
	}
	// learn the port that was assigned by the os
	struct sockaddr* temp = q_serverptr;
	socklen_t len = sizeof(temp);
	getsockname(q_sock, temp, &len);
	struct sockaddr_in* temp_in = (struct sockaddr_in*)temp;
	int queries_port = temp_in->sin_port;
	// also read the server's ip and port from the pipe
	char* server_ip = read_from_pipe(reading, buff_size);
	char* server_port = read_from_pipe(reading, buff_size);
	// proceed only if the worker is assigned with at least one country
	if (!is_empty(dirs)) {
		// initiate a connection with the server in order to send :
		// 1) a port that we will listen to for queries
		// 2) the countries that we are responsible for
		// 3) the stats from the files that we parsed

		// initiate the connection to the server
		struct sockaddr_in server;
		struct sockaddr* server_ptr = (struct sockaddr*)&server;
		struct hostent* rem;
		int sock;
		// create a socket and check for a possible error
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror_and_sig("socket");
		}
		// find the server
		if ((rem = gethostbyname(server_ip)) == NULL) {
			perror_and_sig("gethostbyname");
		}
		// specify that we are talking about an internet domain
		server.sin_family = AF_INET;
		memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
		int s_port = atoi(server_port);
		server.sin_port = htons(s_port);
		// try to connect
		if (connect(sock, server_ptr, sizeof(server)) < 0) {
			perror_and_sig("connect");
		}
		fprintf(stderr, "Connection to server initialized by worker %d\n", getpid());
		// inform the server that we are going to send worker info
		write(sock, "w", sizeof(char));
		// write the query port to the server
		int res = write(sock,&queries_port, sizeof(int));
		if (res < 0) {
			perror_and_sig("write");
		}
		// write how many dirs we are going to send
		write(sock, &(dirs->size), sizeof(int));
		// write the dirs to inform the server
		for (int i = 0; i < dirs->size; i++) {
			char* nth = list_nth(dirs, i);
			write_to_socket(sock, nth, strlen(nth));
		}
		// call the function to parse the whole input and send the stats to the server
		parser(input_dir, dirs, sock, patients, diseases_hash);
		// we are ready to answer to queries!
		fprintf(stderr, "\n");
		fprintf(stderr, "Worker %d ready to answer queries!\n", getpid());
		// read queries until we break
		while (true) {
			// accept a new connection
			int new_sock;
			socklen_t client_len;
			if ((new_sock = accept(q_sock, q_clientptr, &client_len)) < 0) {
				perror_and_sig("accept");
			}
			// read the query form the socket
			char* query = read_from_socket(new_sock);
			// check for a possible signal
			// If a sigint or sigquit are caught
			if (sig_int_raised) {
				// free out data structures
				hash_destroy(diseases_hash);
				destroy_list(dirs);
				// close the fd
				close(reading);
				// exit
				exit(EXIT_SUCCESS);
			}
			if (query != NULL) {
				// call the menu to analyze the query and write the result to the socket
				worker_menu(query, dirs, patients, diseases_hash, new_sock);
				// close the socket
				close(new_sock);
			}
		} 
	} else {
		// if no countries are assigned, just wait until the parent sends a SIGKILL
		while(true);
		// check for a possible signal
		// If a sigint or sigquit are caught
		if (sig_int_raised) {
				// free out data structures
				hash_destroy(diseases_hash);
				destroy_list(dirs);
				// close the fd
				close(reading);
				// exit
				exit(EXIT_SUCCESS);
		}
	}
}
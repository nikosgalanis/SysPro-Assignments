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

void catch_int(int signo) {
	sig_int_raised = signo;
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
	int reading, writing;
	int buff_size = atoi(argv[3]);
	char* input_dir = concat(argv[4], "/");
	writing = open(argv[1], O_WRONLY, 0666);
	reading = open(argv[2], O_RDONLY, 0666);
	// check for possible errors while opening the pipes
	if (reading == -1 || writing == -1) {
		perror("open");
	}
	// if "init is given, then during parsing we must print the stats"
	bool print_stats = (strcmp(argv[5], "init") == 0);
	// Variables to stroe statistics for records.
	int success = 0; int failed = 0;
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
		fprintf(stderr, "%s\n", str);
		list_insert(dirs, str);
	}
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
			perror("socket");
			exit(EXIT_FAILURE);
		}
		// find the server
		if ((rem = gethostbyname(server_ip)) == NULL) {
			herror("gethostbyname");
			exit(EXIT_FAILURE);
		}
		// convert the port to an integer for later
		int port = atoi(server_port);
		// specify that we are talking about an internet domain
		server.sin_family = AF_INET;
		memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
		server.sin_port = htons(port);
		// try to connect
		if (connect(sock, server_ptr, sizeof(server)) < 0) {
			perror("connect");
			exit(EXIT_FAILURE);
		}
		fprintf(stdout, "Connection to server initialized by worker %d", getpid());
		// write the query port to the server
		char* zero = "0";
		int res = write(sock,zero, strlen(zero) + 1);
		if (res < 0) {
			perror("write");
			exit(EXIT_FAILURE);
		}
		// call the function to parse the whole input and send the stats to the server
		parser(input_dir, dirs, sock, patients, diseases_hash);
		// Now, we must open a new conection as a server, in order to answer to queries
		struct sockaddr_in q_server, q_client;
		struct sockaddr* q_serverptr = (struct sockaddr*)&q_server;
		struct sockaddr* q_clientptr = (struct sockaddr*)&q_client;
		struct hostent* q_rem;
		int q_sock;
		// create the socket for the queries
		if ((q_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("socket");
			exit(EXIT_FAILURE);
		}
		// initialize our internet domain
		q_server.sin_family = AF_INET;
		q_server.sin_addr.s_addr = htonl(INADDR_ANY);
		// listen to the port that we've chose
		q_server.sin_port = htons(0);
		// bind the socket to the address
		if (bind(q_sock, q_serverptr, sizeof(q_server)) < 0) {
			perror("bind");
			exit(EXIT_FAILURE);
		}
		// listen for incoming connections
		if (listen(q_sock, MAX_ALIVE_CONNECTIONS) < 0) {
			perror("listen");
			exit(EXIT_FAILURE);
		}
		// read queries until we break
		while (true) {
			// accept a new connection
			int new_sock;
			socklen_t client_len;
			if ((new_sock = accept(q_sock, q_clientptr, &client_len)) < 0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}
			char query[100];
			// read the query form the socket
			read(new_sock, query, 100);
			// check for a possible signal
			// If a sigint or sigquit are caught
			if (sig_int_raised) {
				// Just goto the exiting procedure
				goto EXIT_IF;
			}
			if (query != NULL) {
				// check if an exit command is given
				if (strstr(query, "/exit")) {
				EXIT_IF: 	fprintf(stderr, "Child exiting\n");
					// TODO: Maybe add to a function instead
					// free the memory occupied by our data structures
					hash_destroy(diseases_hash);
					// free the list of the countries given
					destroy_list(dirs);
					// inform the parent that we've ended our job
					write_to_pipe(writing, buff_size, "ready");
					// close the pipes
					close(reading); close(writing);
					//TODO: Maybe close the server
					// if we are here because of an interupt, just exit
					if (sig_int_raised)
						exit(EXIT_SUCCESS);
					// Finally, wait forever until the parent sends a SIGKILL that will exit the worker
					while(true);
				}
				// call the menu to analyze the query and write the result to the socket
				bool result = worker_menu(query, dirs, patients, diseases_hash, writing, buff_size);
				// close the socket
				close(new_sock);
			}
		} 
	} else {
		// if no countries are assigned, just wait until the parent sends a SIGKILL
		while(true);
	}
}
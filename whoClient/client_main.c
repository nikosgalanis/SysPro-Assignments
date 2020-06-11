#include "common_types.h"

// prototype for our operation function
void client_operation(int n_threads, char* query_file, char* server_ip, char* server_port);

int main(int argc, char* argv[]) {
	int num_threads;
	char* query_file, *server_port, *server_ip;
	// check the arguments given
	if (argc != 9) {
		fprintf(stderr, "Usage: ./whoClient –q queryFile -w numThreads –sp servPort –sip servIP");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[1], "-q")) {
		query_file = strdup(argv[2]);
	} else {
		fprintf(stderr, "Usage: ./whoClient –q queryFile -w numThreads –sp servPort –sip servIP");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[3], "-w")) {
		// TODO: Maybe avoid many threads
		num_threads = atoi(argv[4]);
	} else {
		fprintf(stderr, "Usage: ./whoClient –q queryFile -w numThreads –sp servPort –sip servIP");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[5], "-sp")) {
		server_port = strdup(argv[6]);
	} else {
		fprintf(stderr, "Usage: ./whoClient –q queryFile -w numThreads –sp servPort –sip servIP");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[7], "-sip")) {
		server_ip = strdup(argv[8]);
	} else {
		fprintf(stderr, "Usage: ./whoClient –q queryFile -w numThreads –sp servPort –sip servIP");
		exit(EXIT_FAILURE);
	}
	// call the operate function in order for the client to do his job
	client_operation(num_threads, query_file, server_ip, server_port);
	// free those in order to not have any leaks
	free(query_file);
	free(server_ip);
	free(server_port);
	// everything is sound!
	exit(EXIT_SUCCESS);
}
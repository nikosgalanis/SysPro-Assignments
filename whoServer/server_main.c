#include "common_types.h"

// prototype for our operation function
void server_operation(int n_threads, char* query_file, char* server_ip, char* server_port);

int main(int argc, char* argv[]) {
	int num_threads, buffer_size;
	char* stats_port, *query_port;
	// check the arguments given
	if (argc != 9) {
		fprintf(stderr, "Usage: ./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[1], "-q")) {
		query_port = strdup(argv[2]);
	} else {
		fprintf(stderr, "Usage: ./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[3], "-s")) {
		stats_port = atoi(argv[4]);
	} else {
		fprintf(stderr, "Usage: ./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[5], "-w")) {
		num_threads = atoi(argv[6]);
	} else {
		fprintf(stderr, "Usage: ./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[7], "-b")) {
		buffer_size = atoi(argv[8]);
	} else {
		fprintf(stderr, "Usage: ./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize");
		exit(EXIT_FAILURE);
	}
	// call the operate function in order for the client to do his job
	client_operation(num_threads, buffer_size, query_port, stats_port);
	// free those in order to not have any leaks
	free(stats_port);
	free(query_port);
	// everything is sound!
	exit(EXIT_SUCCESS);
}
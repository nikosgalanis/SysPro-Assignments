#include "common_types.h"

// prototype for our operation function
void server_operation(char* query_port, char* stats_port, int buffer_size, int num_threads);

int main(int argc, char* argv[]) {
	int num_threads, buffer_size;
	char* stats_port, *query_port;
	// check the arguments given
	if (argc != 9) {
		fprintf(stderr, "Usage: ./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize\n");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[1], "-q")) {
		query_port = strdup(argv[2]);
	} else {
		fprintf(stderr, "Usage: ./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize\n");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[3], "-s")) {
		stats_port = strdup(argv[4]);
	} else {
		fprintf(stderr, "Usage: ./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize\n");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[5], "-w")) {
		num_threads = atoi(argv[6]);
	} else {
		fprintf(stderr, "Usage: ./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize\n");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[7], "-b")) {
		buffer_size = atoi(argv[8]);
	} else {
		fprintf(stderr, "Usage: ./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize\n");
		exit(EXIT_FAILURE);
	}
	// call the operate function in order for the client to do his job
	server_operation(query_port, stats_port, buffer_size, num_threads);
	// free those in order to not have any leaks
	free(stats_port);
	free(query_port);
	// everything is sound!
	exit(EXIT_SUCCESS);
}
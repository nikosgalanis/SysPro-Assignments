#include "common_types.h"

int main(int argc, char* argv[]) {
	int n_workers, buffer_size;
	char* input_dir, *server_ip, *server_port;
	// check the arguments given
	if (argc != 11) {
		fprintf(stderr, "Usage: ./master –w numWorkers -b bufferSize –s serverIP –p serverPort -i input_dir");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[1], "-w")) {
		n_workers = atoi(argv[2]);
	} else {
		fprintf(stderr, "Usage: ./master –w numWorkers -b bufferSize –s serverIP –p serverPort -i input_dir");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[3], "-b")) {
		buffer_size = atoi(argv[4]);
	} else {
		fprintf(stderr, "Usage: ./master –w numWorkers -b bufferSize –s serverIP –p serverPort -i input_dir");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[5], "-s")) {
		server_ip = strdup(argv[6]);
	} else {
		fprintf(stderr, "Usage: ./master –w numWorkers -b bufferSize –s serverIP –p serverPort -i input_dir");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[7], "-p")) {
		server_port = strdup(argv[8]);
	} else {
		fprintf(stderr, "Usage: ./master –w numWorkers -b bufferSize –s serverIP –p serverPort -i input_dir");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[9], "-w")) {
		input_dir = strdup(argv[10]);
	} else {
		fprintf(stderr, "Usage: ./master –w numWorkers -b bufferSize –s serverIP –p serverPort -i input_dir");
		exit(EXIT_FAILURE);
	}
	
	// free those in order to not have any leaks
	free(input_dir);
	free(server_ip);
	free(server_port);
	// everything is sound!
	exit(EXIT_SUCCESS);
}
#include "common_types.h"

int main(int argc, char const *argv[]) {
    char* input_dir;
	if (argc < 7 || argc > 7) {
		printf("Use as ./diseaseAggregator –w numWorkers -b bufferSize -i input_dir\n");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[1], "-w")) {
		int n_workers = atoi(argv[2]);
	} else {
		printf("Use as ./diseaseAggregator –w numWorkers -b bufferSize -i input_dir\n");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[3], "-b")) {
		int buffer_size = atoi(argv[4]);
	} else {
		printf("Use as ./diseaseAggregator –w numWorkers -b bufferSize -i input_dir\n");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[5], "-i")) {
		input_dir = strdup(argv[6]);
	} else {
		printf("Use as ./diseaseAggregator –w numWorkers -b bufferSize -i input_dir\n");
		exit(EXIT_FAILURE);
	}

    free(input_dir);
    exit(EXIT_SUCCESS);
}
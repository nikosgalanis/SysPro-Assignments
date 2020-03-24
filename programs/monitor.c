#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GlobalStructures.h"

int main(int argc, char const *argv[]) {
	int bucket_size;
	char* in_file;
	if (argc < 9 || argc > 9) {
		printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[1], "-p")) {
		in_file = strdup(argv[2]);
	} else {
		printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[3], "-h1")) {
		num_diseases = atoi(argv[4]);
	} else {
		printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[5], "-h2")) {
		num_countries = atoi(argv[6]);
	} else {
		printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
		exit(EXIT_FAILURE);
	}
	if (! strcmp(argv[7], "-b")) {
		bucket_size = atoi(argv[8]);
	} else {
		printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
		exit(EXIT_FAILURE);
	}
	parse_input(in_file, bucket_size);
	monitor_menu();

	free(in_file);
	exit(EXIT_SUCCESS);
}

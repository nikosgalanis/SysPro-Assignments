#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[]) {
    char* in_file;
    int disease_hash_size, country_hash_size;
    if (! strcmp(argv[1], "-p")) {
        in_file = strdup(argv[2]);
        printf("%s\n", in_file);
    } else {
        printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
        exit(EXIT_FAILURE);
    }
    if (! strcmp(argv[3], "-h1")) {
        disease_hash_size = atoi(argv[4]);
    } else {
        printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
        exit(EXIT_FAILURE);
    }
    if (! strcmp(argv[5], "-h1")) {
        country_hash_size = atoi(argv[6]);
    } else {
        printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
        exit(EXIT_FAILURE);
    }


}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GlobalStructures.h"

int main(int argc, char const *argv[]) {
    char* in_file = malloc(strlen(argv[4]) * sizeof(char));
    int bucket_size;
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


// int compare_ints(Pointer a, Pointer b) {
// 	return *(int*)a - *(int*)b;
// }

// uint ihash_strings(void* key) {
// 	char* str = (char*)key;
//     int h = 0, a = 33;
// 	for (; *str != '\0'; str++)
// 		h = (a * h + *str);
// 	return h;
// }

// void destroy_ht(Pointer p) {
//     HashEntry h = (HashEntry)p;
//     if (h != NULL)
//         if (h->item != NULL)
//             free(h->item);
// }

// int main(void) {
//     HashTable ht = hash_create(100, ihash_strings, 50, free);
//     hash_insert(ht, "lala", NULL);
//     hash_insert(ht, "ldsala", NULL);
//     hash_insert(ht, "lalasda", NULL);
//     hash_insert(ht, "lalad", NULL);
//     hash_insert(ht, "lalaaaa", NULL);

//     hash_destroy(ht);
// }
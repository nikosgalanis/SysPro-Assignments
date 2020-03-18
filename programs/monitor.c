#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GlobalStructures.h"

// int main(int argc, char const *argv[]) {
//     char* in_file;
//     int disease_hash_size, country_hash_size, bucket_size;
//     if (argc < 9 || argc > 9) {
//         printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
//         exit(EXIT_FAILURE);
//     }
//     if (! strcmp(argv[1], "-p")) {
//         in_file = strdup(argv[2]);
//     } else {
//         printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
//         exit(EXIT_FAILURE);
//     }
//     if (! strcmp(argv[3], "-h1")) {
//         disease_hash_size = atoi(argv[4]);
//     } else {
//         printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
//         exit(EXIT_FAILURE);
//     }
//     if (! strcmp(argv[5], "-h2")) {
//         country_hash_size = atoi(argv[6]);
//     } else {
//         printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
//         exit(EXIT_FAILURE);
//     }
//     if (! strcmp(argv[7], "-b")) {
//         bucket_size = atoi(argv[8]);
//     } else {
//         printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
//         exit(EXIT_FAILURE);
//     }
//     parse_input(in_file, country_hash_size, disease_hash_size, bucket_size);
//     monitor_menu();


//     exit(EXIT_SUCCESS);
// }


int compare_ints(Pointer a, Pointer b) {
	return *(int*)a - *(int*)b;
}

uint ihash_strings(void* key) {
	char* str = (char*)key;
    int h = 0, a = 33;
	for (; *str != '\0'; str++)
		h = (a * h + *str);
	return h;
}

void destroy_ht(Pointer p) {
    HashEntry h = (HashEntry)p;
    if (h != NULL)
        if (h->item != NULL)
            free(h->item);
}

int main(void) {
    int* a = malloc(sizeof(int)); *a = 10;
    int* b = malloc(sizeof(int)); *b = 20;
    int* c = malloc(sizeof(int)); *c = 30;
    int* d = malloc(sizeof(int)); *d = 40;

    HashTable ht = hash_create(10, ihash_strings, 50, destroy_ht);
    HashEntry a1 = create_hash_entry("lala", a);
    HashEntry a2 = create_hash_entry("lada", b);
    HashEntry a3 = create_hash_entry("lasa", c);
    HashEntry a4 = create_hash_entry("lal", d);

    hash_insert(ht, a1);
    hash_insert(ht, a2);
    hash_insert(ht, a3);
    hash_insert(ht, a4);
    hash_destroy(ht);
}
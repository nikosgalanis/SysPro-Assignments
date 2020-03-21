#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GlobalStructures.h"

int main(int argc, char const *argv[]) {
    char* in_file;
    int disease_hash_size, country_hash_size, bucket_size;
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
        disease_hash_size = atoi(argv[4]);
    } else {
        printf("Use as ./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n");
        exit(EXIT_FAILURE);
    }
    if (! strcmp(argv[5], "-h2")) {
        country_hash_size = atoi(argv[6]);
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
    parse_input(in_file, country_hash_size, disease_hash_size, bucket_size);
    monitor_menu();


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
//     Heap heap = create_heap(NULL);
//     for (int i = 0; i < 1000; i++) {
//         heap_insert(heap, rand()%10000, "lala");
//     }
//     for (int i = 1; i < 1001; i++) {
//         HeapEntry h = pop(heap);
//         printf("%d\n", h->priority);
//     }

// }
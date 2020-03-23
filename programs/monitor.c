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
//     BalancedTree tree = create_balanced_tree(compare_ints, free);
//     Date aa = string_to_date("19-02-2020 ");
//     Date as = string_to_date("19-03-2020 ");
//     Date ad = string_to_date("19-04-2020 ");
//     Date ag = string_to_date("19-05-2020 ");
//     BalancedTreeEntry a1 = create_balanced_tree_entry(aa, NULL);
//     BalancedTreeEntry a2 = create_balanced_tree_entry(as, NULL);
//     BalancedTreeEntry a3 = create_balanced_tree_entry(ad, NULL);
//     BalancedTreeEntry a4 = create_balanced_tree_entry(ag, NULL);
//     balanced_tree_insert(tree, a1);
//     balanced_tree_insert(tree, a2);
//     balanced_tree_insert(tree, a3);
//     balanced_tree_insert(tree, a4);
//     Date d = string_to_date("20-2-2020");
//     printf("%d\n", total_nodes_grater_than(tree, &d, NULL, NULL));

// }
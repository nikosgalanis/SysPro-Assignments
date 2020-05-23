#include "common_types.h"
#include "BalancedTree.h"
#include "HashTable.h"
#include "LinkedList.h"

int compare_strings (Pointer a, Pointer b);
int compare(Pointer first, Pointer second);
uint hash_strings(void* key);
int nlines(FILE* input);
int n_words(char* str);
char* read_from_pipe(int fd);
void write_to_pipe(int fd, int buff_size, char* info);
void print_list_contents(Pointer ent, Pointer d1, Pointer d2, Pointer d3, Pointer d4);
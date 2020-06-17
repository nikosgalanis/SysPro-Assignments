#include "common_types.h"
#include "BalancedTree.h"
#include "HashTable.h"
#include "LinkedList.h"

int compare_strings (Pointer a, Pointer b);
int compare(Pointer first, Pointer second);
uint hash_strings(void* key);
int nlines(FILE* input);
int n_words(char* str);
char* nth_word(char* s, int n);
char* read_from_pipe(int fd, int buff_size);
char* concat(char* str1, char* str2);
char* itoa(int n);
void write_to_pipe(int fd, int buff_size, char* info);
char* read_from_socket(int fd);
int write_to_socket(int fd, void* buff, int len);
void print_list_contents(Pointer ent, Pointer d1, Pointer d2, Pointer d3, Pointer d4);
void print_countries(Pointer ent, Pointer d1, Pointer d2, Pointer d3, Pointer d4);
int get_pos_from_pid(int pid, int* workers, int n_workers);
int n_files_in_worker(char* path, List countries);
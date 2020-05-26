#include "common_functions.h"

// compare function for strings
int compare_strings (Pointer a, Pointer b) {
	return strcmp((char*) a, (char*)b);
}

// compare function for our entries
int compare(Pointer first, Pointer second) {
	TreeEntry entry1 = (TreeEntry)first;
	TreeEntry entry2 = (TreeEntry)second;
	return compare_dates(entry1->date, entry2->date);
}

// remake of the concat function because c is useless
char* concat(char* str1, char* str2) {
	char* res = malloc(strlen(str1) + strlen(str2) + 1);
    memcpy(res, str1, strlen(str1));
    memcpy(res + strlen(str1), str2, strlen(str2) + 1);
	return res;
}

// Remake of the itoa function, because gcc sucks
char* itoa(int n) {
    char* res = malloc(10);
    snprintf(res, 10,"%d", n);
    return res;
} 

// djb2 hash function, simple and quick
uint hash_strings(Pointer value) {
    uint hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;
    return hash;
}

// Count how many lines there are in a file
int nlines(FILE* input) {
	int n_lines = 0;
	while (!feof(input)) {
		char ch = fgetc(input);
		if (ch == '\n') {
			n_lines++;
		}
	}
	rewind(input);
	return n_lines;
}

int n_words(char* str) {
	char delim[3] = " \n";
	int n = 0;
	char* res = strtok(str, delim);
	while (res) {
		n++;
		res = strtok(NULL, delim);
	}
	return n;
}

char* nth_word(char* str, int n) {
	char delim[3] = " \n";
	char* res = strtok(str, delim);
	while (n) {
		n--;
		res = strtok(NULL, delim);
	}
	return res;
}
// Read a string from a pipe
char* read_from_pipe(int fd) {
	// find out how many bytes we want to read
	int n_bytes;
	read(fd, &n_bytes, sizeof(int));
	int read_count = 0;
    // Allocate a string to return (space for \0 is taken into account by the write function)
	char* res = malloc((n_bytes + 1) * sizeof(*res));
    // read until we read the whole message
	while (read_count < n_bytes) {
        char* curr = res + (read_count * sizeof(char));
	    // try read exactly n bytes from the pipe
        int old = read_count;
        // update the read_cocunt by the result of read
        read_count += read(fd, curr, n_bytes - old);
    }
    // finally, return the whole message
	return res;
}

// Write a sting to the pipe, by writing buff_size bytes each time
void write_to_pipe(int fd, int buff_size, char* info) {
	// exit violently if a buffsize of 0 is given
	assert(buff_size > 0);
	// Hold the number of the bytes in the message, so the reader will read easily
	int n_bytes = strlen(info);
	// Write the n of bytes in front of the message
	write(fd, &n_bytes, sizeof(int));
	// set how many times we must write to the buffer given the buff_size
	int times = (strlen(info) / buff_size) + 1;
	for (int i = 0; i < times; i++) {
		// the first n-1 times, write the full message
		if (i < times - 1) {
			char* current = info + (i * buff_size);
			write(fd, current, buff_size);
		}
		else {
			// the last time, write the remainder of the bytes in the file (if it is grater than 0)
			int remainder = n_bytes % buff_size;
			if (remainder) {
				char* current = info + (i * buff_size);
				write(fd, current, (remainder));
			}
		}
	}
}

// Function to traverse our dirs_to_workers hash table
void print_list_contents(Pointer ent, Pointer d1, Pointer d2, Pointer d3, Pointer d4) {
    HashEntry entry = (HashEntry)ent;
    if (entry) {
		printf("%12s %d\n", entry->key, *(int*)entry->item);
    }
}

// Assign a pid to a workers's id
int get_pos_from_pid(int pid, int* workers, int n_workers) {
	for (int i = 0; i < n_workers; i++) {
		if (pid == workers[i])
			return i;
	}
	return FAILED;
}

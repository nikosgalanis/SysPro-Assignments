#include "common_functions.h"
#include <dirent.h>
#include <errno.h>

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
	char* res = malloc(strlen(str1) + strlen(str2) + 10);
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

// Get the number of the words in a string
int n_words(char* s) {
	char delim[3] = " \n";
	int n = 0;
    char* str = strdup(s);
	char* res = strtok(str, delim);
    // read until we reach null
	while (res) {
		n++;
		res = strtok(NULL, delim);
	}
    // no leaks here
    free (str);
	return n;
}

// Get the n-th word of a string
char* nth_word(char* s, int n) {
    char* str = strdup(s);
    int nw = n_words(str);
    char* arr[nw];
	char delim[3] = " \n";
	arr[0] = strtok(str, delim);
    int i = 1;
	while (i < n) {
		arr[i] = strtok(NULL, delim);
        i++;
    }
    char* ret = strdup(arr[i-1]);
    free(str);
	return ret;
}

// Read a string from a pipe
char* read_from_pipe(int fd, int buff_size) {
	// find out how many bytes we want to read
	int n_bytes;
	int n = read(fd, &n_bytes, sizeof(int));
	// if read fails due to an interupt, then just return null, we'll handle it later
	if (n == -1 && (errno == EINTR)) {
		return NULL;
	}
    // Allocate a string to return (space for \0 is taken into account by the write function)
	char* info = malloc((n_bytes + 1) * sizeof(*info));
	// set how many times we must read from the buffer given the buff_size
    int times = (n_bytes / buff_size) + 1;
    for (int i = 0; i < times; i++) {
        // the first n-1 times, read the full message
        if (i < times -1) {
            char* current = info + (i * buff_size);
            read(fd, current, buff_size);
        } else {
            // the last time, read the remainder of the bytes in the file (if it is grater than 0)
            int remainder = n_bytes % buff_size;
            if (remainder) {
                char* current = info + (i * buff_size);
                read(fd, current, remainder);
            }
        }
    }
	info[n_bytes] = '\0';
    // finally, return the whole message
	return info;
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
		printf("%15s %d\n", entry->key, *(int*)entry->item);
    }
}
// Function to traverse our dirs_to_workers hash table
void print_countries(Pointer ent, Pointer f, Pointer d2, Pointer d3, Pointer d4) {
    FILE* file = (FILE*)f;
	HashEntry entry = (HashEntry)ent;
    if (entry) {
		fprintf(file, "%15s\n", entry->key);
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

// find out how many files are in a directory
int n_files_in_dir(char* path) {
	int count = 0;
	DIR* directory;
	struct dirent* entry;
	directory = opendir(path);
	while ((entry = readdir(directory)) != NULL) {
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			count++;
	}
	closedir(directory);
	return count;
}

// find out how many files a worker has
int n_files_in_worker(char* path, List countries) {
	int count = 0;
	for (int i = 0; i < countries->size; i++) {
		count += n_files_in_dir(concat(path, (char*)list_nth(countries, i)));
	}
	return count;
}

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

char* read_from_pipe(int fd, int buff_size) {
	bool read_anything = false;
	char* str = malloc(STRING_SIZE * sizeof(*str));
	char current[buff_size];
	while (read(fd, current, buff_size)) {
		strcat(str, current);
		read_anything = true;
	}
	if (!read_anything)
		free(str);
	return read_anything ? str : NULL;
}

void write_to_pipe(int fd, int buff_size, char* info) {
	int times = strlen(info) / buff_size + 1;
	for (int i = 0; i < times; i++) {
		char* current = info + (i * buff_size);
		write(fd, current, buff_size);
	}
}


void print_list_contents(Pointer ent, Pointer d1, Pointer d2, Pointer d3, Pointer d4) {
    HashEntry entry = (HashEntry)ent;
    if (entry) {
        List list = entry->item;
        for (int i = 0; i < list->size; i++) {
            printf("%12s %s\n", (char*)list_nth(list, i), entry->key);
        }
    }
}

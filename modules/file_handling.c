#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Count how many lines there are in a file
int nlines(FILE* input) {
    int lines = 0;
    while(!feof(input)) {
        char ch = fgetc(input);
        if(ch == '\n') {
            lines++;
        }
    }
    rewind(input);
    return lines;
}
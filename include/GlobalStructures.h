#pragma once

#include "common_types.h"
#include "Patient.h"
#include "Dates.h"
#include "HashTable.h"
#include "BalancedTree.h"

// Our global pointers to store the hash tables
HashTable diseaseHashTable;
HashTable countryHashTable;
HashTable patients;

int compare(Pointer first, Pointer second) {
    TreeEntry entry1 = (TreeEntry)first;
    TreeEntry entry2 = (TreeEntry)second;
    return compare_dates(entry1->date, entry2->date);
}

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

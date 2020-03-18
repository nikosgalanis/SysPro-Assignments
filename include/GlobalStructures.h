#pragma once

// Includes that must be made
#include "common_types.h"
#include "Patient.h"
#include "Dates.h"
#include "HashTable.h"
#include "BalancedTree.h"

// Our global pointers to store the hash tables
HashTable diseaseHashTable;
HashTable countryHashTable;
HashTable patients;

// Store the pointers to the strings in order to free them later
char** all_strings_from_file;

// number of lines in the file opened
int lines;
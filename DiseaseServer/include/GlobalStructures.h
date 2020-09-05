#pragma once

// Includes that must be made
#include "common_types.h"
#include "Patient.h"
#include "Dates.h"
#include "HashTable.h"
#include "BalancedTree.h"
#include "HeapUsingCBT.h"
#include "Queries.h"

// Our global pointers to stroe some sizes
int num_diseases, num_countries;

// Our global pointers to store the hash tables
HashTable diseaseHashTable;
HashTable countryHashTable;
HashTable patients;

// number of lines in the file opened
int lines;
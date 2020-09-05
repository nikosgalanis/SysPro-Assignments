#pragma once

#include "common_types.h"
#include "common_functions.h"
#include "HashTable.h"
#include "BalancedTree.h"
#include "HashTable.h"
#include "Dates.h"
#include "Patient.h"
#include "HeapUsingCBT.h"
#include <dirent.h> 
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

bool recordPatientExit(char* info, HashTable patients, char* exit_d);
int disease_frequency(char* virus, char* arg2, char* arg3, char* country, HashTable diseases_hash);
char* search_patient_record(char* r_id, HashTable patients);
int num_patient_admissions(char* disease, char* arg2, char* arg3, char* country, HashTable diseases_hash);
int num_patient_discharges(char* disease, char* arg2, char* arg3, char* country, HashTable diseases_hash);
void topk_age_ranges(int k, char* country, char* disease, char* day1, char* day2, HashTable diseases_hash, int write);

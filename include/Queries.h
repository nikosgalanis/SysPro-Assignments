#pragma once

#include "common_types.h"
#include "HashTable.h"
#include "BalancedTree.h"
#include "HashTable.h"
#include "Patient.h"
#include "HeapUsingCBT.h"
#include <dirent.h> 
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

// void globalDiseaseStats(char* info);
// void diseaseFrequency(char* info);
// void topk_Diseases(char* info);
// void topk_Countries(char* info);
// void insertPatientRecord(char* info);
// void recordPatientExit(char* info);
// void numCurrentPatients(char* info);
// void exit_monitor(void);

bool recordPatientExit(char* info, HashTable patients, char* exit_d);
int disease_frequency(char* info, HashTable diseases_hash);
char* search_patient_record(char* r_id, HashTable patients);
char* num_patient_admissions(char* info, HashTable diseases_hash);
void num_patient_discharges(char* info, HashTable diseases_hash);\
void topk_age_ranges(char* info, HashTable diseases_hash);
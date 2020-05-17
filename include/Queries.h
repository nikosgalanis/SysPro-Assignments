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

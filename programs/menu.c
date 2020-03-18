#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dates.h"
#include "Queries.h"

void monitor_menu() {
    char* input = malloc(100 * sizeof(char)); //TODO: change
    char delim[2] = " ";
    printf("Enter a new query\n");
    while (fgets(input, 100, stdin) != NULL) {
        char* instruction = strtok_r(input, delim, &input);
        if (!strcmp(instruction, "/globalDiseaseStats")) {
            globalDiseaseStats(input);
        }
        else if(!strcmp(instruction, "/diseaseFrequency")) {
            diseaseFrequency(input);
        }
        else if(!strcmp(instruction, "/topk-Diseases")) {
            // topk_Diseases(input);
        }
        else if(!strcmp(instruction, "/topk-Countries")) {
            // topk_Countries(input);
        }
        else if(!strcmp(instruction, "/insertPatientRecord")) {
            insertPatientRecord(input);
        }
        else if(!strcmp(instruction, "/recordPatientExit")) {
            recordPatientExit(input);
        }
        else if(!strcmp(instruction, "/numCurrentPatients")) {
            numCurrentPatients(input);
        }
        else if(!strcmp(instruction, "/exit")) {
            exit_monitor();
            exit(EXIT_SUCCESS);
        }
        else {
            printf("Query not recognized. Choose one of the default options\n");
        }
        printf("Enter a new query\n");
    }
}
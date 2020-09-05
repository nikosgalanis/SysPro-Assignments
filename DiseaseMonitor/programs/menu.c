#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dates.h"
#include "Queries.h"

void monitor_menu() {
	char input[STRING_SIZE];
	char delim[3] = " \n";
	printf("Enter a new query\n");
	while (fgets(input, STRING_SIZE, stdin) != NULL) {
		char* instruction = strtok(input, delim);
		char* info = strtok(NULL, "\n");
		if (!strcmp(instruction, "/globalDiseaseStats")) {
			globalDiseaseStats(info);
		}
		else if(!strcmp(instruction, "/diseaseFrequency")) {
			diseaseFrequency(info);
		}
		else if(!strcmp(instruction, "/topk-Diseases")) {
			topk_Diseases(info);
		}
		else if(!strcmp(instruction, "/topk-Countries")) {
			topk_Countries(info);
		}
		else if(!strcmp(instruction, "/insertPatientRecord")) {
			insertPatientRecord(info);
		}
		else if(!strcmp(instruction, "/recordPatientExit")) {
			recordPatientExit(info);
		}
		else if(!strcmp(instruction, "/numCurrentPatients")) {
			numCurrentPatients(info);
		}
		else if(!strcmp(instruction, "/exit")) {
			exit_monitor();
			return;
		}
		else {
			printf("Query not recognized. Choose one of the default options\n");
		}
		printf("\nEnter a new query\n");
	}
}
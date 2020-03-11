#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dates.h"

void monitor_menu() {
    // char* input = malloc(100 * sizeof(char)); //TODO: change
    // char delim[2] = " ";
    // printf("Enter a new query\n");
    // while (scanf("%[^\n]s",input) != EOF) {
    //     char* instruction = strtok(input, delim);
    //     if (!strcmp(instruction, "/globalDiseaseStats")) {
    //         printf("%s\n",input);
    //         char* d1 = strtok(NULL, delim);
    //         char* d2 = strtok(NULL, delim);
    //         printf("%s %s\n", d1,d2);
    //         if (d1 == NULL && d2 == NULL) {
    //             // globalDiseaseStats(NULL, NULL);
    //         } 
    //         else if (d1 != NULL && d2 != NULL) {
    //             Date date1 = string_to_date(d1);
    //             Date date2 = string_to_date(d2);
    //             printf("%d, %d\n", date1.year, date2.year);
    //             // globalDiseaseStats(date1, date2);
    //         }
    //         else {
    //             printf("Use as /globalDiseaseStats [date1 date2]");
    //         }
    //     }
    //     else if(!strcmp(instruction, "/diseaseFrequency")) {
    //         char* virus = strtok(NULL, delim);
    //         printf("%s\n", virus);
    //     }
    // }
}
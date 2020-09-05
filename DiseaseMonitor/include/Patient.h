#pragma once
#include "common_types.h"
#include "Dates.h"

typedef struct patient {
	char* id;
	char* first_name;
	char* last_name;
	char* disease;
	char* country;
	Date entry_date;
	Date exit_date;
} Patient;

Patient* create_patient(char* str);
void destroy_patient(Pointer p);
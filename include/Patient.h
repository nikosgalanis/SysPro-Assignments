#pragma once
#include "common_types.h"
#include "Dates.h"

typedef struct patient {
    int id;
    char first_name[32];
    char last_name[32];
    char disease[32];
    char country[32];
    Date entry_date;
    Date exit_date;
} Patient;
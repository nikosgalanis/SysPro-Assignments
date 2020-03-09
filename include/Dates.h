#pragma once

#include "common_types.h"

typedef struct date {
    int day;
    int month;
    int year;
} Date;

#define EOF_DATE (Date)0
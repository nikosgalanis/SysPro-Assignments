#pragma once

#include "common_types.h"

typedef struct date {
    int day;
    int month;
    int year;
} Date;

#define EOF_DATE (Date)0

Date string_to_date(char* d);
int compare_dates(Date date1, Date date2);

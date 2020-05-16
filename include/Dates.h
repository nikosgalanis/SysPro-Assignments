#pragma once

#include "common_types.h"

typedef struct date {
	int day;
	int month;
	int year;
} Date;

#define EOF_DATE (Date)0

bool empty_string(char* str);
Date string_to_date(char* d);
char* date_to_string(Date date);
int compare_dates(Date date1, Date date2);
bool check_valid_dates(Date date1, Date date2);
bool check_if_null_date(Date date);
bool check_equal_dates(Date date1, Date date2);
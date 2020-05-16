#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Dates.h"

bool empty_string(char* str) {
	if (str == NULL)
		return true;
	for (int i = 0;; i++) {
		if (str[i] != '\n' && str[i] != ' ' && str[i] != '\0' && str[i] != '-')
			return false;
		if (str[i] == '\0' || str[i] == '\0')
			return true;
	}
}

// Convert a DD-MM-YYYY string to an actual date
Date string_to_date(char* d) {
	// We know that our string type is __identical__ to DD-MM-YYYY.
	// so, we are going to take advantage of it
	if (empty_string(d)) {
		Date new_date = {.day = 0, .month = 0, .year = 0};
		return new_date;
	}
	char delim[3] = "-\n";
	char* day = strtok(d, delim);
	char* month = strtok(NULL, delim);
	char* year = strtok(NULL, delim);
	assert(day != NULL && month != NULL && year!= NULL);
	// Update the struct fields and return it
	Date new_date = {.day = atoi(day), .month = atoi(month), .year = atoi(year)};
	return new_date;
}

char* date_to_string(Date date) {
	if (check_if_null_date(date)) {
		return "--";
	}
	// take advantage of the date format
	char* res = malloc(11 * sizeof(*res));
	snprintf(res, 11, "%s-%s-%s", date.day, date.month, date.year);
	return res;
}

// Returns true if date is null (aka -)
bool check_if_null_date(Date date) {
	return date.day == 0 ? true : false;
}

// Checks if a pair of dates is valid, meaning the first is before the second
bool check_valid_dates(Date date1, Date date2) {
	return (compare_dates(date1, date2) < 0) ? true : false;
}

// Comparing 2 dates. Return -1 if 1st is prior(or euqal) to 2nd, or 1 otherwise
int compare_dates(Date date1, Date date2) {
	if (date1.year < date2.year)
		return -1;
	else if (date1.year > date2.year)
		return 1;
	else {
		if (date1.month < date2.month)
			return -1;
		else if (date1.month > date2.month)
			return 1;
		else {
			if (date1.day <= date2.day)
				return -1;
			else
				return 1;
		}
	}
}

bool check_equal_dates(Date date1, Date date2) {
	return (date1.day == date2.day && date1.month == date2.month
			&& date1.year == date2.year);
}
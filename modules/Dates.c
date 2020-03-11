#include <stdlib.h>
#include <string.h>

#include "Dates.h"

// Convert a DD-MM-YYYY string to an actual date
Date string_to_date(char* d) {
    // We know that our string type is __identical__ to DD-MM-YYYY.
    // so, we are going to take advantage of it
    char* day, *month, *year;
    char delim[2] = "-";
    char* day = strtok(d, delim);
    char* month = strtok(NULL, delim);
    char* year = strtok(NULL, delim);
    // Update the struct fields and return it
    Date new_date = {.day = atoi(day), .month = atoi(month), .year = atoi(year)};
    return new_date;
}

// Returns true if date is null (aka -)
bool check_if_null_date(Date date) {
    return date.day == 0 ? true : false;
}

// Comparing 2 dates. Return 0 if 1st is prior(or euqal) to 2nd, or 1 otherwise
int compare_dates(Date date1, Date date2) {
    if (date1.year < date2.year)
        return 0;
    else if (date1.year > date2.year)
        return 1;
    else {
        if (date1.month < date2.month)
            return 0;
        else if (date1.month > date2.month)
            return 1;
        else {
            if (date1.day <= date2.day)
                return 0;
            else
                return 1;
        }
    }
}
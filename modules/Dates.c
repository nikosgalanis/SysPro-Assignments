#include <stdlib.h>
#include <string.h>

#include "Dates.h"

// Convert a DD-MM-YYYY string to an actual date
date string_to_date(char* d) {
    // We know that our string type is __identical__ to DD-MM-YYYY.
    // so, we are going to take advantage of it
    char day[2], month[2], year[4];
    // Disgusting, but we are using C after all!
    day[0] = d[0]; day[1] = d[1];
    month[0] = d[3]; month[1] = d[4];
    year[0] = d[6]; year[1] = d[7]; year[2] = d[8]; year[3] = d[9];
    // Update the struct fields and return it
    date new_date = {.day = atoi(day), .month = atoi(month), .year = atoi(year)};
    return new_date;
}

// Comparing 2 dates. Return 0 if 1st is prior(or euqal) to 2nd, or 1 otherwise
int compare_dates(date date1, date date2) {
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
#pragma once

#include "common_types.h"
#include "BalancedTree.h"

typedef struct date {
    int day;
    int month;
    int year;
} Date;

#define EOF_DATE (Date)0

Date string_to_date(char* d);
int compare_dates(Date date1, Date date2);

int compare(TreeEntry ent1, TreeEntry ent2) {
    return compare_dates(ent1->date, ent2->date);
}
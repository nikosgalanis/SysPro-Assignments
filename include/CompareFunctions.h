#pragma once
#include "common_types.h"
#include "BalancedTree.h"
#include "Dates.h"

int compare(Pointer first, Pointer second) {
	TreeEntry entry1 = (TreeEntry)first;
	TreeEntry entry2 = (TreeEntry)second;
	return compare_dates(entry1->date, entry2->date);
}
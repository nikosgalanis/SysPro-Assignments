#pragma once
#include <stdbool.h> 
#include <stdlib.h>
#include <assert.h>


#define STRING_SIZE 100
// Usefull typedefs
typedef void* Pointer;

typedef unsigned int uint;

typedef int (*CompareFunc)(Pointer a, Pointer b);
typedef void (*DestroyFunc)(Pointer value);
typedef uint (*HashFunc)(Pointer);
typedef void (*VisitFunc)(Pointer, Pointer, Pointer, Pointer, Pointer);
typedef bool (*ConditionFunc)(Pointer, Pointer);


void parse_input (char* file, int bucket_size);
void monitor_menu();
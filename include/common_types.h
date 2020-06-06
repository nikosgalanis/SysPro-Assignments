#pragma once
#include <stdbool.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>


#define STRING_SIZE 1024
#define HASH_SIZE 100
#define BUCKET_SIZE 100
#define FAILED -1
#define PERMS 0777
// Usefull typedefs
typedef void* Pointer;

typedef unsigned int uint;

typedef int (*CompareFunc)(Pointer a, Pointer b);
typedef void (*DestroyFunc)(Pointer value);
typedef uint (*HashFunc)(Pointer);
typedef void (*VisitFunc)(Pointer, Pointer, Pointer, Pointer, Pointer);
typedef bool (*ConditionFunc)(Pointer, Pointer, Pointer, Pointer);


void parse_input (char* file, int bucket_size);
void monitor_menu();
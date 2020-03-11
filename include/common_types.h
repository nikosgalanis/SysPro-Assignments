#pragma once
#include <stdbool.h> 
#include <stdlib.h>

// Usefull typedefs
typedef void* Pointer;

typedef unsigned int uint;

typedef int (*CompareFunc)(Pointer a, Pointer b);
typedef void (*DestroyFunc)(Pointer value);
typedef uint (*HashFunc)(Pointer);
typedef void (*PrintFunc)(Pointer);
void parse_input (char* file, int num_countries, int num_diseases, int bucket_size);
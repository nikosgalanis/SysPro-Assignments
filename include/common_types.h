#pragma once
#include <stdbool.h> 
#include <stdlib.h>

// Usefull typedefs
typedef void* Pointer;

typedef unsigned int uint;

typedef int (*CompareFunc)(Pointer a, Pointer b);
typedef void (*DestroyFunc)(Pointer value);
typedef uint (*HashFunc)(Pointer);


uint hash_strings(char* str) {
    int h = 0, a = 33;
	for (; *str != '\0'; str++)
		h = (a * h + *str);
	return h;
}
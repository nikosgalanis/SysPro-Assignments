#pragma once
#include <stdbool.h> 

typedef void* Pointer;

typedef unsigned int uint;

typedef int (*CompareFunc)(Pointer a, Pointer b);
typedef void (*DestroyFunc)(Pointer value);
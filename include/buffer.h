#pragma once

#include "common_functions.h"

struct buffer{
    int* data;
    int start;
    int end;
    int size;
    int count;
};

typedef struct buffer* Buffer;

Buffer initialize_buffer(int size);
void place(Buffer buffer, int data);
int obtain (Buffer buffer);
void destroy_buffer(Buffer buff);

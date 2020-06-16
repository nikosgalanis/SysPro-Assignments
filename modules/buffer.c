#include "buffer.h"
#include <pthread.h>

extern pthread_mutex_t mtx;
extern pthread_cond_t cond_nonempty;
extern pthread_cond_t cond_nonfull;

// initialize a new buffer
Buffer initialize_buffer(int size) {
	Buffer new_buff = malloc(sizeof(*new_buff));
	new_buff->data = malloc(size * sizeof(int));
	new_buff->start = 0;
	new_buff->end = -1;
	new_buff->count = 0;
	new_buff->size = size;
}

// place an item in a buffer
void place(Buffer buffer, int data) {
	// shared data area -> lock the mutex
	pthread_mutex_lock(&mtx);
	// wait for a signal if the buffer is full
	while (buffer->count >= buffer->size) {
		pthread_cond_wait(&cond_nonfull, &mtx);
	}
	// update the end pointer in te buffer
	buffer->end = (buffer->end + 1) % buffer->size;
	// insert the data in the correct position
	buffer->data[buffer->end] = data;
	// a new item is in, increase our count
	buffer->count++;
	// shared memory area done, unlock the mutex
	pthread_mutex_unlock(&mtx);
}

int obtain (Buffer buffer) {
	// shared data area -> lock the mutex
	pthread_mutex_lock(&mtx);
	// wait for a signal if the buffer is empty
	while (buffer->count <= 0) {
		pthread_cond_wait(&cond_nonempty, &mtx);
	}
	// obtain the data from the 1st position of the buffer
	int data = buffer->data[buffer->start];
	// update the position that we're gonna read next time
	buffer->start = (buffer->start + 1) % buffer->size;
	// we've removed an item form the buffer
	buffer->count--;
	// shared memory area done, unlock the mutex
	pthread_mutex_unlock(&mtx);
	// return the data we've just read
	return data;
} 

// destroy an existing buffer
void destroy_buffer(Buffer buff) {
	assert(buff != NULL);
	free(buff->data);
	free(buff);
}
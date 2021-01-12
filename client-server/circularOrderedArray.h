#ifndef CIRCULAR_ORDERED_ARRAY_H
#define CIRCULAR_ORDERED_ARRAY_H

#include <stdint.h>
#include <stdlib.h>

struct CircularOrderedArray { 
	int next;
	int size;

	void (*cpy)(void*, void*); // copy(soruce, target)
	int (*cmp)(void*, void*);

	size_t dataSize;
	int maxSize;
	int* order;
	void** data;
};

typedef struct CircularOrderedArray CircularOrderedArray;

CircularOrderedArray * initCircularOrderedArray(int maxSize, size_t dataSize, void (*cpy)(void*, void*), int (*cmp)(void*, void*));
void resetCircularOrderedArray(CircularOrderedArray *);
void freeCircularOrderedArray(CircularOrderedArray *);

void insertOrdered(CircularOrderedArray *, void *);

#endif
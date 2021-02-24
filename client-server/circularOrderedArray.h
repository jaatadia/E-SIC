#ifndef CIRCULAR_ORDERED_ARRAY_H
#define CIRCULAR_ORDERED_ARRAY_H

#include <stdint.h>
#include <stdlib.h>

struct CircularOrderedArray { 
	int next;
	int size;

	void (*cpy)(void*, void*); // copy(soruce, target)
	int64_t (*cmp)(void*, void*);

	size_t dataSize;
	int maxSize;
	int* order;
	void** data;
};

typedef struct CircularOrderedArray CircularOrderedArray;

CircularOrderedArray * initCircularOrderedArray(int maxSize, size_t dataSize, void (*cpy)(void*, void*), int64_t (*cmp)(void*, void*));
void resetCircularOrderedArray(CircularOrderedArray *);
void freeCircularOrderedArray(CircularOrderedArray *);

void insertOrdered(CircularOrderedArray *, void *);

void foreach(CircularOrderedArray *, void (*f)(void*));

#endif
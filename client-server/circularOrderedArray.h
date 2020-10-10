#ifndef CIRCULAR_ORDERED_ARRAY_H
#define CIRCULAR_ORDERED_ARRAY_H

#ifndef CIRCULAR_ORDERED_ARRAY_MAX_SIZE
#define CIRCULAR_ORDERED_ARRAY_MAX_SIZE 600
#endif

#include <stdint.h>

struct Node {
	int64_t value;
	int order;
};

typedef struct Node Node;

struct CircularOrderedArray { 
	int next;
	int size;
	Node array[CIRCULAR_ORDERED_ARRAY_MAX_SIZE];
};

typedef struct CircularOrderedArray CircularOrderedArray;

void initCircularOrderedArray(CircularOrderedArray *);
void insertOrdered(CircularOrderedArray *, int64_t );
int64_t median(CircularOrderedArray *);
#endif
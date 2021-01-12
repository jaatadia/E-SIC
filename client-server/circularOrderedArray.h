#ifndef CIRCULAR_ORDERED_ARRAY_H
#define CIRCULAR_ORDERED_ARRAY_H

#ifndef CIRCULAR_ORDERED_ARRAY_MAX_SIZE
#define CIRCULAR_ORDERED_ARRAY_MAX_SIZE 600
#endif

#include <stdint.h>
#include "circularOrderedArrayNode.h"

struct CircularOrderedArray { 
	int next;
	int size;
	Node array[CIRCULAR_ORDERED_ARRAY_MAX_SIZE];
};

typedef struct CircularOrderedArray CircularOrderedArray;

void initCircularOrderedArray(CircularOrderedArray *);

void insertOrdered(CircularOrderedArray *, Node *);

int64_t median(CircularOrderedArray *);
Node* medianNode(CircularOrderedArray *);

#endif
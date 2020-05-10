#ifndef CIRCULAR_ORDERED_ARRAY_H
#define CIRCULAR_ORDERED_ARRAY_H

#define CIRCULAR_ORDERED_ARRAY_MAX_SIZE 600

struct CircularOrderedArray { 
	int next;
	int size;
	double array[CIRCULAR_ORDERED_ARRAY_MAX_SIZE];
	double fifo[CIRCULAR_ORDERED_ARRAY_MAX_SIZE];
};

typedef struct CircularOrderedArray CircularOrderedArray;

void initCircularOrderedArray(CircularOrderedArray *);
void insertOrdered(CircularOrderedArray *, double );
double median(CircularOrderedArray *);

#endif
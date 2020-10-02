/*
 * circularOrderedArray.h
 *
 *  Created on: May 24, 2020
 *      Author: jaatadia@gmail.com
 */
#ifndef CIRCULAR_ORDERED_ARRAY_H
#define CIRCULAR_ORDERED_ARRAY_H

#ifndef CIRCULAR_ORDERED_ARRAY_MAX_SIZE
#define CIRCULAR_ORDERED_ARRAY_MAX_SIZE 600
#endif

#include <stdint.h>

struct CircularOrderedArray { 
	int next;
	int size;
	int64_t fifo[CIRCULAR_ORDERED_ARRAY_MAX_SIZE];
	int64_t array[CIRCULAR_ORDERED_ARRAY_MAX_SIZE];
};

typedef struct CircularOrderedArray CircularOrderedArray;

void initCircularOrderedArray(CircularOrderedArray *);
void insertOrdered(CircularOrderedArray *, int64_t );
double median(CircularOrderedArray *);
#endif

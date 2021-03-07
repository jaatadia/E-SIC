#ifndef LINEAR_FIT_H
#define LINEAR_FIT_H

#include <stdint.h>

// y = m*x + c
struct LinearFitResult {
	double m; 
	double c; 
};

typedef struct LinearFitResult LinearFitResult;


// end: last position of the array to take into account (not included)
void linearFit(void* array, int start, int end, double(*fx)(void*, int), double(*fy)(void*, int), LinearFitResult* result);

#endif
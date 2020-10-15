#ifndef LINEAR_FIT_H
#define LINEAR_FIT_H

#ifndef CICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE
#define CICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE 600
#endif

#include <stdint.h>

struct Point {
	int64_t x;
	int64_t y;
};

typedef struct Point Point;

struct LinearFitResult {
	double m;
	double c;
};

typedef struct LinearFitResult LinearFitResult;

struct CircularLinearFitArray { 
	int size;
	int nextPos;
	Point array[CICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE];

	double m;
	double c;
};

typedef struct CircularLinearFitArray CircularLinearFitArray;

void initCircularLinearFitArray(CircularLinearFitArray* response);

void insertPoint(CircularLinearFitArray* response, int64_t median, int64_t t1);

void linearFit(CircularLinearFitArray* response);
void linearFitFunction(void* array, int start, int end, double(*fx)(void*, int), double(*fy)(void*, int), LinearFitResult* result);

#endif
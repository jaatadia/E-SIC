/*
 * linearfit.h
 *
 *  Created on: May 24, 2020
 *      Author: jaatadia@gmail.com
 */
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

struct CircularLinearFitArray { 
	int size;
	int nextPos;
	Point array[CICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE];

	int64_t Sx;
	int64_t Sy;
	double Sxx;
	double Sxy;

	double m;
	int64_t c;
};

typedef struct CircularLinearFitArray CircularLinearFitArray;

void initCircularLinearFitArray(CircularLinearFitArray* response);

void insertPoint(CircularLinearFitArray* response, int64_t median, int64_t t4);

void linearFit(CircularLinearFitArray* response);

#endif

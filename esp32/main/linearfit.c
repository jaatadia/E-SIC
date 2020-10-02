/*
 * linearfit.c
 *
 *  Created on: May 24, 2020
 *      Author: jaatadia@gmail.com
 */
#include "linearfit.h"

#ifdef DEBUG
#include <stdio.h>
#endif

void initCircularLinearFitArray(CircularLinearFitArray* array){
	array->nextPos = 0;
	array->size = 0;
	array->Sx = 0;
	array->Sy = 0;
	array->Sxx = 0;
	array->Sxy = 0;
}

void insertPoint(CircularLinearFitArray* array, int64_t newX, int64_t newY) {
	#ifdef DEBUG
	printf("linearfit.c: new point x: %f y: %f\n", newX, newY);	
	#endif

	int nextPos = array->nextPos;
	if(array->size == CICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE) {
		int64_t x = array->array[nextPos].x;
		int64_t y = array->array[nextPos].y;
		double dx = x; // cast to double
		double dy = y; // cast to double
		array->Sx -= x;
		array->Sy -= y;
		array->Sxx -= dx * dx;
		array->Sxy -= dx * dy;
	} else {
		array->size++;
	}
	int64_t x = newX;
	int64_t y = newY;
	double dx = newX; // cast to double
	double dy = newY; // cast to double
	array->Sx += x;
	array->Sy += y;
	array->Sxx += dx * dx;
	array->Sxy += dx * dy;
	array->array[nextPos].x = x;
	array->array[nextPos].y = y;

	array->nextPos = (nextPos + 1) % CICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE;
}

#include <stdio.h>
void linearFit(CircularLinearFitArray* array) {	
	int n = array->size;

	double dSx = array->Sx; // cast to double
	double dSy = array->Sy; // cast to double
	double delta = n*array->Sxx - dSx*dSx;

	array->m = (n*array->Sxy - dSx*dSy)/delta;
	array->c = (array->Sxx*dSy - dSx*array->Sxy)/delta;

	#ifdef DEBUG
	printf("linearfit.c: new linear fit m: %f c: %f\n", array->m, array->c);
	#endif
}



/*
double sx = 0.0, sy = 0.0, sxx = 0.0, sxy = 0.0;
	int n = x.size();
	for (int i = 0; i < n; ++i)
	{
		sx += x[i];
		sy += y[i];
		sxx += x[i]*x[i];
		sxy += x[i]*y[i];
	}
	double delta = n*sxx - sx*sx;
	double slope = (n*sxy - sx*sy)/delta;
	double intercept = (sxx*sy - sx*sxy)/delta;
*/

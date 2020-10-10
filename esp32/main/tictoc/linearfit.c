#include "linearfit.h"
#include <stdio.h>
#include <inttypes.h>

//#define TICTOC_LINEAR_FIT_DEBUG

void initCircularLinearFitArray(CircularLinearFitArray* array){
	array->nextPos = 0;
	array->size = 0;
}

void insertPoint(CircularLinearFitArray* array, int64_t newX, int64_t newY) {
	#ifdef TICTOC_LINEAR_FIT_DEBUG
	printf("Linearfit - New point x: %"PRId64" y: %"PRId64"\n.", newX, newY);	
	#endif

	array->array[array->nextPos].x = newX;
	array->array[array->nextPos].y = newY;

	if(array->size < CICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE) {
		array->size++;
	}

	array->nextPos = (array->nextPos + 1) % CICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE;
}

#include <stdio.h>
void linearFit(CircularLinearFitArray* array) {	
	int n = array->size;

	double Sx = 0;
	double Sy = 0;
	double Sxx = 0;
	double Sxy = 0;

	for (int i = 0; i < n; ++i)
	{
		double x = array->array[i].x;
		double y = array->array[i].y;
		Sx += x;
		Sy += y;
		Sxx += x*x;
		Sxy += x*y;
	}
	
	double delta = n*Sxx - Sx*Sx;
	array->m = (n*Sxy - Sx*Sy)/delta;
	array->c = (Sxx*Sy - Sx*Sxy)/delta;

	#ifdef TICTOC_LINEAR_FIT_DEBUG
	printf("Linearfit - New linear fit m: %f c: %f\n.", array->m, array->c);
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
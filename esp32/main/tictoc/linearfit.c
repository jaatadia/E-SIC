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
	printf("Linearfit - New point x: %"PRId64" y: %"PRId64".\n", newX, newY);	
	#endif

	array->array[array->nextPos].x = newX;
	array->array[array->nextPos].y = newY;

	if(array->size < CICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE) {
		array->size++;
	}

	array->nextPos = (array->nextPos + 1) % CICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE;
}

#include <stdio.h>
void linearFitFunction(void* array, int start, int end, double(*fx)(void*, int), double(*fy)(void*, int), LinearFitResult* result) {
	int n = end - start;
	double Sx = 0;
	double Sy = 0;
	double stt = 0;
	double sts = 0;

	for (int i = start; i < end; ++i)
	{
		Sx += (*fx)(array, i);
		Sy += (*fy)(array, i);
	}
	for (int i = start; i < end; ++i)
	{
		double t = (*fx)(array, i) - Sx/n;
		stt += t*t;
		sts += t*(*fy)(array, i);
	}
	double slope = sts/stt;
	double intercept = (Sy - Sx*slope)/n;


	result->m = slope;
	result->c = intercept;

	#ifdef TICTOC_LINEAR_FIT_DEBUG
	printf("Linearfit - New linear fit m: %f c: %f.\n", result->m, result->c);
	#endif	
}	


double getX(void* array, int pos) {
	return ((Point*) array)[pos].x;
}

double getY(void* array, int pos) {
	return ((Point*) array)[pos].y;
}

void linearFit(CircularLinearFitArray* array) {	
	LinearFitResult result;
	linearFitFunction(array->array, 0, array->size, getX, getY, &result);
	array->m = result.m;
	array->c = result.c;
}

/*
	https://www.johndcook.com/blog/2008/10/20/comparing-two-ways-to-fit-a-line-to-data/
	
	double sx = 0.0, sy = 0.0, stt = 0.0, sts = 0.0;
	int n = x.size();
	for (int i = 0; i < n; ++i)
	{
		sx += x[i];
		sy += y[i];
	}
	for (int i = 0; i < n; ++i)
	{
		double t = x[i] - sx/n;
		stt += t*t;
		sts += t*y[i];
	}
	double slope = sts/stt;
	double intercept = (sy - sx*slope)/n;
*/
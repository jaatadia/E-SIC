#include "linearfit.h"

// TODO find out if we really want a linear fit with only one variable 
// TODO refactor this, it is full of "should be" constants
void linearFit(LinearFitResponse* response, double* array, int startPosition, int size) {
	double Sx = 0; 
	double Sy = 0;
	double Sxx = 0;
	double Sxy = 0;

	int position = startPosition;
	for(int i = 0; i < size; i ++){
		Sx += i;
		Sx += i*i;
		Sy += array[position];
		Sxy += i*array[position];
		position = (position + 1 == size) ? 0 : position + 1;
	}

	response->m=(size*Sxy - Sx * Sy) / (size * Sxx - Sx * Sx);
	response->c= (1/size) * Sy - response->m * Sx / size;
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
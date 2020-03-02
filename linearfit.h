#ifndef LINEAR_FIT_H
#define LINEAR_FIT_H

struct LinearFitResponse { 
	double m;
	double c;
};

typedef struct LinearFitResponse LinearFitResponse;

void linearFit(LinearFitResponse* response, double* array, int startPosition, int size);

#endif
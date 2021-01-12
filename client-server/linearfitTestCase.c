#include "linearfit.h"
#include "cunit.h"
#include <stdio.h>

struct TestArray { 
	int64_t * x;
	int64_t * y;
};

typedef struct TestArray TestArray;

double getX(void* array, int position){
	return ((TestArray*)array)->x[position];
}

double getY(void* array, int position){
	return ((TestArray*)array)->y[position];
}


void correctLinearFit(){

	TestArray testArray;
	LinearFitResult result;

	
	int64_t x[] = {1, 2, 3};
	int64_t y[] = {1, 2, 3};
	testArray.x=x;
	testArray.y=y;

	linearFit(&testArray, 0, 3, getX, getY, &result);

	assert("slope 1", result.m, 1);
	assert("intercept 1", result.c, 0);

	int64_t x2[] = {1, 2, 3, 4};
	int64_t y2[] = {3, 5, 7, 9};
	testArray.x=x2;
	testArray.y=y2;

	linearFit(&testArray, 0, 4, getX, getY, &result);

	assert("slope 2", result.m, 2);
	assert("intercept 2", result.c, 1);
}

void linearfitHorizontal(){
	
	TestArray testArray;
	LinearFitResult result;

	int64_t x[] = {1, 2, 3};
	int64_t y[] = {1, 1, 1};
	testArray.x=x;
	testArray.y=y;

	linearFit(&testArray, 0, 3, getX, getY, &result);

	assert("slope horizontal", result.m, 0);
	assert("intercept horizontal", result.c, 1);

	/*CircularLinearFitArray circularLinearFitArray;
	initCircularLinearFitArray(&circularLinearFitArray);

	insertPoint(&circularLinearFitArray, 1, 1);
	insertPoint(&circularLinearFitArray, 2, 1);
	insertPoint(&circularLinearFitArray, 3, 1);
	insertPoint(&circularLinearFitArray, 4, 1);

	linearFit(&circularLinearFitArray);
	assert("Horizontal linearfit slope", circularLinearFitArray.m, 0);
	assert("Horizontal linearfit intercept", circularLinearFitArray.c, 1);*/
}



int main(int argc, char** argv){
	if (CICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE != 4){
		printf("Compile this test with -DCICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE=4");	
		return 1;
	}
	
	correctLinearFit();
	linearfitHorizontal();
	
	return reportResults();
}
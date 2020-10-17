#include "linearfit.h"
#include "cunit.h"
#include <stdio.h>

void correctLinearFit(){

	CircularLinearFitArray circularLinearFitArray;
	initCircularLinearFitArray(&circularLinearFitArray);

	insertPoint(&circularLinearFitArray, 1, 1);
	insertPoint(&circularLinearFitArray, 2, 2);
	insertPoint(&circularLinearFitArray, 3, 3);
	linearFit(&circularLinearFitArray);

	assert("Partially filled nextPos", circularLinearFitArray.nextPos, 3);
	// assert("Partially filled Sx", circularLinearFitArray.Sx, 1 + 2 + 3);
	// assert("Partially filled Sxx", circularLinearFitArray.Sxx, 1*1 + 2*2 + 3*3);
	// assert("Partially filled Sy", circularLinearFitArray.Sy, 1 + 2 + 3);
	// assert("Partially filled Sxy", circularLinearFitArray.Sxy, 1*1 + 2*2 + 3*3);
	assert("Partially filled linearfit slope", circularLinearFitArray.m, 1);
	assert("Partially filled linearfit intercept", circularLinearFitArray.c, 0);

	insertPoint(&circularLinearFitArray, 1, 3);
	insertPoint(&circularLinearFitArray, 2, 5);
	insertPoint(&circularLinearFitArray, 3, 7);
	insertPoint(&circularLinearFitArray, 4, 9);
	linearFit(&circularLinearFitArray);
	assert("Fully filled nextPos", circularLinearFitArray.nextPos, 3);
	// assert("Fully filled Sx", circularLinearFitArray.Sx, 1 + 2 + 3 + 4);
	// assert("Fully filled Sxx", circularLinearFitArray.Sxx, 1*1 + 2*2 + 3*3 + 4*4);
	// assert("Fully filled Sy", circularLinearFitArray.Sy, 3 + 5 + 7 + 9);
	// assert("Fully filled Sxy", circularLinearFitArray.Sxy, 1*3 + 2*5 + 3*7 + 4*9);
	assert("Fully filled linearfit slope", circularLinearFitArray.m, 2);
	assert("Fully filled linearfit intercept", circularLinearFitArray.c, 1);

}

void linearfitHorizontal(){
	CircularLinearFitArray circularLinearFitArray;
	initCircularLinearFitArray(&circularLinearFitArray);

	insertPoint(&circularLinearFitArray, 1, 1);
	insertPoint(&circularLinearFitArray, 2, 1);
	insertPoint(&circularLinearFitArray, 3, 1);
	insertPoint(&circularLinearFitArray, 4, 1);

	linearFit(&circularLinearFitArray);
	assert("Horizontal linearfit slope", circularLinearFitArray.m, 0);
	assert("Horizontal linearfit intercept", circularLinearFitArray.c, 1);
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
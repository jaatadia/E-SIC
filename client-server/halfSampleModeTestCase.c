#include "halfSampleMode.h"
#include <stdio.h>
#include "cunit.h"

int64_t getValue(void * array, int position){
	return ((int64_t *) array)[position];
}

void mode1Sample(){
	int size = 1;
	HalfSampleModeResult result;
	
	int64_t array[] = {3};
	halfSampleMode(array, 0, size, getValue, &result);
	assert("Mode 1 sample", result.mode, 3);
	assert("Mode 1 sample position", result.position1, 0);
	assert("Mode 1 sample position", result.position2, -1);
}

void mode2Samples(){
	int size = 2;
	HalfSampleModeResult result;

	int64_t array[] = {3, 3};
	halfSampleMode(array, 0, size, getValue, &result);
	assert("Mode 2 equal samples", result.mode, 3);
	assert("Mode 2 sample position", result.position1, 0);
	assert("Mode 2 sample position", result.position2, 1);	

	int64_t array2[] = {3, 4};
	halfSampleMode(array2, 0, size, getValue, &result);
	assert("Mode 2 equal samples", result.mode, 3);
	assert("Mode 2 sample position", result.position1, 0);
	assert("Mode 2 sample position", result.position2, 1);	

	int64_t array3[] = {3, 5};
	halfSampleMode(array3, 0, size, getValue, &result);
	assert("Mode 2 equal samples", result.mode, 4);
	assert("Mode 2 sample position", result.position1, 0);
	assert("Mode 2 sample position", result.position2, 1);	
}

void mode3Samples(){
	int size = 3;
	HalfSampleModeResult result;
	
	int64_t array[] = {3, 3, 3};
	halfSampleMode(array, 0, size, getValue, &result);
	assert("Mode 3 equal samples", result.mode, 3);
	assert("Mode 3 sample position", result.position1, 1);
	assert("Mode 3 sample position", result.position2, -1);	

	int64_t array2[] = {2, 3, 4};
	halfSampleMode(array2, 0, size, getValue, &result);
	assert("Mode 3 equal samples", result.mode, 3);
	assert("Mode 3 sample position", result.position1, 1);
	assert("Mode 3 sample position", result.position2, -1);	

	int64_t array3[] = {0, 3, 5};
	halfSampleMode(array3, 0, size, getValue, &result);
	assert("Mode 3 equal samples", result.mode, 4);
	assert("Mode 3 sample position", result.position1, 1);
	assert("Mode 3 sample position", result.position2, 2);	

	int64_t array4[] = {1, 3, 6};
	halfSampleMode(array4, 0, size, getValue, &result);
	assert("Mode 3 equal samples", result.mode, 2);
	assert("Mode 3 sample position", result.position1, 0);
	assert("Mode 3 sample position", result.position2, 1);	
}

void mode4Samples(){
	int size = 4;
	HalfSampleModeResult result;

	int64_t array[] = {3, 3, 3, 3};
	halfSampleMode(array, 0, size, getValue, &result);
	assert("Mode 4 A equal samples", result.mode, 3);
	assert("Mode 4 A sample position", result.position1, 2);
	assert("Mode 4 A sample position", result.position2, 3);	

	int64_t array2[] = {1, 2, 3, 4};
	halfSampleMode(array2, 0, size, getValue, &result);
	assert("Mode 4 B equal samples", result.mode, 3);
	assert("Mode 4 B sample position", result.position1, 2);
	assert("Mode 4 B sample position", result.position2, 3);	


	int64_t array3[] = {1, 2, 4, 4};
	halfSampleMode(array3, 0, size, getValue, &result);
	assert("Mode 4 C equal samples", result.mode, 4);
	assert("Mode 4 C sample position", result.position1, 2);
	assert("Mode 4 C sample position", result.position2, 3);	

	int64_t array4[] = {1, 1, 2, 3};
	halfSampleMode(array4, 0, size, getValue, &result);
	assert("Mode 4 D equal samples", result.mode, 1);
	assert("Mode 4 D sample position", result.position1, 0);
	assert("Mode 4 D sample position", result.position2, 1);	
}

void windowedMedianPosition(){
	int windowsSize = 2;
	HalfSampleModeResult result;

	int size = 9;
	int64_t array[] = {1, 2, 3, 4, 8, 10, 12, 13, 22};
	halfSampleModeWindow(array, 0, size, getValue, windowsSize, &result);
	assert("Mode window samples", result.position1, 2);
	assert("Mode window samples", result.position2, 4);
}

int main(int argc, char** argv){
	mode1Sample();
	mode2Samples();
	mode3Samples();
	mode4Samples();

	windowedMedianPosition();
	
	return reportResults();
}
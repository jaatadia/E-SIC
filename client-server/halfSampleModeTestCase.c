#include "halfSampleMode.h"
#include <stdio.h>
#include "cunit.h"

int64_t getValue(void * array, int position){
	return ((int64_t *) array)[position];
}

void mode1Sample(){
	int size = 1;
	int64_t array[] = {3};
	assert("Mode 1 sample", halfSampleMode(array, 0, size, getValue), 3);
	assert("Mode 1 sample position", halfSampleModePosition(array, 0, size, getValue), 0);
}

void mode2Samples(){
	int size = 2;
	int64_t array[] = {3, 3};
	assert("Mode 2 equal samples", halfSampleMode(array, 0, size, getValue), 3);
	assert("Mode 2 equal samples position", halfSampleModePosition(array, 0, size, getValue), 0);

	int64_t array2[] = {3, 4};
	assert("Mode 2 different consecutive samples", halfSampleMode(array2, 0, size, getValue), 3);
	assert("Mode 2 different consecutive samples position", halfSampleModePosition(array2, 0, size, getValue), 0);

	int64_t array3[] = {3, 5};
	assert("Mode 2 different non-consecutive samples", halfSampleMode(array3, 0, size, getValue), 4);
	assert("Mode 2 different non-consecutive samples position", halfSampleModePosition(array3, 0, size, getValue), 0);
}

void mode3Samples(){
	int size = 3;
	int64_t array[] = {3, 3, 3};
	assert("Mode 3 equal samples", halfSampleMode(array, 0, size, getValue), 3);
	assert("Mode 3 equal samples position", halfSampleModePosition(array, 0, size, getValue), 1);

	int64_t array2[] = {2, 3, 4};
	assert("Mode 3 different consecutive samples", halfSampleMode(array2, 0, size, getValue), 3);
	assert("Mode 3 different consecutive samples", halfSampleModePosition(array2, 0, size, getValue), 1);

	int64_t array3[] = {0, 3, 5};
	assert("Mode 3 samples mode to the right", halfSampleMode(array3, 0, size, getValue), 4);
	assert("Mode 3 samples mode to the right position", halfSampleModePosition(array3, 0, size, getValue), 1);

	int64_t array4[] = {1, 3, 6};
	assert("Mode 2 samples mode to the left", halfSampleMode(array4, 0, size, getValue), 2);
	assert("Mode 2 samples mode to the left position", halfSampleModePosition(array4, 0, size, getValue), 0);
}

void mode4Samples(){
	int size = 4;
	int64_t array[] = {3, 3, 3, 3};
	assert("Mode 4 equal samples", halfSampleMode(array, 0, size, getValue), 3);
	assert("Mode 4 equal samples position", halfSampleModePosition(array, 0, size, getValue), 1);

	int64_t array2[] = {1, 2, 3, 4};
	assert("Mode 4 different consecutive samples", halfSampleMode(array2, 0, size, getValue), 3);
	assert("Mode 4 different consecutive samples position", halfSampleModePosition(array2, 0, size, getValue), 2);

	int64_t array3[] = {1, 2, 4, 4};
	assert("Mode 4 samples mode to the right", halfSampleMode(array3, 0, size, getValue), 4);
	assert("Mode 4 samples mode to the right position", halfSampleModePosition(array3, 0, size, getValue), 2);

	int64_t array4[] = {1, 1, 2, 3};
	assert("Mode 4 samples mode to the left", halfSampleMode(array4, 0, size, getValue), 1);
	assert("Mode 4 samples mode to the left position", halfSampleModePosition(array4, 0, size, getValue), 0);
}

void windowedMedianPosition(){
	int windowsSize = 2;
	int windows[] = {6, 3};
	
	int size = 9;
	int64_t array[] = {1, 2, 3, 4, 8, 10, 12, 13, 22};
	assert("Mode 4 equal samples", halfSampleModeWindowedMedianPosition(windows, windowsSize, array, 0, size, getValue), 2);
}

int main(int argc, char** argv){
	mode1Sample();
	mode2Samples();
	mode3Samples();
	mode4Samples();

	windowedMedianPosition();
	
	return reportResults();
}
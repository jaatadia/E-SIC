#include "circularOrderedArray.h"
#include <stdio.h>

int successTests = 0;
int failedTests = 0;

void assert(char* testName, int actual, int expected) {
	int condition = expected == actual;
	
	if(condition){
		successTests++;
		printf("Test: %s. SUCCESS\n", testName);	
	} else {
		failedTests ++;	
		printf("Test: %s. FAIL. Expected: %d Actual: %d\n", testName, expected, actual);	
	}
	
}

void assertPositions(char* testName, double * arrayActual, double * arrayExpected, int size){

	int fail = 0;
	
	printf("Test: %s. ", testName);	
	for (int i = 0; i < size; i++){
		if(arrayActual[i] == arrayExpected[i]){
			printf(" %f✓", arrayActual[i]);	
		} else {
			fail = 1;
			printf(" %f!=%f", arrayActual[i], arrayExpected[i]);	
		}
	}

	if(!fail){
		successTests++;
		printf(" SUCCESS\n");	
	} else {
		failedTests ++;	
		printf(" FAIL\n");	
	}

}

void correctInsertStarting() {
	printf("---- correctInsertStarting ----\n");	

	CircularOrderedArray array;
	initCircularOrderedArray(&array);

	
	assert("0. size", array.size, 0);
	assert("0. nextPos", array.next, 0);

	insertOrdered(&array, 2);
	double fifo[] = {2};
	double ordered[] = {2};
	assert("1. size", array.size, 1);
	assert("1. nextPos", array.next, 0);	
	assertPositions("1. fifo", array.fifo, fifo, 1);
	assertPositions("1. ordered", array.array, ordered, 1);
	assert("1. median", median(&array), 2);

	insertOrdered(&array, 4);
	double fifo2[] = {2, 4};
	double ordered2[] = {2, 4};
	assert("2. size", array.size, 2);
	assert("2. nextPos", array.next, 0);	
	assertPositions("2. fifo", array.fifo, fifo2, 2);
	assertPositions("2. ordered", array.array, ordered2, 2);
	assert("2. median", median(&array), 4);

	insertOrdered(&array, 3);
	double fifo3[] = {2, 4, 3};
	double ordered3[] = {2, 3, 4};
	assert("3. size", array.size, 3);
	assert("3. nextPos", array.next, 0);	
	assertPositions("3. fifo", array.fifo, fifo3, 3);
	assertPositions("3. ordered", array.array, ordered3, 3);
	assert("3. median", median(&array), 3);

	insertOrdered(&array, 0);
	double fifo4[] = {2, 4, 3, 0};
	double ordered4[] = {0, 2, 3, 4};
	assert("4. size", array.size, 4);
	assert("4. nextPos", array.next, 0);	
	assertPositions("4. fifo", array.fifo, fifo4, 4);
	assertPositions("4. ordered", array.array, ordered4, 4);
	assert("4. median", median(&array), 3);
}


void correctInsertOnceFull() {
	printf("---- correctInsertOnceFull ----\n");	
	CircularOrderedArray array;
	initCircularOrderedArray(&array);
	for(int i = 0; i< CIRCULAR_ORDERED_ARRAY_MAX_SIZE; i++){
		insertOrdered(&array, i);
	}
	
	double fifo_0[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	double ordered_0[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	assert("_0. size", array.size, 10);
	assert("_0. nextPos", array.next, 0);
	assertPositions("_0. fifo", array.fifo, fifo_0, 10);
	assertPositions("_0. ordered", array.array, ordered_0, 10);
	assert("_0. median", median(&array), 5);

	insertOrdered(&array, 2);
	double fifo_1[] = {2, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	double ordered_1[] = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9};
	assert("_1. size", array.size, 10);
	assert("_1. nextPos", array.next, 1);
	assertPositions("_1. fifo", array.fifo, fifo_1, 10);
	assertPositions("_1. ordered", array.array, ordered_1, 10);
	assert("_1. median", median(&array), 5);

	insertOrdered(&array, 8);
	double fifo_2[] = {2, 8, 2, 3, 4, 5, 6, 7, 8, 9};
	double ordered_2[] = {2, 2, 3, 4, 5, 6, 7, 8, 8, 9};
	assert("_2. size", array.size, 10);
	assert("_2. nextPos", array.next, 2);
	assertPositions("_2. fifo", array.fifo, fifo_2, 10);
	assertPositions("_2. ordered", array.array, ordered_2, 10);
	assert("_2. median", median(&array), 6);

	insertOrdered(&array, -1);
	insertOrdered(&array, 1);
	insertOrdered(&array, 0);
	double fifo_3[] = {2, 8, -1, 1, 0, 5, 6, 7, 8, 9};
	double ordered_3[] = {-1, 0, 1, 2, 5, 6, 7, 8, 8, 9};
	assert("_3. size", array.size, 10);
	assert("_3. nextPos", array.next, 5);
	assertPositions("_3. fifo", array.fifo, fifo_3, 10);
	assertPositions("_3. ordered", array.array, ordered_3, 10);
	assert("_3. median", median(&array), 6);
}


int main(int argc, char** argv){
	if (CIRCULAR_ORDERED_ARRAY_MAX_SIZE != 10){
		printf("Compile this test with -DCIRCULAR_ORDERED_ARRAY_MAX_SIZE=10");	
		return 1;
	}
	
	correctInsertStarting();
	correctInsertOnceFull();
	printf("\n-------------------------------------------------------\n");
	printf("Run: %d, Succesful: %d, Fail: %d.\n", failedTests + successTests, successTests, failedTests);
	return 0;

}
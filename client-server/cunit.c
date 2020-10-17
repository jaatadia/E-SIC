#include <stdio.h>
#include <inttypes.h>

#include "cunit.h"

int successTests = 0;
int failedTests = 0;

void assert(char* testName, int actual, int expected) {
	int condition = expected == actual;
	
	if(condition){
		successTests++;
		printf("Test: %s. \033[0;32mSUCCESS\033[39;49m\n", testName);	
	} else {
		failedTests ++;	
		printf("Test: %s. \033[1;31mFAIL\033[39;49m. Expected: %d Actual: %d\n", testName, expected, actual);	
	}
	
}

void assertPositions(char* testName, void * arrayActual, int64_t(*fn)(void*, int), int64_t * arrayExpected, int size){

	int fail = 0;
	
	printf("Test: %s. ", testName);	
	for (int i = 0; i < size; i++){
		int64_t actual = (*fn)(arrayActual, i);
		if(actual == arrayExpected[i]){
			printf(" \033[0;32m%ld✓\033[39;49m", actual);	
		} else {
			fail = 1;
			printf(" \033[1;31m%ld!=%ld\033[39;49m", actual, arrayExpected[i]);	
		}
	}

	if(!fail){
		successTests++;
		printf(" \033[0;32mSUCCESS\033[39;49m\n");	
	} else {
		failedTests ++;	
		printf(" \033[1;31mFAIL\033[39;49m\n");	
	}

}

void assertInMargin(char* testName, int64_t actual, int64_t expected, int margin) {
	int64_t difference = (expected - actual);
	difference = (difference < 0) ? -difference : difference;
	
	if(difference < margin){
		successTests++;
		printf("Test: %s. \033[0;32mSUCCESS\033[39;49m Difference: %"PRId64"\n", testName, difference);	
	} else {
		failedTests ++;	
		printf("Test: %s. \033[1;31mFAIL\033[39;49m. Expected: %"PRId64" Actual: %"PRId64" Difference: %"PRId64"\n", testName, expected, actual, difference);	
	}	
}

int reportResults(){
	printf("\n-------------------------------------------------------\n");
	printf("Run: %d, Succesful: %s%d%s, Fail: %s%d%s.\n", failedTests + successTests, 
		(successTests > 0) ? "\033[0;32m" : "", successTests, (successTests > 0) ? "\033[39;49m" : "",
		(failedTests > 0) ? "\033[1;31m" : "", failedTests, (failedTests > 0) ? "\033[39;49m" : "");

	return (failedTests > 0) ? -1 : 0;
}	
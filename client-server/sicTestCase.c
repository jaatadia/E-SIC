#include "sic.h"
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


void syncStatesTestCase() {
	SicData sic;
	sicInit(&sic);
	
	sicStep(&sic, -1*1000, -1*1000 + 10, -1*1000 + 11, -1*1000 + 21);
	assert("noSync", sic.state, NO_SYNC);
	assert("noSync steps", sic.syncSteps, 1);

	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);

	assert("reSync when no sync achieved", sic.state, NO_SYNC);
	assert("reSync steps when no sync achieved", sic.syncSteps, 0);

	for(int i=0; i< 720; i++){
		sicStep(&sic, i*1000, i*1000 + 10, i*1000 + 11, i*1000 + 21);
		if(i == 598){
			assert("Iteration 598 noSync", sic.state, NO_SYNC);
			assert("Iteration 598 steps", sic.syncSteps, 599);
		} else if(i == 599) {
			assert("Iteration 599 preSync", sic.state, NO_SYNC);
			assert("Iteration 599 steps", sic.syncSteps, 600);
		} else if(i == 658) {
			assert("Iteration 658 preSync", sic.state, NO_SYNC);
			assert("Iteration 658 steps", sic.syncSteps, 659);
		} else if(i == 659) {
			assert("Iteration 659 preSync", sic.state, PRE_SYNC);
			assert("Iteration 659 steps", sic.syncSteps, 0);
		} else if(i == 718) {
			assert("Iteration 718 preSync", sic.state, PRE_SYNC);
			assert("Iteration 718 steps", sic.syncSteps, 59);
		} else if(i == 719) {
			assert("Iteration 719 preSync", sic.state, SYNC);
			assert("Iteration 719 steps", sic.syncSteps, 0);
		}
	}

	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);

	assert("reSync", sic.state, RE_SYNC);
	assert("reSync steps", sic.syncSteps, 0);
}

void syncNoDifferenceInClocks() {
	SicData sic;
	sicInit(&sic);
	
	for(int i=0; i< 720; i++){
		sicStep(&sic, i*1000000, i*1000000 + 10, i*1000000 + 11, i*1000000 + 21);
	}

	assert("Synced algorithm no difference in clocks actual_m", sic.actual_m, 0);
	assert("Synced algorithm no difference in clocks actual_c", sic.actual_c, 0);
	assert("Synced algorithm no difference in clocks", sicTime(&sic, 1000), 1000);
}

void syncServerInFuture() {
	SicData sic;
	sicInit(&sic);
	
	for(int i=0; i< 720; i++){
		sicStep(&sic, i*1000000, (i+1)*1000000 + 10, (i+1)*1000000 + 11, i*1000000 + 21);
	}

	assert("Synced algorithm server in future actual_m", sic.actual_m, 0);
	assert("Synced algorithm server in future actual_c", sic.actual_c, -1000000);
	assert("Synced algorithm server in future", sicTime(&sic, 1000), 1001000);
}

void syncServerInPast() {
	SicData sic;
	sicInit(&sic);
	
	for(int i=0; i< 720; i++){
		sicStep(&sic, i*1000000, (i-1)*1000000 + 10, (i-1)*1000000 + 11, i*1000000 + 21);
	}

	assert("Synced algorithm server in past actual_m", sic.actual_m, 0);
	assert("Synced algorithm server in past actual_c", sic.actual_c, +1000000);
	assert("Synced algorithm server in past", sicTime(&sic, 1000000), 0);
}

int main(int argc, char** argv){
	syncStatesTestCase();
	syncNoDifferenceInClocks();
	syncServerInFuture();
	syncServerInPast();


	//TODO extract testing logic to its own module
	printf("\n-------------------------------------------------------\n");
	printf("Run: %d, Succesful: %d, Fail: %d.\n", failedTests + successTests, successTests, failedTests);
	return 0;
}
#include "sic.h"
#include <stdlib.h>
#include <stdio.h>

int successTests = 0;
int failedTests = 0;
int true = 1;
int seed = 1111;

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

int randUpTo(int max) {
	return rand() % max;
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

void syncServerDifFrequency() {
	SicData sicA;
	sicInit(&sicA);
	
	int64_t serverTime = 1602262903000000;
	int64_t serverDelay = 50;
	int64_t startTimeA = 1000;
	int64_t tAS = 2222;
	
	double f_multiplier = 1.01;

	for(int i=0; i< 720; i++){
		int64_t timeSinceStart = i*1000000;
		sicStep(&sicA, 
			startTimeA + f_multiplier * timeSinceStart , 
			timeSinceStart + tAS + serverTime, 
			timeSinceStart + tAS + serverTime + serverDelay,
			startTimeA + f_multiplier * (timeSinceStart + tAS + tAS + serverDelay));
	}

	int64_t timeSinceStart = 800*1000000;
	int64_t tS = timeSinceStart + serverTime;
	int64_t tS_A = sicTime(&sicA, f_multiplier * timeSinceStart + startTimeA);
	
	printf("frequency:\n");
	printf("time ServerA: %ld.\n", tS);
	printf("Server Time Acording to NodeA: %ld. Diff to RealServer: %ld.\n", tS_A, tS - tS_A);
	
	assert("Parallel: A in error margin", true, ((tS - tS_A) < 100) && ((tS - tS_A) > -100));
}


void parallel() {
	SicData sicA;
	SicData sicB;
	sicInit(&sicA);
	sicInit(&sicB);
	
	int64_t serverTime = 1602262903000000;
	int64_t serverDelay = 50;
	int64_t startTimeA = 1000;
	int64_t startTimeB = 2222;
	int64_t tAS = 2222;
	int64_t tBS = 5333;
	
	for(int i=0; i< 720; i++){
		int64_t timeSinceStart = i*1000000;
		sicStep(&sicA, 
			timeSinceStart + startTimeA, 
			timeSinceStart + tAS + serverTime, 
			timeSinceStart + tAS + serverTime + serverDelay,
			timeSinceStart + startTimeA + tAS + tAS + serverDelay);

		sicStep(&sicB, 
			timeSinceStart + startTimeB, 
			timeSinceStart + tBS + serverTime, 
			timeSinceStart + tBS + serverTime + serverDelay,
			timeSinceStart + startTimeB + tBS + tBS + serverDelay);
	}

	int64_t timeSinceStart = 800*1000000;
	int64_t tS = timeSinceStart + serverTime;
	int64_t tS_A = sicTime(&sicA, timeSinceStart + startTimeA);
	int64_t tS_B = sicTime(&sicB, timeSinceStart + startTimeB);
	
	printf("Parallel:\n");
	printf("time ServerA: %ld.\n", tS);
	printf("Server Time Acording to NodeA: %ld. Diff to RealServer: %ld.\n", tS_A, tS - tS_A);
	printf("Server Time Acording to NodeB: %ld. Diff to RealServer: %ld.\n", tS_B, tS - tS_B);
	
	assert("Parallel: A in error margin", true, ((tS - tS_A) < 100) && ((tS - tS_A) > -100));
	assert("Parallel: B in error margin", true, ((tS - tS_B) < 100) && ((tS - tS_B) > -100));
}

void parallelSimulatedVariations() {
	SicData sicA;
	SicData sicB;
	sicInit(&sicA);
	sicInit(&sicB);
	
	int maxVariation = 1000;
	int64_t serverTime = 1602262903000000;
	int64_t serverDelay = 50;
	int64_t startTimeA = 1000;
	int64_t startTimeB = 2222;
	int64_t tAS = 2222;
	int64_t tBS = 5333;
	
	for(int i=0; i< 720; i++){

		int64_t timeSinceStart = i*1000000 + randUpTo(200000);

		int64_t timeSinceStartA = timeSinceStart + randUpTo(maxVariation);
		int64_t vtAS = tAS + randUpTo(maxVariation);
		int64_t vtSA = tAS + randUpTo(maxVariation);
		int64_t vserverDelayA = serverDelay + randUpTo(maxVariation);

		int64_t timeSinceStartB = timeSinceStart + randUpTo(maxVariation);
		int64_t vtBS = tBS + randUpTo(maxVariation);
		int64_t vtSB = tBS + randUpTo(maxVariation);
		int64_t vserverDelayB = serverDelay + randUpTo(maxVariation);

		sicStep(&sicA, 
			timeSinceStartA + startTimeA, 
			timeSinceStartA + vtAS + serverTime, 
			timeSinceStartA + vtAS + serverTime + vserverDelayA,
			timeSinceStartA + startTimeA + vtAS + vtSA + vserverDelayA);

		sicStep(&sicB, 
			timeSinceStartB + startTimeB, 
			timeSinceStartB + vtBS + serverTime, 
			timeSinceStartB + vtBS + serverTime + vserverDelayB,
			timeSinceStartB + startTimeB + vtBS + vtSB + vserverDelayB);
	}

	int64_t timeSinceStart = 800*1000000;
	int64_t tS = timeSinceStart + serverTime;
	int64_t tS_A = sicTime(&sicA, timeSinceStart + startTimeA);
	int64_t tS_B = sicTime(&sicB, timeSinceStart + startTimeB);
	
	printf("Parallel Variations:\n");
	printf("time ServerA: %ld.\n", tS);
	printf("Server Time Acording to NodeA: %ld. Diff to RealServer: %ld.\n", tS_A, tS - tS_A);
	printf("Server Time Acording to NodeB: %ld. Diff to RealServer: %ld.\n", tS_B, tS - tS_B);
	
	assert("Parallel Variations: A in error margin", true, ((tS - tS_A) < 100) && ((tS - tS_A) > -100));
	assert("Parallel Variations: B in error margin", true, ((tS - tS_B) < 100) && ((tS - tS_B) > -100));
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
	srand(seed);

	syncStatesTestCase();
	syncNoDifferenceInClocks();
	syncServerInFuture();
	syncServerInPast();
	parallel();
	parallelSimulatedVariations();
	syncServerDifFrequency();

	//TODO extract testing logic to its own module
	printf("\n-------------------------------------------------------\n");
	printf("Run: %d, Succesful: %d, Fail: %d.\n", failedTests + successTests, successTests, failedTests);
	return 0;
}
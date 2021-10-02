#include "esic.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "cunit.h"
#include "limits.h"

int seed = 1112;
int rtt = 0;

int randUpTo(int max) {
	return rand() % max;
}

int randSign() {
	return (rand() % 2 == 0) ? 1:-1;
}

void syncStatesTestCase() {
	ESicData sic;
	esicInit(&sic);
	
	esicStep(&sic, -1*1000, -1*1000 + 10, -1*1000 + 11, -1*1000 + 21);
	assert("noSync", sic.state, esic_NO_SYNC);
	assert("noSync steps", sic.syncSteps, 1);

	esicStepTimeout(&sic);
	esicStepTimeout(&sic);
	esicStepTimeout(&sic);
	esicStepTimeout(&sic);
	esicStepTimeout(&sic);
	esicStepTimeout(&sic);

	assert("reSync when no sync achieved", sic.state, esic_NO_SYNC);
	assert("reSync steps when no sync achieved", sic.syncSteps, 0);

	for(int i=0; i< esic_STARTUP_CYCLES + esic_P; i++){
		esicStep(&sic, i*1000, i*1000 + 10, i*1000 + 11, i*1000 + 21);
		if(i == esic_STARTUP_CYCLES - 2){
			assert("Iteration pre sync state", sic.state, esic_NO_SYNC);
			assert("Iteration pre sync steps", sic.syncSteps, esic_STARTUP_CYCLES - 1);
		} else if(i == esic_STARTUP_CYCLES - 1) {
			assert("Iteration sync state", sic.state, esic_SYNC);
			assert("Iteration sync steps", sic.syncSteps, 0);
		} else if(i == esic_STARTUP_CYCLES + esic_P - 2) {
			assert("Iteration sync pre recalculation state", sic.state, esic_SYNC);
			assert("Iteration sync pre recalculation steps", sic.syncSteps, esic_P - 1);
		} else if(i == esic_STARTUP_CYCLES + esic_P - 1) {
			assert("Iteration sync recalculation state", sic.state, esic_SYNC);
			assert("Iteration sync recalculation steps", sic.syncSteps, 0);
		}
	}

	for(int i=0; i< esic_MAX_to; i++){
		esicStepTimeout(&sic);	
	}
	
	assert("reSync", sic.state, esic_RE_SYNC);
	assert("reSync steps", sic.syncSteps, 0);

	esicEnd(&sic);
}

void syncServerDifFrequency() {
	ESicData sicA;
	esicInit(&sicA);
	
	int64_t serverTime = 1602262903000000;
	int64_t serverDelay = 50;
	int64_t startTimeA = 1000;
	int64_t tAS = 2222;
	
	double f_multiplier = 1 + 0.01;

	for(int i=0; i< 600 + 61 * 60; i++){
		int64_t timeSinceStart = i*1000000;
		esicStep(&sicA, 
			startTimeA + f_multiplier * timeSinceStart , 
			timeSinceStart + tAS + serverTime, 
			timeSinceStart + tAS + serverTime + serverDelay,
			startTimeA + f_multiplier * (timeSinceStart + tAS + tAS + serverDelay));
	}

	int64_t timeSinceStart = 800*1000000;
	int64_t tS = timeSinceStart + serverTime;
	int64_t tS_A = esicTime(&sicA, f_multiplier * timeSinceStart + startTimeA);
	
	assertInMargin("syncServerDifFrequency", tS_A, tS, 100);
	esicEnd(&sicA);
}


void parallel() {
	ESicData sicA;
	ESicData sicB;
	esicInit(&sicA);
	esicInit(&sicB);
	
	int64_t serverTime = 1602262903000000;
	int64_t serverDelay = 50;
	int64_t startTimeA = 1000;
	int64_t startTimeB = 2222;
	int64_t tAS = 2222;
	int64_t tBS = 5333;
	
	for(long long i=0; i< 60000; i++){
		int64_t timeSinceStart = i*1000000;
		esicStep(&sicA, 
			timeSinceStart + startTimeA, 
			timeSinceStart + tAS + serverTime, 
			timeSinceStart + tAS + serverTime + serverDelay,
			timeSinceStart + startTimeA + tAS + tAS + serverDelay);

		esicStep(&sicB, 
			timeSinceStart + startTimeB, 
			timeSinceStart + tBS + serverTime, 
			timeSinceStart + tBS + serverTime + serverDelay,
			timeSinceStart + startTimeB + tBS + tBS + serverDelay);
	}

	int64_t timeSinceStart = 800*1000000;
	int64_t tS = timeSinceStart + serverTime;
	int64_t tS_A = esicTime(&sicA, timeSinceStart + startTimeA);
	int64_t tS_B = esicTime(&sicB, timeSinceStart + startTimeB);
	
	printf("Parallel:\n");
	printf("time ServerA: %ld.\n", tS);
	printf("Server Time Acording to NodeA: %ld. Diff to RealServer: %ld.\n", tS_A, tS - tS_A);
	printf("Server Time Acording to NodeB: %ld. Diff to RealServer: %ld.\n", tS_B, tS - tS_B);
	
	assertInMargin("Parallel: server time A", tS_A, tS, 100);
	assertInMargin("Parallel: server time B", tS_B, tS, 100);

	esicEnd(&sicA);
	esicEnd(&sicB);
}

void parallelSimulatedVariations() {
	ESicData sicA;
	ESicData sicB;
	esicInit(&sicA);
	esicInit(&sicB);
	
	int maxVariation = 500;
	int64_t serverTime = 1602262903000000;
	int64_t serverDelay = 50;
	int64_t startTimeA = 1000;
	int64_t startTimeB = 2222;
	int64_t tAS = 2222;
	int64_t tBS = 5333;
	
	for(long long i=0; i< 60000; i++){

		int64_t timeSinceStart = i*1000000 + randSign() * randUpTo(200000);

		int64_t timeSinceStartA = timeSinceStart + randSign() * randUpTo(maxVariation);
		int64_t vtAS = tAS + randSign() * randUpTo(maxVariation);
		int64_t vtSA = tAS + randSign() * randUpTo(maxVariation);
		int64_t vserverDelayA = serverDelay;

		int64_t timeSinceStartB = timeSinceStart + randSign() * randUpTo(maxVariation);
		int64_t vtBS = tBS + randSign() * randUpTo(maxVariation);
		int64_t vtSB = tBS + randSign() * randUpTo(maxVariation);
		int64_t vserverDelayB = serverDelay;

		esicStep(&sicA, 
			timeSinceStartA + startTimeA, 
			timeSinceStartA + vtAS + serverTime, 
			timeSinceStartA + vtAS + serverTime + vserverDelayA,
			timeSinceStartA + startTimeA + vtAS + vtSA + vserverDelayA);

		esicStep(&sicB, 
			timeSinceStartB + startTimeB, 
			timeSinceStartB + vtBS + serverTime, 
			timeSinceStartB + vtBS + serverTime + vserverDelayB,
			timeSinceStartB + startTimeB + vtBS + vtSB + vserverDelayB);
	}

	int64_t timeSinceStart = 800*1000000;
	int64_t tS = timeSinceStart + serverTime;
	int64_t tS_A = esicTime(&sicA, timeSinceStart + startTimeA);
	int64_t tS_B = esicTime(&sicB, timeSinceStart + startTimeB);
	
	printf("Parallel Variations:\n");
	printf("time ServerA: %ld.\n", tS);
	printf("Server Time Acording to NodeA: %ld. Diff to RealServer: %ld.\n", tS_A, tS - tS_A);
	printf("Server Time Acording to NodeB: %ld. Diff to RealServer: %ld.\n", tS_B, tS - tS_B);
	
	assertInMargin("Parallel Variations: server time A", tS_A, tS, 1000);
	assertInMargin("Parallel Variations: server time B", tS_B, tS, 1000);

	esicEnd(&sicA);
	esicEnd(&sicB);
}


void syncNoDifferenceInClocks() {
	ESicData sic;
	esicInit(&sic);
	
	for(long long i=0; i< 60000; i++){
		esicStep(&sic, i*1000000, i*1000000 + 10, i*1000000 + 11, i*1000000 + 21);
	}

	assert("Synced algorithm no difference in clocks actual_m", sic.actual_m, 0);
	assert("Synced algorithm no difference in clocks actual_c", sic.actual_c, 0);
	assert("Synced algorithm no difference in clocks", esicTime(&sic, 1000), 1000);

	esicEnd(&sic);
}

void syncServerInFuture() {
	ESicData sic;
	esicInit(&sic);
	
	for(long long i=0; i< 60000; i++){
		esicStep(&sic, i*1000000, (i+1)*1000000 + 10, (i+1)*1000000 + 11, i*1000000 + 21);
	}

	assert("Synced algorithm server in future actual_m", sic.actual_m, 0);
	assert("Synced algorithm server in future actual_c", sic.actual_c, -1000000);
	assert("Synced algorithm server in future", esicTime(&sic, 1000), 1001000);

	esicEnd(&sic);
}

void syncServerInPast() {
	ESicData sic;
	esicInit(&sic);
	
	for(long long i=0; i< 60000; i++){
		esicStep(&sic, i*1000000, (i-1)*1000000 + 10, (i-1)*1000000 + 11, i*1000000 + 21);
	}

	assert("Synced algorithm server in past actual_m", sic.actual_m, 0);
	assert("Synced algorithm server in past actual_c", sic.actual_c, +1000000);
	assert("Synced algorithm server in past", esicTime(&sic, 1000000), 0);

	esicEnd(&sic);
}

/** input file parsing **/
struct ParsedT {
	int type;
	int64_t t[4];
};
int main(int argc, char** argv){
	srand(seed);

	syncStatesTestCase();
	syncNoDifferenceInClocks();
	syncServerInFuture();
	syncServerInPast();
	parallel();
	parallelSimulatedVariations();
	syncServerDifFrequency();

	return reportResults();
}
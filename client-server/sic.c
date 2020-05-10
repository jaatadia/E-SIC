#include "sic.h"
#include "linearfit.h"
#include <limits.h>

void updateMedians(SicData* sic, double phiEstimate);
void updateRoundTripTime(SicData* sic, long long rtt);
long long min(long long* array, int start, int size, int maxSize);
int rttChangeDectected(SicData* sic);


void sicInit(SicData* sic, int epoch) {
	
	sic->state = NO_SYNC;

	sic->errSync = epoch;
	sic->preSync = INT_MAX - P;
	sic->epochSync = INT_MAX - P;
	
	sic->phiAccumulator = 0;
    sic->medianNextPos = 0;
    sic->medianSize = 0;

    sic->rttNextPos = 0;
    sic->rttSize = 0;
    sic->rttFirst = 0;
    sic->rttLast = 0;

    sic->actual_m = 0;
    sic->actual_c = 0;	
}

void sicStep(SicData* sic, int epoch, long long t1, long long t2, long long t3, long long t4) {
	updateMedians(sic, t1 - t2 + (t2 - t1 + t4 - t3) / 2.0);
	updateRoundTripTime(sic, t4 - t1);
	if (rttRoutePreserved(sic)) {
		if(epoch >= sic->preSync + P) {
			sic->state = PRE_SYNC;
		}
	} else {
		sicInit(sic, epoch);
	}
	if (sic->state == SYNC && epoch >= sic->epochSync + P) {
		LinearFitResponse linearFitResponse;
		linearFit(&linearFitResponse, sic->Wmedian, sic->medianNextPos, sic->medianSize);
		sic->epochSync = epoch;
		sic->actual_m = (1 - ALPHA) * linearFitResponse.m + ALPHA * sic->actual_m;
		sic->actual_c = linearFitResponse.c;	
	} else if(sic -> state == NO_SYNC && epoch >= sic->errSync + MEDIAN_MAX_SIZE) {
		sic->preSync = epoch;
		sic->state = PRE_SYNC;
	} else if(sic -> state == PRE_SYNC && epoch >= sic->preSync + P) {
		LinearFitResponse linearFitResponse;
		linearFit(&linearFitResponse, sic->Wmedian, sic->medianNextPos, sic->medianSize);
		sic->state = SYNC;
		sic->epochSync = epoch;
		sic->actual_m = linearFitResponse.m;
		sic->actual_c = linearFitResponse.c;
	}
}

void updateMedians(SicData* sic, double phiEstimate) {
	sic->Wm[nex]


	if(sic->medianSize == MEDIAN_MAX_SIZE) {
		sic->Wm[sic->medianNextPos] = phiEstimate;
		sic->Wmedian[sic->medianNextPos] = sic->phiAccumulator / MEDIAN_MAX_SIZE;
		sic->medianNextPos = (sic->medianNextPos + 1) % MEDIAN_MAX_SIZE;
	} else {
		sic->phiAccumulator = sic->phiAccumulator + phiEstimate;
		sic->Wm[sic->medianSize] = phiEstimate;
		sic->Wmedian[sic->medianSize] = sic->phiAccumulator / (sic->medianSize + 1);
		sic->medianSize++;
	}
}

// TODO optimize this
void updateRoundTripTime(SicData* sic, long long rtt) {
	if(sic->rttSize == 2 * MEDIAN_MAX_SIZE) {
		sic->Wrtt[sic->rttNextPos] = rtt;
		sic->rttNextPos = (sic->rttNextPos + 1) % (2 * MEDIAN_MAX_SIZE);

		sic->rttFirst = min(sic->Wrtt, 
			sic->rttNextPos,
			MEDIAN_MAX_SIZE,
			2 * MEDIAN_MAX_SIZE); 

		sic->rttLast = min(sic->Wrtt, 
			(sic->rttNextPos + MEDIAN_MAX_SIZE) % (2 * MEDIAN_MAX_SIZE),
			MEDIAN_MAX_SIZE,
			 2 * MEDIAN_MAX_SIZE);
	} else {
		sic->Wrtt[sic->rttSize] = rtt;
		sic->rttSize++;

		sic->rttFirst = min(sic->Wrtt, 
			0, 
			sic->rttSize / 2,
			2 * MEDIAN_MAX_SIZE);

		sic->rttLast = min(sic->Wrtt, 
			sic->rttSize / 2, 
			sic->rttSize / 2,
			2 * MEDIAN_MAX_SIZE); 
	}
}

long long min(long long* array, int start, int size, int maxSize) {
	long long min = array[start];
	for (int i = 1; i < size; i ++) {
		long long value = array[(start + i) % maxSize];
		if(value < min) {
			min = value;
		}
	}
	return min;
}

// |rttl - rttf| <= errRTT * min(Wrtt)
int rttRoutePreserved(SicData* sic) {
	long long smaller;
	long long bigger;
	if(sic->rttFirst >= sic->rttLast) {
		smaller = sic->rttLast;
		bigger = sic->rttFirst;
	} else {
		smaller = sic->rttFirst;
		bigger = sic->rttLast;
	}
	return (bigger - smaller) <= errRTT * smaller;
}





















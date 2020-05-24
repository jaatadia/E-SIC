#include "sic.h"

void updateMedians(SicData* sic, double phiEstimate);
void updateRoundTripTime(SicData* sic, long long rtt);
long long min(long long* array, int start, int size, int maxSize);
int rttChangeDectected(SicData* sic);

void sicReset(SicData* sic){
	sic->syncSteps = 0;
	
	initCircularOrderedArray(&sic->Wm);
	initCircularLinearFitArray(&sic->Wmedian);

    sic->rttNextPos = 0;
    sic->rttSize = 0;
    sic->rttFirst = 0;
    sic->rttLast = 0;
}


void sicInit(SicData* sic) {
	sicReset(sic);
	sic->state = NO_SYNC;
    sic->actual_m = 0;
    sic->actual_c = 0;	
}

void sicStepTimeout(SicData* sic){
	sic->to++;
	if(sic->to == MAX_to){
		if(sic->state == NO_SYNC) { 
			sicInit(sic);
		} else { //if we have already an actual_m and actual_c we want to keep them
			sicReset(sic);
			sic->state = RE_SYNC;
		}
	}
}

void sicStep(SicData* sic, long long t1, long long t2, long long t3, long long t4) {
	sic->to=0;
	insertOrdered(&sic->Wm, t1 - t2 + (t2 - t1 + t4 - t3) / 2.0); // Wm <- t1 - t2 + (t2 - t1 + t4 - t3) / 2.0 
	insertPoint(&sic->Wmedian, t1, median(&sic->Wm)); // Wmedian <- (t1, median(Wm))
	sic->syncSteps++;

	/* updateRoundTripTime(sic, t4 - t1);
	if (!rttRoutePreserved(sic)) {
		sicInit(sic);
	} */

	if ((sic->state == PRE_SYNC || sic->state == SYNC) && sic->syncSteps == P) {
		linearFit(&sic->Wmedian);
		sic->state = SYNC;
		sic->syncSteps = 0;
		sic->actual_m = (1 - ALPHA) * sic->Wmedian.m + ALPHA * sic->actual_m;
		sic->actual_c = (1 - ALPHA) * sic->Wmedian.c + ALPHA * sic->actual_c;	
	} else if((sic->state == NO_SYNC || sic->state == RE_SYNC) && sic->syncSteps == P + MEDIAN_MAX_SIZE) {
		linearFit(&sic->Wmedian);
		sic->state = PRE_SYNC;
		sic->syncSteps = 0;
		sic->actual_m = sic->Wmedian.m;
		sic->actual_c = sic->Wmedian.c;
	} 
}

int sicTimeAvailable(SicData* sic){
	return sic->state > NO_SYNC;
}

long long sicTime(SicData* sic, long long systemClock){
	return systemClock - (systemClock*sic->actual_m + sic->actual_c);
}


// TODO optimize and extract this
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

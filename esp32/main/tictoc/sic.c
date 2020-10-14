#include "sic.h"

//#define TICTOC_SIC_DEBUG

void updateMedians(SicData* sic, double phiEstimate);
void updateRoundTripTime(SicData* sic, int64_t rtt);
int64_t min(int64_t* array, int start, int size, int maxSize);
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
			#ifdef TICTOC_SIC_DEBUG
			printf("SIC - Restarting NO_SYNC state.");
			#endif
		} else { //if we have already an actual_m and actual_c we want to keep them
			sicReset(sic);
			sic->state = RE_SYNC;
			#ifdef TICTOC_SIC_DEBUG
			printf("SIC - Entered RE_SYNC state.");
			#endif
		}
	}
}

void sicStep(SicData* sic, int64_t t1, int64_t t2, int64_t t3, int64_t t4) {
	sic->to=0;
	insertOrderedWithTime(&sic->Wm, t4 - t3 - (t2 - t1 + t4 - t3) / 2.0, t4); // Wm <- t1 - t2 + (t2 - t1 + t4 - t3) / 2.0 
	Node* median = medianNode(&sic->Wm);
	insertPoint(&sic->Wmedian, median->t, median->value); // Wmedian <- (t1, median(Wm))
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
		#ifdef TICTOC_SIC_DEBUG
		printf("SIC - Entered SYNC state: new m: %f c: %f\n.", sic->actual_m, sic->actual_c);
		#endif
	} else if((sic->state == NO_SYNC || sic->state == RE_SYNC) && sic->syncSteps == P + MEDIAN_MAX_SIZE) {
		linearFit(&sic->Wmedian);
		sic->state = PRE_SYNC;
		sic->syncSteps = 0;
		sic->actual_m = sic->Wmedian.m;
		sic->actual_c = sic->Wmedian.c;
		#ifdef TICTOC_SIC_DEBUG
		printf("SIC - Entered PRE_SYNC state: new m: %f c: %f\n.", sic->actual_m, sic->actual_c);
		#endif
	} 
}

int sicTimeAvailable(SicData* sic){
	return sic->state > NO_SYNC;
}

int64_t sicTime(SicData* sic, int64_t systemClock){
	return systemClock - (systemClock*sic->actual_m + sic->actual_c);
}


// TODO optimize and extract this
void updateRoundTripTime(SicData* sic, int64_t rtt) {
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

int64_t min(int64_t* array, int start, int size, int maxSize) {
	int64_t min = array[start];
	for (int i = 1; i < size; i ++) {
		int64_t value = array[(start + i) % maxSize];
		if(value < min) {
			min = value;
		}
	}
	return min;
}

// |rttl - rttf| <= errRTT * min(Wrtt)
int rttRoutePreserved(SicData* sic) {
	int64_t smaller;
	int64_t bigger;
	if(sic->rttFirst >= sic->rttLast) {
		smaller = sic->rttLast;
		bigger = sic->rttFirst;
	} else {
		smaller = sic->rttFirst;
		bigger = sic->rttLast;
	}
	return (bigger - smaller) <= errRTT * smaller;
}

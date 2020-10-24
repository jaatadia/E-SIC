#include "sic.h"
#include "halfSampleMode.h"
#include <stdio.h>

#define TICTOC_SIC_DEBUG

int SAMPLE_WINDOWS_SIZE = 3;
int SAMPLE_WINDOWS[] = {300, 150, 75};

void updateSamples(SicData* sic, int64_t phi, int64_t t);
void calculateLinearFit(SicData* sic, LinearFitResult* result);
int64_t getPhi(void * array, int pos);
double getPhiDouble(void * array, int pos);
double getTime(void * array, int pos);

void sicReset(SicData* sic){
	sic->syncSteps = 0;
	initCircularOrderedArray(&sic->Wm);
	initCircularLinearFitArray(&sic->Wmode);
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
	
	int64_t phi = t4 - t3 - (t2 - t1 + t4 - t3) / 2.0;
	int64_t t = t3 + phi;

	updateSamples(sic, phi, t);
	
	sic->syncSteps++;

	if ((sic->state == PRE_SYNC || sic->state == SYNC) && sic->syncSteps == P) {
		LinearFitResult result;
		calculateLinearFit(sic, &result);

		sic->state = SYNC;
		sic->syncSteps = 0;
		sic->actual_m = (1 - ALPHA) * result.m + ALPHA * sic->actual_m;
		sic->actual_c = (1 - ALPHA) * result.c + ALPHA * sic->actual_c;	
		#ifdef TICTOC_SIC_DEBUG
		printf("SIC - Entered SYNC state: new m: %f c: %f\n.", sic->actual_m, sic->actual_c);
		#endif
	} else if((sic->state == NO_SYNC || sic->state == RE_SYNC) && sic->syncSteps == P + SAMPLES_SIZE) {
		LinearFitResult result;
		calculateLinearFit(sic, &result);

		sic->state = PRE_SYNC;
		sic->syncSteps = 0;
		sic->actual_m = result.m;
		sic->actual_c = result.c;
		#ifdef TICTOC_SIC_DEBUG
		printf("SIC - Entered PRE_SYNC state: new m: %f c: %f\n.", sic->actual_m, sic->actual_c);
		#endif
	} 
}

void updateSamples(SicData* sic, int64_t phi, int64_t t){	
	insertOrderedWithTime(&sic->Wm, phi, t);

	
	if(sic->state == PRE_SYNC || sic->state == SYNC || sic->syncSteps >= SAMPLES_SIZE){
		int windowsSize = 3;
		int windows[] = {300, 150, 75};
		int modePosition = halfSampleModeWindowedMedianPosition(windows, windowsSize, &sic->Wm, 0, SAMPLES_SIZE, getPhi);

		insertPoint(&sic->Wmode, sic->Wm.array[modePosition - 1].time, sic->Wm.array[modePosition - 1].value);
		insertPoint(&sic->Wmode, sic->Wm.array[modePosition].time, sic->Wm.array[modePosition].value);
		insertPoint(&sic->Wmode, sic->Wm.array[modePosition + 1].time, sic->Wm.array[modePosition + 1].value);
	}
}

void calculateLinearFit(SicData* sic, LinearFitResult* result){
	/* 
	int modePosition = halfSampleModePosition(&sic->Wm, 0, SAMPLES_SIZE, getPhi);
	int start = modePosition - SIC_LINEAR_FIT_WINDOW;
	int end = modePosition + SIC_LINEAR_FIT_WINDOW;
	if(start < 0) start = 0;
	if(end > SAMPLES_SIZE) end = SAMPLES_SIZE;
	linearFitFunction(&sic->Wm, start, end, getTime, getPhiDouble, result); 
	*/

	/*
	int modePosition = halfSampleModeWindowedMedianPosition(SAMPLE_WINDOWS, SAMPLE_WINDOWS_SIZE, &sic->Wm, 0, SAMPLES_SIZE, getPhi);
	int start = modePosition - SIC_LINEAR_FIT_WINDOW;
	int end = modePosition + SIC_LINEAR_FIT_WINDOW;
	if(start < 0) start = 0;
	if(end > SAMPLES_SIZE) end = SAMPLES_SIZE;
	linearFitFunction(&sic->Wm, start, end, getTime, getPhiDouble, result); 
	*/

	linearFitResult(&sic->Wmode, result);

}


int64_t getPhi(void * array, int pos){
	return ((CircularOrderedArray*) array)->array[pos].value;
}

double getPhiDouble(void * array, int pos){
	return getPhi(array, pos);
}

double getTime(void * array, int pos){
	return ((CircularOrderedArray*) array)->array[pos].time;
}

int sicTimeAvailable(SicData* sic){
	return sic->state > NO_SYNC;
}

int64_t sicTime(SicData* sic, int64_t systemClock){
	return systemClock - (systemClock*sic->actual_m + sic->actual_c);
}
#include "sic.h"
#include "halfSampleMode.h"
#include <stdio.h>

//#define TICTOC_SIC_DEBUG

int SAMPLE_WINDOWS_SIZE = 3;
int SAMPLE_WINDOWS[] = {300, 150, 75};

void updateSamples(SicData* sic, int64_t t1, int64_t t2, int64_t t3, int64_t t4);
void calculateLinearFit(SicData* sic, LinearFitResult* result);

int64_t getPhi(void * array, int pos);
double getPhiDouble(void * array, int pos);
int64_t getTime(void * array, int pos);
double getTimeDouble(void * array, int pos);



/***********************************
* 			Wm Definitions
***********************************/
// TODO extract this to its own module

struct WmNode { 
	int64_t phi;
    int64_t time;
};

typedef struct WmNode WmNode;


void cpyWmNode(void * source, void * target){
	WmNode* sourceWmNode = (WmNode*) source;
	WmNode* targetWmNode = (WmNode*) target;
	targetWmNode->phi = sourceWmNode->phi;
	targetWmNode->time = sourceWmNode->time;
}

int cmpWmNode(void * first, void * second){
	WmNode* firstWmNode = (WmNode*) first;
	WmNode* secondWmNode = (WmNode*) second;
	return firstWmNode->phi - secondWmNode->phi;
}

//----------------------------------


void sicReset(SicData* sic){
	sic->syncSteps = 0;
	resetCircularOrderedArray(sic->Wm);
}

void sicInit(SicData* sic) {
	sic->Wm = initCircularOrderedArray(SAMPLES_SIZE, sizeof(WmNode), cpyWmNode, cmpWmNode);
	sic->Wmode = initCircularOrderedArray(3*MODE_SAMPLES_SIZE, sizeof(WmNode), cpyWmNode, cmpWmNode);
	sicReset(sic);
	sic->state = NO_SYNC;
    sic->actual_m = 0;
    sic->actual_c = 0;	
}



void sicEnd(SicData* sic) {
	freeCircularOrderedArray(sic->Wm);
	freeCircularOrderedArray(sic->Wmode);
}

void sicStepTimeout(SicData* sic){
	sic->to++;
	if(sic->to == MAX_to){
		if(sic->state == NO_SYNC) { 
			sicReset(sic);
			sic->state = NO_SYNC;
			#ifdef TICTOC_SIC_DEBUG
			printf("SIC - Restarting NO_SYNC state.\n");
			#endif
		} else { //if we have already an actual_m and actual_c we want to keep them
			sicReset(sic);
			sic->state = RE_SYNC;
			#ifdef TICTOC_SIC_DEBUG
			printf("SIC - Entered RE_SYNC state.\n");
			#endif
		}
	}
}

void sicStep(SicData* sic, int64_t t1, int64_t t2, int64_t t3, int64_t t4) {
	sic->to=0;
	
	sic->syncSteps++;
	updateSamples(sic, t1, t2, t3, t4);

	if ((sic->state == PRE_SYNC || sic->state == SYNC) && sic->syncSteps == P) {
		LinearFitResult result;
		calculateLinearFit(sic, &result);

		sic->state = SYNC;
		sic->syncSteps = 0;
		sic->actual_m = (1 - ALPHA) * result.m + ALPHA * sic->actual_m;
		sic->actual_c = (1 - ALPHA) * result.c + ALPHA * sic->actual_c;	
		#ifdef TICTOC_SIC_DEBUG
		printf("SIC - Entered SYNC state: new m: %f c: %f.\n", sic->actual_m, sic->actual_c);
		#endif
	} else if((sic->state == NO_SYNC || sic->state == RE_SYNC) && sic->syncSteps == P * MODE_SAMPLES_SIZE + SAMPLES_SIZE) {
		LinearFitResult result;
		calculateLinearFit(sic, &result);

		sic->state = PRE_SYNC;
		sic->syncSteps = 0;
		sic->actual_m = result.m;
		sic->actual_c = result.c;
		#ifdef TICTOC_SIC_DEBUG
		printf("SIC - Entered PRE_SYNC state: new m: %f c: %f.\n", sic->actual_m, sic->actual_c);
		#endif
	} 
}

#include <inttypes.h>	
void updateSamples(SicData* sic, int64_t t1, int64_t t2, int64_t t3, int64_t t4){	
	WmNode node;
	node.phi = (t1 -t2 - t3 + t4) / 2.0;
	node.time = t1;
	insertOrdered(sic->Wm, &node);

	if(((sic->state == NO_SYNC || sic->state == RE_SYNC) && (sic->syncSteps >= SAMPLES_SIZE) && ((sic->syncSteps - SAMPLES_SIZE) % MODE_SAMPLES_SIZE == 0)) ||
		((sic->state == PRE_SYNC || sic->state == SYNC) && sic->syncSteps == P)) {
		int modePosition = halfSampleModeWindowedMedianPosition(SAMPLE_WINDOWS, SAMPLE_WINDOWS_SIZE, sic->Wm, 0, SAMPLES_SIZE, getPhi);
		node.phi = getPhi(sic->Wm, modePosition);
		node.time = getTime(sic->Wm, modePosition);
		insertOrdered(sic->Wmode, &node);		

		node.phi = getPhi(sic->Wm, modePosition-1);
		node.time = getTime(sic->Wm, modePosition-1);
		insertOrdered(sic->Wmode, &node);		

		node.phi = getPhi(sic->Wm, modePosition+1);
		node.time = getTime(sic->Wm, modePosition+1);
		insertOrdered(sic->Wmode, &node);		
	}
}

void calculateLinearFit(SicData* sic, LinearFitResult* result){
	linearFit(sic->Wmode, 0, 3*MODE_SAMPLES_SIZE, getTimeDouble, getPhiDouble, result); 
}


int64_t getPhi(void * array, int pos){
	return ((WmNode*)((CircularOrderedArray*) array)->data[pos])->phi;
}

double getPhiDouble(void * array, int pos){
	return getPhi(array, pos);
}

int64_t getTime(void * array, int pos){
	return ((WmNode*)((CircularOrderedArray*) array)->data[pos])->time;
}

double getTimeDouble(void * array, int pos){
	return getTime(array, pos);
}

int sicTimeAvailable(SicData* sic){
	return sic->state > NO_SYNC;
}

int64_t sicTime(SicData* sic, int64_t systemClock){
	return systemClock - (int64_t)(systemClock*sic->actual_m + sic->actual_c);
}
#include "sic.h"
#include "halfSampleMode.h"
#include <stdio.h>

//#define TICTOC_SIC_DEBUG

int SAMPLE_WINDOWS_SIZE = 3;
int SAMPLE_WINDOWS[] = {300, 150, 75};

void updateSamples(SicData* sic, int64_t t1, int64_t t2, int64_t t3, int64_t t4);
void calculateLinearFit(SicData* sic);

/***********************************
* 			Wm Definitions
***********************************/
// TODO extract this to its own module

struct WmNode { 
	int64_t cmp;
	int64_t phi;
    int64_t time;
};

typedef struct WmNode WmNode;


void cpyWmNode(void * source, void * target){
	WmNode* sourceWmNode = (WmNode*) source;
	WmNode* targetWmNode = (WmNode*) target;

	targetWmNode->cmp = sourceWmNode->cmp;
	targetWmNode->phi = sourceWmNode->phi;
	targetWmNode->time = sourceWmNode->time;
}

double cmpWmNode(void * first, void * second){
	WmNode* firstWmNode = (WmNode*) first;
	WmNode* secondWmNode = (WmNode*) second;
	return (double)firstWmNode->cmp - (double)secondWmNode->cmp;
}

int64_t getCmp(void * array, int pos){
	return ((WmNode*)((CircularOrderedArray*) array)->data[pos])->cmp;
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

//----------------------------------


void sicReset(SicData* sic){
	sic->syncSteps = 0;
	resetCircularOrderedArray(sic->Wm);
}

void sicInit(SicData* sic) {
	sic->Wm = initCircularOrderedArray(SAMPLES_SIZE, sizeof(WmNode), cpyWmNode, cmpWmNode);
	sic->Wmode = initCircularOrderedArray(MODE_SAMPLES_SIZE, sizeof(WmNode), cpyWmNode, cmpWmNode);
	sicReset(sic);
	sic->state = NO_SYNC;
    sic->actual_m = 0;
    sic->actual_c = 0;	
    sic->lastN = 1;
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

	if(((sic->state == NO_SYNC || sic->state == RE_SYNC) && sic->syncSteps == SAMPLES_SIZE) ||
		((sic->state == PRE_SYNC || sic->state == SYNC) && sic->syncSteps == P)){
		
		calculateLinearFit(sic);
		sic->state = SYNC;
		sic->syncSteps = 0;
		#ifdef TICTOC_SIC_DEBUG
		printf("SIC - SYNC state: new m: %f c: %f.\n", sic->actual_m, sic->actual_c);
		#endif
	} 
}



/*
int64_t iteration = 0;
int64_t lastPos = -1;
int64_t last = 0;
void printSamples(void* elem){
	int64_t current = ((WmNode*) elem)->cmp;
	if(lastPos != -1 && current < last){
		printf("!!!order failed %ld: %ld, %ld: %ld.\n", lastPos, last, lastPos +1, current);	
		printf("iteration %ld.\n", iteration);	
		exit(1);	
	}
	last = current;
	lastPos++; 
	iteration ++;
}
*/

#include <inttypes.h>	
void updateSamples(SicData* sic, int64_t t1, int64_t t2, int64_t t3, int64_t t4){	
	WmNode node;
	
	node.phi = (t1 - t2 - sic->lastN * t3 + sic->lastN *t4)/(sic->lastN+1);		
	node.cmp = node.phi;
	node.time = (t1+t4)/2;
		
	insertOrdered(sic->Wm, &node);

	HalfSampleModeResult hsmResult;
	if(sic->syncSteps % P == 0){
		halfSampleModeWindow(sic->Wm, 0, sic->Wm->size, getCmp, &hsmResult, SIC_LINEAR_FIT_WINDOW);

		for(int i=hsmResult.position1; i<hsmResult.position2; i++){
			WmNode node;	
			node.phi = getPhi(sic->Wm, i);
			node.cmp = getCmp(sic->Wm, i);
			node.time = getTime(sic->Wm, i);
			insertOrdered(sic->Wmode, &node);
		}
	}	

	//node.phi = (t1-t2-t3+t4)/2;
	//node.phi = t1-t2;
	//node.phi = t4-t3;


	//node.cmp = (t1-t2-t3+t4)/2;
	//node.cmp = t1-t2;
	//node.cmp = t4-t3;


	//node.time = t4;

	//node.cmp = t1-t2;
	//node.phi = t1-t2;
	//node.time = t1;
	//insertOrdered(sic->Wm, &node);

	/*
	node.cmp = t4-t3;
	node.phi = t4-t3;
	node.time = t4;
	insertOrdered(sic->Wm2, &node);
	*/

	/*
	lastPos = -1;
	foreach(sic->Wm, printSamples);
	*/
	
	/*if(sic->syncSteps % P == 0){
		HalfSampleModeResult result;
		halfSampleMode(sic->Wm, 0, sic->Wm->size, getCmp, &result);
		cpyWmNode((WmNode*) sic->Wm->data[result.position1], &node);
		insertOrdered(sic->Wmode, &node);
		if(result.position2 != -1) {
			cpyWmNode((WmNode*) sic->Wm->data[result.position2], &node);
			insertOrdered(sic->Wmode, &node);
		}
		sic->phi=result.mode;

	}*/
}

void calculateLinearFit(SicData* sic){
	LinearFitResult result;
	//HalfSampleModeResult hsmResult;
	
//	LinearFitResult result2;
//	HalfSampleModeResult hsmResult2;
	
/*	halfSampleModeWindow(sic->Wm, 0, sic->Wm->size, getCmp, &hsmResult, SIC_LINEAR_FIT_WINDOW);

	for(int i=hsmResult.position1; i<hsmResult.position2; i++){
			WmNode node;	
			node.phi = getPhi(sic->Wm, i);
			node.cmp = getCmp(sic->Wm, i);
			node.time = getTime(sic->Wm, i);
			insertOrdered(sic->Wmode, &node);
	}
*/
	
	//linearFit(sic->Wm, 0, sic->Wm->size, getTimeDouble, getPhiDouble, &result); 
	linearFit(sic->Wmode, 0, sic->Wmode->size, getTimeDouble, getPhiDouble, &result); 


//	halfSampleModeWindow(sic->Wm2, 0, sic->Wm2->size, getCmp, &hsmResult2, SIC_LINEAR_FIT_WINDOW);
//	linearFit(sic->Wm2, hsmResult2.position1, hsmResult2.position2, getTimeDouble, getPhiDouble, &result2); 

	/*
	halfSampleMode(sic->Wm, 0, sic->Wm->size, getCmp, &hsmResult);
	

	HalfSampleModeResult hsmResult2;
	halfSampleMode(sic->Wm2, 0, sic->Wm2->size, getCmp, &hsmResult2);

	double n = 0.2;
	sic->phi = (n*hsmResult.mode + hsmResult2.mode)/(1+n);*/


	sic->actual_m = result.m;
	sic->actual_c = result.c;

//	sic->actual_m2 = result2.m;
//	sic->actual_c2 = result2.c;
}

int sicTimeAvailable(SicData* sic){
	return sic->state > NO_SYNC;
}

int64_t computePhi(SicData* sic, int64_t systemClock){
	return (int64_t)(systemClock*sic->actual_m + sic->actual_c);
	//return sic->phi;
	
	//double n = 1;
	//return (n*(systemClock*sic->actual_m + sic->actual_c) + (systemClock*sic->actual_m2 + sic->actual_c2))/(1+n);
}

int64_t sicTime(SicData* sic, int64_t systemClock){
	return systemClock - computePhi(sic, systemClock);
}

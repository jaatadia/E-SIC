#include "esic.h"
#include "halfSampleMode.h"
#include <stdio.h>
#include <inttypes.h>	
#include "node.h"

//#define TICTOC_SIC_DEBUG

void esicReset(ESicData* esic);
void esic_updateSamples(ESicData* esic, int64_t t1, int64_t t2, int64_t t3, int64_t t4);
void esic_calculateLinearFit(ESicData* esic);


//function to retrieve the value to use for the comparison in the position pos of the array.
int64_t esic_getCmp(void * array, int pos){
	return ((Node*)((CircularOrderedArray*) array)->data[pos])->cmp;
}

int64_t esic_getPhi(void * array, int pos){
	return ((Node*)((CircularOrderedArray*) array)->data[pos])->phi;
}

double esic_getPhiDouble(void * array, int pos){
	return esic_getPhi(array, pos);
}

int64_t esic_getTime(void * array, int pos){
	return ((Node*)((CircularOrderedArray*) array)->data[pos])->time;
}

double esic_getTimeDouble(void * array, int pos){
	return esic_getTime(array, pos);
}

//----------------------------------


void esicInit(ESicData* esic) {
	esic->Wm = initCircularOrderedArray(esic_SAMPLES_SIZE, sizeof(Node), cpyNode, cmpNode);
	esic->Wmode = initCircularOrderedArray(esic_MODE_WINDOW * esic_MODE_SAMPLES, sizeof(Node), cpyNode, cmpNode);
	esicReset(esic);
	esic->state = esic_NO_SYNC;
    esic->actual_m = 0;
    esic->actual_c = 0;	
}

// reset internal information without discarding current estimation paramenters.
void esicReset(ESicData* esic){
	esic->syncSteps = 0;
	resetCircularOrderedArray(esic->Wm);
	resetCircularOrderedArray(esic->Wmode);
}

void esicEnd(ESicData* esic) {
	freeCircularOrderedArray(esic->Wm);
	freeCircularOrderedArray(esic->Wmode);
}

void esicStepTimeout(ESicData* esic){
	esic->to++;
	//If the amount of timeouted requests reached the threshold we restart the algorithm.
	if(esic->to == esic_MAX_to){
		if(esic->state == esic_NO_SYNC) { 
			esicReset(esic);
			esic->state = esic_NO_SYNC;
			#ifdef TICTOC_SIC_DEBUG
			printf("SIC - Restarting NO_SYNC state.\n");
			#endif
		} else { 
			//If we have already an actual_m and actual_c we want to keep them
			esicReset(esic);
			esic->state = esic_RE_SYNC;
			#ifdef TICTOC_SIC_DEBUG
			printf("SIC - Entered RE_SYNC state.\n");
			#endif
		}
	}
}

void esicStep(ESicData* esic, int64_t t1, int64_t t2, int64_t t3, int64_t t4) {
	esic->to=0; // Successfully received data -> reset timeout counter
	esic->syncSteps++; // Increase loop counter


	esic_updateSamples(esic, t1, t2, t3, t4);

	// If we filled the sample array for the first time or one minute has passed since the last estimation we get a new estimation of phi
	if(((esic->state == esic_NO_SYNC || esic->state == esic_RE_SYNC) && esic->syncSteps == esic_STARTUP_CYCLES) ||
		(esic->state == esic_SYNC && esic->syncSteps == esic_P)){
		
		esic_calculateLinearFit(esic);
		esic->state = esic_SYNC;
		esic->syncSteps = 0;
		#ifdef TICTOC_SIC_DEBUG
		printf("SIC - SYNC state: new m: %f c: %f.\n", esic->actual_m, esic->actual_c);
		#endif
	} 
}


// Calculates phi and inserts it in the corresponding arrays
void esic_updateSamples(ESicData* esic, int64_t t1, int64_t t2, int64_t t3, int64_t t4){	
	Node node;
	
	node.phi = (t1 - t2 - esic_N * t3 + esic_N * t4)/(esic_N+1);
	node.cmp = node.phi;
	node.time = (t1+t4)/2;
		
	// Insert the latest sample
	insertOrdered(esic->Wm, &node);

	// Each minute we get the mode values from the samples array and insert them mode array
	HalfSampleModeResult hsmResult;
	if(esic->syncSteps % esic_MODE_CYCLES == 0){
		halfSampleModeWindow(esic->Wm, 0, esic->Wm->size, esic_getCmp, esic_MODE_WINDOW, &hsmResult);

		for(int i=hsmResult.position1; i<hsmResult.position2; i++){
			Node node;	
			node.phi = esic_getPhi(esic->Wm, i);
			node.cmp = esic_getCmp(esic->Wm, i);
			node.time = esic_getTime(esic->Wm, i);
			insertOrdered(esic->Wmode, &node);
		}
	}	
}

// Combines the old and the new value using the given alpha factor.
double esic_ponderate(double previousValue, double newValue) {
	return esic_ALPHA * previousValue + (1-esic_ALPHA) * newValue;
}

// Uses the current data to estimate the phi parameters.
void esic_calculateLinearFit(ESicData* esic){
	LinearFitResult result;
	linearFit(esic->Wmode, 0, esic->Wmode->size, esic_getTimeDouble, esic_getPhiDouble, &result); 

	if(esic->state == esic_SYNC){
		esic->actual_m = esic_ponderate(esic->actual_m, result.m);
		esic->actual_c = esic_ponderate(esic->actual_c, result.c);	
	} else {
		esic->actual_m = result.m;
		esic->actual_c = result.c;	
	}
	
}

int esicTimeAvailable(ESicData* esic){
	return esic->state > esic_NO_SYNC;
}

int64_t esic_computePhi(ESicData* esic, int64_t systemClock){
	return (int64_t)(systemClock*esic->actual_m + esic->actual_c);
}

int64_t esicTime(ESicData* esic, int64_t systemClock){
	return systemClock - esic_computePhi(esic, systemClock);
}

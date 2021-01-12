#ifndef SIC_H
#define SIC_H

#include "linearfit.h"
#include "circularOrderedArray.h"

#define RTT_SIZE 600

//With the new implementation, indicate where to start and stop 
#define SAMPLES_SIZE 600
#define SIC_LINEAR_FIT_WINDOW 30

#define P 60
#define ALPHA 0.05
#define TIMEOUT 0.8
#define errRTT 0.8
#define MAX_to 6

#define NO_SYNC 0
#define RE_SYNC 1
#define PRE_SYNC 2
#define SYNC 3

struct SicData { 
	int state;
	int syncSteps;
    int to;

    CircularOrderedArray* Wm;

    double actual_m;
    double actual_c;
};

typedef struct SicData SicData;

void sicInit(SicData* sic);
void sicEnd(SicData* sic);

void sicStepTimeout(SicData* sic);
void sicStep(SicData* sic, int64_t t1, int64_t t2, int64_t t3, int64_t t4);

int sicTimeAvailable(SicData* sic);
int64_t sicTime(SicData* sic, int64_t systemClock);

#endif
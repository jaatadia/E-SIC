/*
 * sic.h
 *
 *  Created on: May 24, 2020
 *      Author: jaatadia@gmail.com
 */
#ifndef SIC_H
#define SIC_H

#define MEDIAN_MAX_SIZE 600
#define P 60
#define ALPHA 0.05
#define TIMEOUT 0.8
#define errRTT 0.8
#define MAX_to 6

#define NO_SYNC 0
#define RE_SYNC 1
#define PRE_SYNC 2
#define SYNC 3

#include <stdint.h>
#include "linearfit.h"
#include "circularOrderedArray.h"

struct SicData { 
	int state;
	int syncSteps;
    int to;

    CircularOrderedArray Wm;
    CircularLinearFitArray Wmedian;

    double actual_m;
    int64_t actual_c;
};

typedef struct SicData SicData;

void sicInit(SicData* sic);
void sicStepTimeout(SicData* sic);
void sicStep(SicData* sic, int64_t t1, int64_t t2, int64_t t3, int64_t t4);

int sicTimeAvailable(SicData* sic);
int64_t sicTime(SicData* sic, int64_t systemClock);

#endif

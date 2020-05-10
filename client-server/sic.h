#ifndef SIC_H
#define SIC_H

#define MEDIAN_MAX_SIZE 600
#define P 60
#define ALPHA 0.05
#define TIMEOUT 0.8
#define errRTT 0.8
#define MAX_to 6

#define NO_SYNC 0
#define PRE_SYNC 1
#define SYNC 2

struct SicData { 
	int state;
	int e_prev;

	int errSync;
	int preSync;
	int epochSync;

    CircularOrderedArray Wm;
    CircularOrderedArray Wmedian;

    int rttNextPos;
	int rttSize;
    long long Wrtt[2 * MEDIAN_MAX_SIZE];
    long long rttFirst;
    long long rttLast;

    double actual_m;
    double actual_c;
};

typedef struct SicData SicData;

void sicInit(SicData* sic, int epoch);
void sicStep(SicData* sic, int epoch, long long t1, long long t2, long long t3, long long t4);

#endif
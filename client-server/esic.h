#ifndef ESIC_H
#define ESIC_H

#include "linearfit.h"
#include "circularOrderedArray.h"

/* 
* This library represents the core of the esic algorithm.
*/


// Amount of samples historic samples the algorithm uses
#define esic_SAMPLES_SIZE 600 

// Amount of algorithm cycles between estimation parameters computation.
#define esic_P 60 

// Amount of cycles between the extraction of a new mode values from the samples
#define esic_MODE_CYCLES esic_P

/* 
* Each MODE_CYCLES cycles the algorithm will take this amount of samples arround the mode and store 
* it in the mode array.
*/
#define esic_MODE_WINDOW 15

// The historic amount of mode windows that will be remembered by the algorithm.
#define esic_MODE_SAMPLES 30

// The amount cycles before the algothm starts producing aproximations
#define esic_STARTUP_CYCLES esic_SAMPLES_SIZE

// Amount of consecutive timeouts the algorthm supports without discarding current status.
#define esic_MAX_to 6 

// Constant that represents the different states of the algorithm 
#define esic_NO_SYNC 0
#define esic_RE_SYNC 1
#define esic_SYNC 3

// Constant that represents the relation t(c->s)/t(s->c)
#define esic_N 1

// Constant that represents how much importance will the last aproximation have in the new one
#define esic_ALPHA 0

struct ESicData { 
    int state;
    int syncSteps;
    int to;

    CircularOrderedArray* Wm;
    CircularOrderedArray* Wmode;

    double actual_m;
    double actual_c;
};

typedef struct ESicData ESicData;

// Allocates resources
void esicInit(ESicData* esic);

// Frees allocated resources
void esicEnd(ESicData* esic);

// Indicate the algorithm that the current attempt to comunicate with the server resulted in timeout
void esicStepTimeout(ESicData* esic);

/* 
* Indicates the algorithm the current values of the exchange of information with the server.
* Where:
*   - t1: timestamp using the client clock of the moment the synchronization message was sent by the client.
*   - t2: timestamp using the server clock of the moment the synchronization message was received by the server.
*   - t3: timestamp using the server clock of the moment the response of synchronization message was sent by the server.
*   - t4: timestamp using the client clock of the moment the response of synchronization message was received by the client.
*/
void esicStep(ESicData* esic, int64_t t1, int64_t t2, int64_t t3, int64_t t4);

// Indicates if the algorithm has successfully synchronized with the server.
int esicTimeAvailable(ESicData* esic);

// Given the current systemClock, return the equivalent value in microseconds since epoch .
int64_t esicTime(ESicData* esic, int64_t systemClock);

#endif
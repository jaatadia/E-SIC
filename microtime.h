#ifndef MICRO_TIME_H
#define MICRO_TIME_H

#define NULL 0 

#include <stdint.h> 

long long epochInMicros();

//Writes the timestamp in a networksafeway using two positions of the response array starting in pos.
//we cannot use uint64 due to esp32 being 32bit architecture.
void encodeEpochInMicros(long long timestamp, int32_t* response, int pos);

//obtains the timestampe from the position pos of the response array encoded using encodeEpochInMicros.
long long decodeEpochInMicros(int32_t* response, int pos);

#endif
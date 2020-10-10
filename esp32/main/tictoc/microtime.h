#ifndef MICRO_TIME_H
#define MICRO_TIME_H

#ifndef NULL
#define NULL 0 
#endif

#include <stdint.h> 

int64_t epochInMicros();

//Writes the timestamp in a networksafeway using two positions of the response array starting in pos.
//we cannot use uint64 due to esp32 being 32bit architecture.
void encodeEpochInMicros(int64_t timestamp, int32_t* response, int pos);

//obtains the timestampe from the position pos of the response array encoded using encodeEpochInMicros.
int64_t decodeEpochInMicros(int32_t* response, int pos);

void microsToTimestamp(int64_t micros, char* stringBuffer);

#endif
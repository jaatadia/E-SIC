#include "microtime.h"
#include <sys/time.h>
#include <arpa/inet.h>

int64_t epochInMicros(){
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;	
}

void encodeEpochInMicros(int64_t timestamp, int32_t* response, int pos){
	int32_t seconds = timestamp / 1000000;
	int32_t micros = timestamp % 1000000;

	response[pos] = htonl(seconds);
	response[pos+1] = htonl(micros);
}

int64_t decodeEpochInMicros(int32_t* response, int pos){
	return ((int64_t)ntohl(response[pos])) * 1000000 + ntohl(response[pos+1]);
}
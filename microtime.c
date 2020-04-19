#include "microtime.h"
#include <sys/time.h>
#include <arpa/inet.h>

long long epochInMicros(){
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;	
}

void encodeEpochInMicros(long long timestamp, uint32_t* response, int pos){
	uint32_t highPart = timestamp >> 32;
	uint32_t lowPart = timestamp - ((long long) timestamp << 32);

	response[pos] = htonl(highPart);
	response[pos+1] = htonl(lowPart);
}

long long decodeEpochInMicros(uint32_t* response, int pos){
	return (((long long) ntohl(response[pos])) << 32) + ntohl(response[pos+1]);
}
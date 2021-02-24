#ifndef HSM_H
#define HSM_H

#include <stdint.h>

struct HalfSampleModeResult {
	int64_t mode;
	int position1;
	int position2;
};

typedef struct HalfSampleModeResult HalfSampleModeResult;

void halfSampleMode(void* array, int start, int end, int64_t(*fx)(void*, int), HalfSampleModeResult* result);

void halfSampleModeWindow(void* array, int start, int end, int64_t(*fx)(void*, int), HalfSampleModeResult* result, int windowSize);



#endif
#ifndef HSM_H
#define HSM_H

#include <stdint.h>

int64_t halfSampleMode(void* array, int start, int end, int64_t(*fx)(void*, int));
int halfSampleModePosition(void* array, int start, int end, int64_t(*fx)(void*, int));
int halfSampleModeWindowedMedianPosition(int* slidingWindows, int slidingWindowsSize, void* array, int start, int end, int64_t(*fx)(void*, int));

#endif
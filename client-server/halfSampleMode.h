#ifndef HSM_H
#define HSM_H

#include <stdint.h>

struct ModeWindow {
	int64_t start;
	int64_t end;
};

typedef struct ModeWindow ModeWindow;

int64_t halfSampleMode(void* array, int start, int end, int64_t(*fx)(void*, int));
int halfSampleModePosition(void* array, int start, int end, int64_t(*fx)(void*, int));



int halfSampleModeWindowedMedianPosition(int* slidingWindows, int slidingWindowsSize, void* array, int start, int end, int64_t(*fx)(void*, int));

void halfSampleModeWindow(int* slidingWindows, int slidingWindowsSize, void* array, int start, int end, int64_t(*fx)(void*, int), ModeWindow * modeWindow);


#endif
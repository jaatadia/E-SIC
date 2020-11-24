#include "halfSampleMode.h"

#include <stdio.h>
#include <inttypes.h>

//#define HSM_DEBUG
//#define HSM_WINDOW_DEBUG

int halfSampleStep(int N, void* array, int start, int end, int64_t(*fx)(void*, int)){
	#ifdef HSM_DEBUG
	printf("HalfSampleMode - start: %d, end: %d, N: %d.\n", start, end, N);
	#endif	
	
	int j = -1;
	int64_t wmin = (*fx)(array, end-1) - (*fx)(array, start);

	for(int i = start; i < end - (N-1); i++) {

		int64_t w = (*fx)(array, i + (N-1)) - (*fx)(array, i);
		if(w <= wmin) {
			wmin = w;
			j = i;
		}

		#ifdef HSM_DEBUG
		int64_t xi = (*fx)(array, i);
		int64_t xin = (*fx)(array, i + (N-1));
		printf("HalfSampleMode - i: %d, Xi: %ld, XiN-1: %ld, w: %ld.\n", i, xi, xin, w);
		#endif	
	}
	return j;
}


int64_t innerHalfSampleMode(void* array, int start, int end, int64_t(*fx)(void*, int)){
	int size = end - start;
	if(size == 1) {
		return (*fx)(array, start);
	}

	if(size == 2) {
		return ((*fx)(array, start) + (*fx)(array, start+1))/2;
	}

	if(size == 3) {
		int64_t x1 = (*fx)(array, start);
		int64_t x2 = (*fx)(array, start + 1);
		int64_t x3 = (*fx)(array, start + 2);

		if(x2 - x1 == x3 - x2) return x2;
		return (x2 - x1 < x3 - x2) ? (x1 + x2)/2 : (x2 + x3)/2;
	}

	int N = (size % 2 == 0) ? (size/2) : (size/2 + 1);
	int j = halfSampleStep(N, array, start, end, fx);

	#ifdef HSM_DEBUG
	printf("HalfSampleMode - j: %d N: %d.\n", j, N);
	#endif	

	return innerHalfSampleMode(array, j, j + N, fx);
}

void printHSMDebugInfo(void* array, int start, int end, int64_t(*fx)(void*, int), int64_t mode){
	printf("HalfSampleMode - Array Conent: [");	
	for (int i = start; i < end; i++){
		printf("%"PRId64, (*fx)(array, i));	
		if(i + 1 < end) printf(", ");	
	}
	printf("].\n");
	printf("HalfSampleMode - Mode: %"PRId64".\n", mode);
}

int64_t halfSampleMode(void* array, int start, int end, int64_t(*fx)(void*, int)){

	int64_t mode = innerHalfSampleMode(array, start, end, fx);
	#ifdef HSM_DEBUG
	printHSMDebugInfo(array, start, end, fx, mode);
	#endif	

	return mode;
}

int halfSampleModePosition(void* array, int start, int end, int64_t(*fx)(void*, int)){

	if((end - start) <= 2) return start;

	int64_t mode = halfSampleMode(array, start, end, fx);
	
	int firstMatch = -1;
	int lastMatch = -1;

	for(int i = start; i< end-1; i++){
		int64_t current = (*fx)(array, i);
		int64_t next = (*fx)(array, i+1);
		if(current == mode){
			lastMatch = i;
			if(firstMatch == -1) {
				firstMatch = i;
			}
			if(next>mode){
				return (lastMatch + firstMatch) / 2;
			}
		} else if(next > mode) {
			return i;
		}
	}

	if((*fx)(array, end-1) == mode) {
		lastMatch = end-1;
		return (lastMatch + firstMatch) / 2;
	}

	//we should never arrive here since the mode will never be in the las position of the ordered array 
	return mode;
}

int halfSampleModeWindowedMedianPosition(int* slidingWindows, int slidingWindowsSize, void* array, int start, int end, int64_t(*fx)(void*, int)) {
	ModeWindow modeWindow;
	halfSampleModeWindow(slidingWindows, slidingWindowsSize, array, start, end, fx, &modeWindow);
	return (modeWindow.start + modeWindow.end)/2;
}

void halfSampleModeWindow(int* slidingWindows, int slidingWindowsSize, void* array, int start, int end, int64_t(*fx)(void*, int), ModeWindow * modeWindow) {
	int medianPosition = (start + end)/2;
	int stepStart = start;
	int stepEnd = end;
	for(int i = 0; i < slidingWindowsSize; i++){
		int j = halfSampleStep(slidingWindows[i], array, stepStart, stepEnd, fx);
		stepStart = j;
		stepEnd = j + slidingWindows[i];
		medianPosition = j + slidingWindows[i]/2;

		#ifdef HSM_DEBUG
		printf("HalfSampleMode - j: %d N: %d medianPosition: %d.\n", j, slidingWindows[i], medianPosition);
		#endif	

	}


	#ifdef HSM_WINDOW_DEBUG	
	for(int i = start; i < end; i++){
		printf(";%ld", fx(array, i));
	}

	printf("\n");

	for(int i = start; i < end; i++){
		if(i==stepStart || i==stepEnd || i==medianPosition){
			printf(";%ld", fx(array, i));		
		} else {
			printf(";");		
		}
	}

	printf("\n");
	#endif
	
	
	modeWindow->start=stepStart;
	modeWindow->end=stepEnd;
}
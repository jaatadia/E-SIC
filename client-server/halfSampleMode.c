#include "halfSampleMode.h"

#include <stdio.h>
#include <inttypes.h>

//#define HSM_DEBUG

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


	int j = -1;
	int N = (size % 2 == 0) ? (size/2) : (size/2 + 1);
	int64_t wmin = (*fx)(array, end-1) - (*fx)(array, start);

	for(int i = start; i < end - N; i++) {
		int64_t w = (*fx)(array, start + (N-1)) - (*fx)(array, i);
		if(w <= wmin) {
			wmin = w;
			j = i;
		}
	}

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
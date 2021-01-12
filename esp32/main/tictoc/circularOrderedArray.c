#include "circularOrderedArray.h"
#include <stdio.h>
#include <inttypes.h>

//#define TICTOC_CIRCULAR_ARRAY_DEBUG

CircularOrderedArray * initCircularOrderedArray(int maxSize, size_t dataSize, void (*cpy)(void*, void*), int (*cmp)(void*, void*)){
	CircularOrderedArray* array = malloc(sizeof(CircularOrderedArray));
	array->next = 0;
	array->size = 0;

	array->maxSize = maxSize;
	array->cpy = cpy;
	array->cmp = cmp;
	array->order = malloc(sizeof(int)*maxSize);
	array->data = malloc(sizeof(void*)*maxSize);

	for(int i = 0; i < maxSize; i ++) {
		array->data[i]=malloc(dataSize);
	}

	return array;
}

void resetCircularOrderedArray(CircularOrderedArray* array){
	array->next = 0;
	array->size = 0;
}


void freeCircularOrderedArray(CircularOrderedArray* array){
	for(int i = 0; i < array->maxSize; i ++) {
		free(array->data[i]);
	}
	free(array->data);
	free(array->order);
	free(array);
}

void swap(CircularOrderedArray * array, int first, int second){
	// aux = first;
	int orderAux = array->order[first];
	void* dataAux = array->data[first];

	// first = second;
	array->order[first] = array->order[second];
	array->data[first] = array->data[second];

	// second = aux;
	array->order[second] = orderAux;
	array->data[second] = dataAux;
}

int orderRight(CircularOrderedArray * array, int position){
	int nextPosition = position;
	while((nextPosition + 1 < array->size) && (*array->cmp)(array->data[nextPosition], array->data[nextPosition+1]) > 0){ //hasNext && bigger than next
		swap(array, nextPosition, nextPosition+1);
		nextPosition++;
	}
	return position!=nextPosition;
}

void orderLeft(CircularOrderedArray * array, int position){
	while(position > 0 && (*array->cmp)(array->data[position], array->data[position-1]) < 0){ //hasPrevious && smaller than previous
		swap(array, position, position-1);
		position--;
	}
}

void orderPosition(CircularOrderedArray * array, int position){
	if(!orderRight(array, position)){
		orderLeft(array, position);
	}	
}

int findPosition(int* array, int size, int order) {
	for(int i = 0; i < size; i++){
		if(array[i] == order) return i;
	}

	//should never arrive here
	return -1;
}

/*
void printDebugInfo(CircularOrderedArray* array){
	printf("CircularOrderedArray - Array Conent: \n");	
	printf("CircularOrderedArray - [");	
	for (int i = 0; i < array->size; i++){
		printf("{\"value\":\"%"PRId64"\", \"order\":\"%d\"}" , array->array[i].value, array->array[i].order);	
		if(i != array->size - 1) printf(", ");	
	}
	printf("].\n");	
	printf("CircularOrderedArray - Current Median: %"PRId64".\n", median(array));
	printf("CircularOrderedArray - Next Order to override: %d.\n", array->next);
}
*/

void insertOrdered(CircularOrderedArray* array, void* node){
	int insertPosition;
	if(array->size < array->maxSize){
		insertPosition = array->size;
		array->size++;
	} else {
		insertPosition = findPosition(array->order, array->maxSize, array->next) ;
	}

	(*array->cpy)(node, array->data[insertPosition]);
	array->order[insertPosition] = array->next;
	array->next = (array->next + 1) % array->maxSize;

	orderPosition(array, insertPosition);

	#ifdef TICTOC_CIRCULAR_ARRAY_DEBUG
	printDebugInfo(array);
	#endif
}



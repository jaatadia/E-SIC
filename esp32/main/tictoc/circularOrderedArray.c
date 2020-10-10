#include "circularOrderedArray.h"
#include <stdio.h>
#include <inttypes.h>

//#define TICTOC_CIRCULAR_ARRAY_DEBUG

void initCircularOrderedArray(CircularOrderedArray * array){
	array->next = 0;
	array->size = 0;
}

void swap(Node * first, Node * second){
	Node aux;
	
	//aux = first
	aux.value = first->value;
	aux.order = first->order;
	 
	//first = second 
	first->value = second->value;
	first->order = second->order;

	//second = aux
	second->value = aux.value;
	second->order = aux.order;
}

int orderRight(Node * array, int position, int size){
	int nextPosition = position;
	while((nextPosition + 1 < size) && array[nextPosition].value > array[nextPosition+1].value){ //hasNext && bigger than next
		swap(&array[nextPosition], &array[nextPosition+1]);
		nextPosition++;
	}
	return position!=nextPosition;
}

void orderLeft(Node * array, int position){
	while(position > 0 && array[position].value < array[position-1].value){ //hasPrevious && smaller than previous
		swap(&array[position], &array[position-1]);
		position--;
	}
}

void orderPosition(Node * array, int position, int size){
	if(!orderRight(array, position, size)){
		orderLeft(array, position);
	}	
}

int findPosition(Node* array, int size, int order) {
	for(int i = 0; i < size; i++){
		if(array[i].order == order) return i;
	}

	//should never arrive here
	return -1;
}

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

void insertOrdered(CircularOrderedArray* array, int64_t value){
	int insertPosition;
	if(array->size < CIRCULAR_ORDERED_ARRAY_MAX_SIZE){
		insertPosition = array->size;
		array->size++;
	} else {
		insertPosition = findPosition(array->array, CIRCULAR_ORDERED_ARRAY_MAX_SIZE, array->next) ;
	}

	array->array[insertPosition].value=value;
	array->array[insertPosition].order=array->next;
	array->next = (array->next + 1) % CIRCULAR_ORDERED_ARRAY_MAX_SIZE;

	orderPosition(array->array, insertPosition, array->size);

	#ifdef TICTOC_CIRCULAR_ARRAY_DEBUG
	printDebugInfo(array);
	#endif
}

int64_t median(CircularOrderedArray* array){
	return array->array[array->size/2].value;
}

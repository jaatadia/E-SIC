#include "circularOrderedArray.h"

void orderPosition(double * array, int position, int size){
	if(!orderRight(array, position, size)){
		orderLeft(array, position, size);
	}	
}

int orderRight(double * array, double * position, int size){
	if((position + 1 < size) && array[position] > array[position+1]){ //hasNext && bigger than next
		swap(&array[position], &array[position+1]);
		orderRight(array, position+1, size);
		return 1;
	}
	return 0;
}

void orderLeft(double * array, double * position){
	if(position > 0 && array[position] < array[position-1]){ //hasPrevious && smaller than previous
		swap(&array[position], &array[position-1]);
		orderLeft(array, position-1);
	}
}

void swap(double * first, double * second){
	double aux = *first;
	*first=*second;
	*second=aux;
}

void insertOrdered(CircularOrderedArray* array, double value){
	int insertPosition;
	if(array->size < CIRCULAR_ORDERED_ARRAY_MAX_SIZE){
		insertPosition = array->size;
		array->fifo[array->size]=value;
		array->size++;
	} else {
		double valueToRemove = array->fifo[array->next];
		for(int i = 0; i< CIRCULAR_ORDERED_ARRAY_MAX_SIZE; i++){
			if(array->array[i] == valueToRemove){
				insertPosition = i;
				break;
			}
		}
		array->fifo[next]=value;
		array->next = array->next + 1 % CIRCULAR_ORDERED_ARRAY_MAX_SIZE;
	}
	
	array->array[insertPosition] = value;
	orderPosition(array->array, insertPosition, array->size);
}

double median(CircularOrderedArray* array){
	return array->array[array->size/2];
}


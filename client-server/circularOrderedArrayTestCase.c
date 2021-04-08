#include "circularOrderedArray.h"
#include <stdio.h>
#include "cunit.h"

int64_t f_value(void * array, int position){
	return *((int*)((CircularOrderedArray*) array)->data[position]);
}

int64_t f_order(void * array, int position){
	return ((CircularOrderedArray*) array)->order[position];
}

void cpy(void * source, void * target){
	*((int*)target) = *((int*)source);
}

double cmp(void * first, void * second){
	return *((int*)first) - *((int*)second);
}

void correctInsertStarting() {
	printf("---- correctInsertStarting ----\n");	
	
	CircularOrderedArray* array = initCircularOrderedArray(10, sizeof(int), cpy, cmp);

	assert("0. size", array->size, 0);
	assert("0. nextPos", array->next, 0);
	
	int node=2;
	insertOrdered(array, &node);
	int64_t value[] = {2};
	int64_t order[] = {0};
	assert("1. size", array->size, 1);
	assert("1. nextPos", array->next, 1);	
	assertPositions("1. value", array, f_value, value, 1);
	assertPositions("1. order", array, f_order, order, 1);
	

	node=4;
	insertOrdered(array, &node);
	int64_t value2[] = {2, 4};
	int64_t order2[] = {0, 1};
	assert("2. size", array->size, 2);
	assert("2. nextPos", array->next, 2);	
	assertPositions("2. value", array, f_value, value2, 2);
	assertPositions("2. order", array, f_order, order2, 2);
	
	node=3;
	insertOrdered(array, &node);
	int64_t value3[] = {2, 3, 4};
	int64_t order3[] = {0, 2, 1};
	assert("3. size", array->size, 3);
	assert("3. nextPos", array->next, 3);	
	assertPositions("3. value", array, f_value, value3, 3);
	assertPositions("3. order", array, f_order, order3, 3);
	
	node=0;
	insertOrdered(array, &node);
	int64_t value4[] = {0, 2, 3, 4};
	int64_t order4[] = {3, 0, 2, 1};
	assert("4. size", array->size, 4);
	assert("4. nextPos", array->next, 4);	
	assertPositions("4. value", array, f_value, value4, 4);
	assertPositions("4. order", array, f_order, order4, 4);
	
	freeCircularOrderedArray(array);
}


void correctInsertOnceFull() {
	printf("---- correctInsertOnceFull ----\n");	
	
	CircularOrderedArray* array = initCircularOrderedArray(10, sizeof(int), cpy, cmp);

	for(int i = 0; i< 10; i++){
		insertOrdered(array, &i);
	}
	
	int64_t value_0[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int64_t order_0[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	assert("_0. size", array->size, 10);
	assert("_0. nextPos", array->next, 0);
	assertPositions("_0. value", array, f_value, value_0, 10);
	assertPositions("_0. order", array, f_order, order_0, 10);

	int node=2;
	insertOrdered(array, &node);
	
	int64_t value_1[] = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9};
	int64_t order_1[] = {1, 0, 2, 3, 4, 5, 6, 7, 8, 9};
	assert("_1. size", array->size, 10);
	assert("_1. nextPos", array->next, 1);
	assertPositions("_1. value", array, f_value, value_1, 10);
	assertPositions("_1. order", array, f_order, order_1, 10);

	node=8;
	insertOrdered(array, &node);
	
	int64_t value_2[] = {2, 2, 3, 4, 5, 6, 7, 8, 8, 9};
	int64_t order_2[] = {0, 2, 3, 4, 5, 6, 7, 1, 8, 9};
	assert("_2. size", array->size, 10);
	assert("_2. nextPos", array->next, 2);
	assertPositions("_2. value", array, f_value, value_2, 10);
	assertPositions("_2. order", array, f_order, order_2, 10);

	node=-1;
	insertOrdered(array, &node);
	node=1;
	insertOrdered(array, &node);
	node=0;
	insertOrdered(array, &node);
	
	int64_t value_3[] = {-1, 0, 1, 2, 5, 6, 7, 8, 8, 9};
	int64_t order_3[] = {2, 4, 3, 0, 5, 6, 7, 1, 8, 9};

	assert("_3. size", array->size, 10);
	assert("_3. nextPos", array->next, 5);
	assertPositions("_3. value", array, f_value, value_3, 10);
	assertPositions("_3. order", array, f_order, order_3, 10);

	freeCircularOrderedArray(array);
}


int main(int argc, char** argv){
	correctInsertStarting();
	correctInsertOnceFull();
	
	return reportResults();

}
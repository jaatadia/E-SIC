#include "circularOrderedArray.h"
#include <stdio.h>
#include "cunit.h"


int64_t f_value(void * node, int position){
	return ((Node*) node)[position].value;
}

int64_t f_order(void * node, int position){
	return ((Node*) node)[position].order;
}

void correctInsertStarting() {
	printf("---- correctInsertStarting ----\n");	

	CircularOrderedArray array;
	initCircularOrderedArray(&array);

	
	assert("0. size", array.size, 0);
	assert("0. nextPos", array.next, 0);

	insertOrdered(&array, 2);
	int64_t value[] = {2};
	int64_t order[] = {0};
	assert("1. size", array.size, 1);
	assert("1. nextPos", array.next, 1);	
	assertPositions("1. value", array.array, f_value, value, 1);
	assertPositions("1. order", array.array, f_order, order, 1);
	assert("1. median", median(&array), 2);

	insertOrdered(&array, 4);
	int64_t value2[] = {2, 4};
	int64_t order2[] = {0, 1};
	assert("2. size", array.size, 2);
	assert("2. nextPos", array.next, 2);	
	assertPositions("2. value", array.array, f_value, value2, 2);
	assertPositions("2. order", array.array, f_order, order2, 2);
	assert("2. median", median(&array), 4);

	insertOrdered(&array, 3);
	int64_t value3[] = {2, 3, 4};
	int64_t order3[] = {0, 2, 1};
	assert("3. size", array.size, 3);
	assert("3. nextPos", array.next, 3);	
	assertPositions("3. value", array.array, f_value, value3, 3);
	assertPositions("3. order", array.array, f_order, order3, 3);
	assert("3. median", median(&array), 3);

	insertOrdered(&array, 0);
	int64_t value4[] = {0, 2, 3, 4};
	int64_t order4[] = {3, 0, 2, 1};
	assert("4. size", array.size, 4);
	assert("4. nextPos", array.next, 4);	
	assertPositions("4. value", array.array, f_value, value4, 4);
	assertPositions("4. order", array.array, f_order, order4, 4);
	assert("4. median", median(&array), 3);
}


void correctInsertOnceFull() {
	printf("---- correctInsertOnceFull ----\n");	
	CircularOrderedArray array;
	initCircularOrderedArray(&array);
	for(int i = 0; i< CIRCULAR_ORDERED_ARRAY_MAX_SIZE; i++){
		insertOrdered(&array, i);
	}
	
	
	int64_t value_0[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int64_t order_0[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	assert("_0. size", array.size, 10);
	assert("_0. nextPos", array.next, 0);
	assertPositions("_0. value", array.array, f_value, value_0, 10);
	assertPositions("_0. order", array.array, f_order, order_0, 10);
	assert("_0. median", median(&array), 5);

	insertOrdered(&array, 2);
	
	int64_t value_1[] = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9};
	int64_t order_1[] = {1, 0, 2, 3, 4, 5, 6, 7, 8, 9};
	assert("_1. size", array.size, 10);
	assert("_1. nextPos", array.next, 1);
	assertPositions("_1. value", array.array, f_value, value_1, 10);
	assertPositions("_1. order", array.array, f_order, order_1, 10);
	assert("_1. median", median(&array), 5);

	insertOrdered(&array, 8);
	
	int64_t value_2[] = {2, 2, 3, 4, 5, 6, 7, 8, 8, 9};
	int64_t order_2[] = {0, 2, 3, 4, 5, 6, 7, 1, 8, 9};
	assert("_2. size", array.size, 10);
	assert("_2. nextPos", array.next, 2);
	assertPositions("_2. value", array.array, f_value, value_2, 10);
	assertPositions("_2. order", array.array, f_order, order_2, 10);
	assert("_2. median", median(&array), 6);

	insertOrdered(&array, -1);
	insertOrdered(&array, 1);
	insertOrdered(&array, 0);
	
	int64_t value_3[] = {-1, 0, 1, 2, 5, 6, 7, 8, 8, 9};
	int64_t order_3[] = {2, 4, 3, 0, 5, 6, 7, 1, 8, 9};

	assert("_3. size", array.size, 10);
	assert("_3. nextPos", array.next, 5);
	assertPositions("_3. value", array.array, f_value, value_3, 10);
	assertPositions("_3. order", array.array, f_order, order_3, 10);
	assert("_3. median", median(&array), 6);
}


int main(int argc, char** argv){
	if (CIRCULAR_ORDERED_ARRAY_MAX_SIZE != 10){
		printf("Compile this test with -DCIRCULAR_ORDERED_ARRAY_MAX_SIZE=10");	
		return 1;
	}
	
	correctInsertStarting();
	correctInsertOnceFull();
	
	return reportResults();

}
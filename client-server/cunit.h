#ifndef CUNIT_H
#define CUNIT_H

#include <stdint.h>

void assert(char* testName, int actual, int expected);
void assertPositions(char* testName, void * arrayActual, int64_t(*fn)(void*, int), int64_t * arrayExpected, int size);
void assertInMargin(char* testName, int64_t actual, int64_t expected, int margin);
int reportResults();

#endif
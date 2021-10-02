#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include <stdlib.h>

struct Node { 
	int64_t cmp; // This value will be used to order the array
	int64_t phi; 
    int64_t time;
};

typedef struct Node Node;

// function to copy a Node from source to target.
void cpyNode(void * source, void * target);

// function to order two Node
double cmpNode(void * first, void * second);

#endif
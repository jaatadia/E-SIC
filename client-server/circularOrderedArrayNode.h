#ifndef CIRCULAR_ORDERED_ARRAY_NODE_H
#define CIRCULAR_ORDERED_ARRAY_NODE_H

#include <stdint.h>

struct Node {
	// Ordering Info
	int64_t value;
	int order;
	
	// Extra data
	int64_t time;
};

typedef struct Node Node;

void copyNode(Node* source, Node* target);

#endif
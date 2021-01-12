#include "circularOrderedArrayNode.h"

void copyNode(Node* source, Node* target){
	// Ordering Info
	target->value = source->value;
	target->order = source->order;

	// Extra data
	target->time = source->time;
}
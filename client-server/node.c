#include "node.h"


// function to copy a Node from source to target.
void cpyNode(void * source, void * target){
	Node* sourceNode = (Node*) source;
	Node* targetNode = (Node*) target;

	targetNode->cmp = sourceNode->cmp;
	targetNode->phi = sourceNode->phi;
	targetNode->time = sourceNode->time;
}

// function to order two Node
double cmpNode(void * first, void * second){
	Node* firstNode = (Node*) first;
	Node* secondNode = (Node*) second;
	return (double)firstNode->cmp - (double)secondNode->cmp;
}
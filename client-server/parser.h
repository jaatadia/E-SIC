#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>


#define TIC_TOC_LINE 0
#define INTERRUPTION_LINE 1
#define TIMEOUT_LINE 2

struct ParsedT {
	int type;
	int64_t t[4];
};

typedef struct ParsedT ParsedT;




void loadServerValues(char* file, int64_t* interruptions, int64_t* size);
void loadClientValues(char* file, ParsedT* parsed, int64_t* size);


void freeResources();

#endif
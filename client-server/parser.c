#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "limits.h"

#include <regex.h> 
#include "parser.h"

int regInit = 0; 

regex_t reg; 
int ngroups;
regmatch_t *groups;

regex_t ireg; 
int ingroups;
regmatch_t *igroups;

regex_t toreg; 

regex_t sreg; 
int sngroups;
regmatch_t *sgroups;

void initRegex(){
	if (regInit==1) return;

	regInit = 1; 

	if(regcomp(&reg, "^.*t1:([0-9]+) t2:([0-9]+) t3:([0-9]+) t4:([0-9]+).*$", REG_EXTENDED)!=0) exit(-10); 
	ngroups = reg.re_nsub + 1;
	groups = malloc(ngroups * sizeof(regmatch_t));
	
	if(regcomp(&ireg, "^.*timeRequested:([0-9]+) tictocTime:([0-9]+).*$", REG_EXTENDED)!=0) exit(-11); 
	ingroups = ireg.re_nsub + 1;
	igroups = malloc(ngroups * sizeof(regmatch_t));

	if(regcomp(&toreg, "^.*TicTocDaemon timeout.*$", 0)!=0) exit(-12); 

	if(regcomp(&sreg, "^.*timeRequested: ([0-9]+).*$", REG_EXTENDED)!=0) exit(-11); 
	sngroups = sreg.re_nsub + 1;
	sgroups = malloc(sngroups * sizeof(regmatch_t));	
}

void freeResources(){
	if(regInit == 1){
		regfree(&reg); 
		int ngroups;
		free(groups);

		regfree(&ireg); 
		free(igroups);

		regfree(&toreg); 

		regfree(&sreg); 
		free(sgroups);
	}
}

int64_t parseNumber(char* string, int start, int end){
	int64_t result = 0;
	for(int j = start; j< end; j++) {
		result *= 10;
		result += string[j] - '0';
	}
	return result;
}

void parseLine(ParsedT * parsed, char* line){
	if(regexec(&reg, line, ngroups, groups, 0) == 0){
		parsed->type = TIC_TOC_LINE;
		for(int i = 0; i<4;i++){
			parsed->t[i] = parseNumber(line, groups[i+1].rm_so, groups[i+1].rm_eo);
		}
	} else if(regexec(&ireg, line, ingroups, igroups, 0) == 0){
		parsed->type = INTERRUPTION_LINE;
		parsed->t[0] = parseNumber(line, igroups[1].rm_so, igroups[1].rm_eo); // t0
		parsed->t[1] = parseNumber(line, igroups[2].rm_so, igroups[2].rm_eo); // sic(t0)
	} else if(regexec(&toreg, line, ingroups, igroups, 0) == 0){
		parsed->type = TIMEOUT_LINE;
	} else {
		printf("line not matched: %s\n", line);
		exit(-11);
	}
}
	
void parseServerLine(ParsedT * parsed, char* line) {
	if(regexec(&sreg, line, sngroups, sgroups, 0) == 0){
		parsed->type = INTERRUPTION_LINE;
		parsed->t[0] = parseNumber(line, sgroups[1].rm_so, sgroups[1].rm_eo);
	} else {
		parsed->type = 0;
	}
}

void loadServerValues(char* file, int64_t* interruptions, int64_t* size){
	initRegex();
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    (*size) = 0;

	ParsedT parsed;

    if((fp = fopen(file, "r")) == 0) exit(-2);
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("line: %s", line);
        parseServerLine(&parsed, line);
		if(parsed.type == INTERRUPTION_LINE){
			interruptions[(*size)] = parsed.t[0];
			(*size) ++;
		}

    }
	fclose(fp);
	free(line);
}

void loadClientValues(char* file, ParsedT* parsed, int64_t* size){
	initRegex();
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

	int64_t lastRealPhiFound = 0;
    int64_t lastRealPhi = 0;

    (*size) = 0;
    if((fp = fopen(file, "r")) == 0) exit(-2);

    while ((read = getline(&line, &len, fp)) != -1) {
        parseLine(&parsed[*size], line);
        (*size) ++;
    }
    printf("\n");
	fclose(fp);
	free(line);
}

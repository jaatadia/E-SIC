#include "sic.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "cunit.h"
#include "limits.h"

int seed = 1112;
int rtt = 0;

int randUpTo(int max) {
	return rand() % max;
}

int randSign() {
	return (rand() % 2 == 0) ? 1:-1;
}

void syncStatesTestCase() {
	SicData sic;
	sicInit(&sic);
	
	sicStep(&sic, -1*1000, -1*1000 + 10, -1*1000 + 11, -1*1000 + 21);
	assert("noSync", sic.state, NO_SYNC);
	assert("noSync steps", sic.syncSteps, 1);

	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);

	assert("reSync when no sync achieved", sic.state, NO_SYNC);
	assert("reSync steps when no sync achieved", sic.syncSteps, 0);

	for(int i=0; i< 720; i++){
		sicStep(&sic, i*1000, i*1000 + 10, i*1000 + 11, i*1000 + 21);
		if(i == 598){
			assert("Iteration 598 noSync", sic.state, NO_SYNC);
			assert("Iteration 598 steps", sic.syncSteps, 599);
		} else if(i == 599) {
			assert("Iteration 599 preSync", sic.state, NO_SYNC);
			assert("Iteration 599 steps", sic.syncSteps, 600);
		} else if(i == 658) {
			assert("Iteration 658 preSync", sic.state, NO_SYNC);
			assert("Iteration 658 steps", sic.syncSteps, 659);
		} else if(i == 659) {
			assert("Iteration 659 preSync", sic.state, PRE_SYNC);
			assert("Iteration 659 steps", sic.syncSteps, 0);
		} else if(i == 718) {
			assert("Iteration 718 preSync", sic.state, PRE_SYNC);
			assert("Iteration 718 steps", sic.syncSteps, 59);
		} else if(i == 719) {
			assert("Iteration 719 preSync", sic.state, SYNC);
			assert("Iteration 719 steps", sic.syncSteps, 0);
		}
	}

	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);
	sicStepTimeout(&sic);

	assert("reSync", sic.state, RE_SYNC);
	assert("reSync steps", sic.syncSteps, 0);

	sicEnd(&sic);
}

void syncServerDifFrequency() {
	SicData sicA;
	sicInit(&sicA);
	
	int64_t serverTime = 1602262903000000;
	int64_t serverDelay = 50;
	int64_t startTimeA = 1000;
	int64_t tAS = 2222;
	
	double f_multiplier = 1 + 0.01;

	for(int i=0; i< 600 + 61 * 60; i++){
		int64_t timeSinceStart = i*1000000;
		sicStep(&sicA, 
			startTimeA + f_multiplier * timeSinceStart , 
			timeSinceStart + tAS + serverTime, 
			timeSinceStart + tAS + serverTime + serverDelay,
			startTimeA + f_multiplier * (timeSinceStart + tAS + tAS + serverDelay));
	}

	int64_t timeSinceStart = 800*1000000;
	int64_t tS = timeSinceStart + serverTime;
	int64_t tS_A = sicTime(&sicA, f_multiplier * timeSinceStart + startTimeA);
	
	assertInMargin("syncServerDifFrequency", tS_A, tS, 100);
	sicEnd(&sicA);
}


void parallel() {
	SicData sicA;
	SicData sicB;
	sicInit(&sicA);
	sicInit(&sicB);
	
	int64_t serverTime = 1602262903000000;
	int64_t serverDelay = 50;
	int64_t startTimeA = 1000;
	int64_t startTimeB = 2222;
	int64_t tAS = 2222;
	int64_t tBS = 5333;
	
	for(long long i=0; i< 60000; i++){
		int64_t timeSinceStart = i*1000000;
		sicStep(&sicA, 
			timeSinceStart + startTimeA, 
			timeSinceStart + tAS + serverTime, 
			timeSinceStart + tAS + serverTime + serverDelay,
			timeSinceStart + startTimeA + tAS + tAS + serverDelay);

		sicStep(&sicB, 
			timeSinceStart + startTimeB, 
			timeSinceStart + tBS + serverTime, 
			timeSinceStart + tBS + serverTime + serverDelay,
			timeSinceStart + startTimeB + tBS + tBS + serverDelay);
	}

	int64_t timeSinceStart = 800*1000000;
	int64_t tS = timeSinceStart + serverTime;
	int64_t tS_A = sicTime(&sicA, timeSinceStart + startTimeA);
	int64_t tS_B = sicTime(&sicB, timeSinceStart + startTimeB);
	
	printf("Parallel:\n");
	printf("time ServerA: %ld.\n", tS);
	printf("Server Time Acording to NodeA: %ld. Diff to RealServer: %ld.\n", tS_A, tS - tS_A);
	printf("Server Time Acording to NodeB: %ld. Diff to RealServer: %ld.\n", tS_B, tS - tS_B);
	
	assertInMargin("Parallel: server time A", tS_A, tS, 100);
	assertInMargin("Parallel: server time B", tS_B, tS, 100);

	sicEnd(&sicA);
	sicEnd(&sicB);
}

void parallelSimulatedVariations() {
	SicData sicA;
	SicData sicB;
	sicInit(&sicA);
	sicInit(&sicB);
	
	int maxVariation = 1000;
	int64_t serverTime = 1602262903000000;
	int64_t serverDelay = 50;
	int64_t startTimeA = 1000;
	int64_t startTimeB = 2222;
	int64_t tAS = 2222;
	int64_t tBS = 5333;
	
	for(long long i=0; i< 60000; i++){

		int64_t timeSinceStart = i*1000000 + randSign() * randUpTo(200000);

		int64_t timeSinceStartA = timeSinceStart + randSign() * randUpTo(maxVariation);
		int64_t vtAS = tAS + randSign() * randUpTo(maxVariation);
		int64_t vtSA = tAS + randSign() * randUpTo(maxVariation);
		int64_t vserverDelayA = serverDelay;

		int64_t timeSinceStartB = timeSinceStart + randSign() * randUpTo(maxVariation);
		int64_t vtBS = tBS + randSign() * randUpTo(maxVariation);
		int64_t vtSB = tBS + randSign() * randUpTo(maxVariation);
		int64_t vserverDelayB = serverDelay;

		sicStep(&sicA, 
			timeSinceStartA + startTimeA, 
			timeSinceStartA + vtAS + serverTime, 
			timeSinceStartA + vtAS + serverTime + vserverDelayA,
			timeSinceStartA + startTimeA + vtAS + vtSA + vserverDelayA);

		sicStep(&sicB, 
			timeSinceStartB + startTimeB, 
			timeSinceStartB + vtBS + serverTime, 
			timeSinceStartB + vtBS + serverTime + vserverDelayB,
			timeSinceStartB + startTimeB + vtBS + vtSB + vserverDelayB);
	}

	int64_t timeSinceStart = 800*1000000;
	int64_t tS = timeSinceStart + serverTime;
	int64_t tS_A = sicTime(&sicA, timeSinceStart + startTimeA);
	int64_t tS_B = sicTime(&sicB, timeSinceStart + startTimeB);
	
	printf("Parallel Variations:\n");
	printf("time ServerA: %ld.\n", tS);
	printf("Server Time Acording to NodeA: %ld. Diff to RealServer: %ld.\n", tS_A, tS - tS_A);
	printf("Server Time Acording to NodeB: %ld. Diff to RealServer: %ld.\n", tS_B, tS - tS_B);
	
	assertInMargin("Parallel Variations: server time A", tS_A, tS, 100);
	assertInMargin("Parallel Variations: server time B", tS_B, tS, 100);

	sicEnd(&sicA);
	sicEnd(&sicB);
}


void syncNoDifferenceInClocks() {
	SicData sic;
	sicInit(&sic);
	
	for(long long i=0; i< 60000; i++){
		sicStep(&sic, i*1000000, i*1000000 + 10, i*1000000 + 11, i*1000000 + 21);
	}

	assert("Synced algorithm no difference in clocks actual_m", sic.actual_m, 0);
	assert("Synced algorithm no difference in clocks actual_c", sic.actual_c, 0);
	assert("Synced algorithm no difference in clocks", sicTime(&sic, 1000), 1000);

	sicEnd(&sic);
}

void syncServerInFuture() {
	SicData sic;
	sicInit(&sic);
	
	for(long long i=0; i< 60000; i++){
		sicStep(&sic, i*1000000, (i+1)*1000000 + 10, (i+1)*1000000 + 11, i*1000000 + 21);
	}

	assert("Synced algorithm server in future actual_m", sic.actual_m, 0);
	assert("Synced algorithm server in future actual_c", sic.actual_c, -1000000);
	assert("Synced algorithm server in future", sicTime(&sic, 1000), 1001000);

	sicEnd(&sic);
}

void syncServerInPast() {
	SicData sic;
	sicInit(&sic);
	
	for(long long i=0; i< 60000; i++){
		sicStep(&sic, i*1000000, (i-1)*1000000 + 10, (i-1)*1000000 + 11, i*1000000 + 21);
	}

	assert("Synced algorithm server in past actual_m", sic.actual_m, 0);
	assert("Synced algorithm server in past actual_c", sic.actual_c, +1000000);
	assert("Synced algorithm server in past", sicTime(&sic, 1000000), 0);

	sicEnd(&sic);
}

/** input file parsing **/
struct ParsedT {
	int type;
	int64_t t[4];
};

typedef struct ParsedT ParsedT;

#include <regex.h> 
int TIC_TOC_LINE = 0;
int INTERRUPTION_LINE = 1;
int TIMEOUT_LINE = 2;

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
		parsed->t[0] = parseNumber(line, igroups[1].rm_so, igroups[1].rm_eo);
		parsed->t[1] = parseNumber(line, igroups[2].rm_so, igroups[2].rm_eo);
	} else if(regexec(&toreg, line, ingroups, igroups, 0) == 0){
		parsed->type = TIMEOUT_LINE;
	} else {
		printf("line not matched: %s\n", line);
		exit(-11);
	}
}

/** en input file parsing **/
	
void loadValues(SicData* sic, char* file, int64_t* t0, int64_t* estimations, int64_t* size, int64_t* serverInterruptions){
	initRegex();
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

	int64_t lastRealPhiFound = 0;
    int64_t lastRealPhi = 0;

    (*size) = 0;

	ParsedT parsed;

    if((fp = fopen(file, "r")) == 0) exit(-2);

    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("line: %s", line);
        parseLine(&parsed, line);
		
		if(parsed.type == TIC_TOC_LINE){
			//printf("t1:%ld t2:%ld t3:%ld t4:%ld\n", parsed.t[0], parsed.t[1], parsed.t[2], parsed.t[3]);
			sicStep(sic, parsed.t[0], parsed.t[1], parsed.t[2], parsed.t[3]);	
			//printf(", %ld", parsed.t[3] - parsed.t[0]);
			//printf("RTT %d: %ld.\n", rtt, parsed.t[3] - parsed.t[0]);
			if(lastRealPhiFound){
				int64_t rtt = parsed.t[3] - parsed.t[0];
				int64_t tcs = lastRealPhi - parsed.t[0] + parsed.t[1];
				int64_t tsc = - lastRealPhi - parsed.t[2] + parsed.t[3];

				int64_t phi = (parsed.t[0] - parsed.t[1] - parsed.t[2] + parsed.t[3])/2;
				int64_t phiDiff = phi - lastRealPhi;
				double n = tcs/(double)tsc;
				//printf("rtt = %ld, tcs = %ld, tsc = %ld, phi = %ld, diffToRealPhi = %ld, n=%f\n", rtt, tcs, tsc, phi, phiDiff, n);
				printf("%f, ", n);
			}
			rtt++;
		} else if(parsed.type == INTERRUPTION_LINE){
			//printf("tt_input:%ld tt:%ld \n", parsed.t[0], parsed.t[1]);
			if(parsed.t[1] != 0) {
				//printf("Iteration %ld - ", (*size));
				//assertInMargin("training assertion", parsed.t[1], sicTime(sic, parsed.t[0]), 100);
				t0[(*size)] = parsed.t[0];
				
				estimations[(*size)] = parsed.t[1];
				//estimations[(*size)] = sicTime(sic, parsed.t[0]);
				
				lastRealPhi = parsed.t[0] - serverInterruptions[(*size)];
				lastRealPhiFound = 1;
				(*size) ++;
				//printf("-----\n");
			}
		} else if(parsed.type == TIMEOUT_LINE){
			sicStepTimeout(sic);
		}

    }
    printf("\n");
	fclose(fp);
	free(line);
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

void printArray(FILE * f, char* name, int64_t* values, int64_t size){
	fprintf(f, "%s = [", name);
	fprintf(f, "%ld", values[0]);
	for(int i = 1; i<size; i++) {
		fprintf(f, ", %ld", values[i]);
	}
	fprintf(f, "]\n");
}


void fileTest(){
	SicData sicA;
	SicData sicB;
	sicInit(&sicA);
	sicInit(&sicB);

	int64_t sizeServerT;
	int64_t serverT[50000];

	int64_t sizeEstimationsNodeA;
	int64_t estimationsNodeA[50000];
	int64_t t0A[50000];

	int64_t sizeEstimationsNodeB;
	int64_t estimationsNodeB[50000];
	int64_t t0B[50000];


	printf("\n---------loading1---------.\n");
	loadServerValues("./samples/04_29/ESP_SERVER.txt", serverT, &sizeServerT);
	int64_t size = sizeServerT;
	
	loadValues(&sicA, "./samples/04_29/ESP_CLIENT.txt", t0A, estimationsNodeA, &sizeEstimationsNodeA, serverT);
	if(sizeEstimationsNodeA < size) size = sizeEstimationsNodeA;
	
	//loadValues(&sicB, "./samples/ESP3.txt", t0B, estimationsNodeB, &sizeEstimationsNodeB);
	//if(sizeEstimationsNodeB < size) size = sizeEstimationsNodeB;

	
	int64_t maxDif = 0;
	int64_t minDif = 10000000;

	int starting = 0;
	for(int i = starting; i<size; i++) {
		printf("Iteration %d - ", i);
		assertInMargin("fileTest: timeServer A B ", estimationsNodeA[i], serverT[i], 100);	
		int64_t dif = estimationsNodeA[i] - serverT[i];
		dif = (dif < 0) ? - dif : dif;
		if(dif > maxDif) maxDif = dif;
		if(dif < minDif) minDif = dif;
	}

	printf("# samples: %ld\n", sizeEstimationsNodeA);
	printf("MinDif: %ld\n", minDif);
	printf("MaxDif: %ld\n", maxDif);



	FILE* f = fopen("values.py", "w");

	printArray(f, "serverTime", serverT, size);
	printArray(f, "t0A", t0A, size);
	printArray(f, "estimationA", estimationsNodeA, size);
	//printArray(f, "t0B", t0B, size);
	//printArray(f, "estimationB", estimationsNodeB, size);

	fclose(f);

	sicEnd(&sicA);
	sicEnd(&sicB);
}



int main(int argc, char** argv){
	srand(seed);

	/*
	//syncStatesTestCase();
	syncNoDifferenceInClocks();
	syncServerInFuture();
	syncServerInPast();
	parallel();
	//parallelSimulatedVariations();
	syncServerDifFrequency();
	*/
	fileTest();

	freeResources();

	return reportResults();
}
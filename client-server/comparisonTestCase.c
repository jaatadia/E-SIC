#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "limits.h"

#include "cunit.h"
#include "parser.h"
#include "sic.h"
#include "esic.h"
#include "node.h"
#include "halfSampleMode.h"

int64_t absl(int64_t value){
	return value < 0 ? -value : value;
}

int64_t min(int64_t first, int64_t second){
	return first < second ? first : second;
}

int64_t max(int64_t first, int64_t second){
	return first > second ? first : second;
}


void printValue(FILE * f, int64_t value) {
	fprintf(f, "%ld,", value);
}


void startArray(FILE * f){
	fprintf(f, "[");
}

void startNamedArray(FILE * f, char * name){
	fprintf(f, "%s = ", name);
	startArray(f);
}


void endArray(FILE * f){
	fprintf(f, "],\n");
}

void printArray(FILE * f, CircularOrderedArray* array){
	void iter(void * value){
		fprintf(f, "%ld,", ((Node*) value)->phi);
	}

	startArray(f);
	foreach(array, iter);
	endArray(f);
}

int64_t test_getCmp(void * array, int pos){
	return ((Node*)((CircularOrderedArray*) array)->data[pos])->cmp;
}

void printMode(FILE * f, char* name , CircularOrderedArray* array){
	HalfSampleModeResult hsmResult;
	halfSampleModeWindow(array, 0, array->size, test_getCmp, esic_MODE_WINDOW, &hsmResult);
	fprintf(f, "%d\n", hsmResult.position1);	
	fprintf(f, "%d\n", hsmResult.position2);	
}

int main(int argc, char** argv){

	FILE* ftc = fopen("samples_tc.py", "w");
	FILE* fts = fopen("samples_ts.py", "w");
	FILE* fr = fopen("samples_phi.py", "w");
	FILE* fs = fopen("samples_tsic.py", "w");
	FILE* fe = fopen("samples_tesic.py", "w");
	startNamedArray(ftc, "tc");
	startNamedArray(fts, "ts");
	startNamedArray(fr, "realPhi");
	startNamedArray(fs, "sicPhi");
	startNamedArray(fe, "esicPhi");

	int64_t sizeServer;
	int64_t sizeClient;

	int64_t serverLines[50000];
	ParsedT clientLines[50000];

	loadServerValues("./samples/04_29/ESP_SERVER.txt", serverLines, &sizeServer);
	loadClientValues("./samples/04_29/ESP_CLIENT.txt", clientLines, &sizeClient);

	SicData sic;
	ESicData esic;
	sicInit(&sic);
	esicInit(&esic);
	
	int64_t sicMinDif = 100000;
	int64_t sicMaxDif = 0;
	int64_t esicMinDif = 100000;
	int64_t esicMaxDif = 0;

	int64_t interruption = 0;
	for(int64_t i = 0; i<sizeClient; i++) {
		if(clientLines[i].type == TIC_TOC_LINE){
			sicStep(&sic, clientLines[i].t[0], clientLines[i].t[1], clientLines[i].t[2], clientLines[i].t[3]);
			esicStep(&esic, clientLines[i].t[0], clientLines[i].t[1], clientLines[i].t[2], clientLines[i].t[3]);
		} else if(clientLines[i].type == TIMEOUT_LINE){
			sicStepTimeout(&sic);
			esicStepTimeout(&esic);
		} else if(clientLines[i].type == INTERRUPTION_LINE && clientLines[i].t[1] != 0){
			int64_t ts = serverLines[interruption];
			int64_t tc = clientLines[i].t[0];
			int64_t sicT = sicTime(&sic, tc);
			int64_t esicT = esicTime(&esic, tc);

			sicMinDif = min(sicMinDif, absl(sicT-ts));
			sicMaxDif = max(sicMaxDif, absl(sicT-ts));
			esicMinDif = min(esicMinDif, absl(esicT-ts));
			esicMaxDif = max(esicMaxDif, absl(esicT-ts));

			assertInMargin("sic: ", sicT, ts, 2000);	
			assertInMargin("esic: ", esicT, ts, 2000);	

			printValue(ftc, tc);
			printValue(fts, ts);
			printValue(fr, tc-ts);
			printValue(fs, sicT);
			printValue(fe, esicT);

			interruption++;
		}
	}
	printf("sic: minDif: %ld maxDif: %ld.\n", sicMinDif, sicMaxDif);
	printf("esic: minDif: %ld maxDif: %ld.\n", esicMinDif, esicMaxDif);

	endArray(ftc);
	endArray(fts);
	endArray(fr);
	endArray(fs);
	endArray(fe);
	
	

	sicEnd(&sic);
	esicEnd(&esic);
	freeResources();
	fclose(ftc);
	fclose(fts);
	fclose(fr);
	fclose(fs);
	fclose(fe);
}
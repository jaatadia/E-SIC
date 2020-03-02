#include "sic.h"
#include <stdio.h>

void assert(char* testName, int condition) {
	printf("Test: %s. %s\n", testName,  condition ? "SUCCESS" : "FAIL");
}

void noDelayTestCase() {
	SicData sic;
	sicInit(&sic, 0);
	sicStep(&sic, 0, 0, 0, 0, 0);

	//assert("noDelay: sic.state", sic.state == NO_SYNC);
	printf("\n");
	assert("noDelay: sic.phiAccumulator", sic.phiAccumulator == 0);
    assert("noDelay: sic.medianNextPos", sic.medianNextPos == 0);
    assert("noDelay: sic.medianSize", sic.medianSize == 1);
    assert("noDelay: sic.rttNextPos", sic.rttNextPos == 0);
    assert("noDelay: sic.rttSize", sic.rttSize == 1);
    assert("noDelay: sic.rttFirst", sic.rttFirst == 0);
    assert("noDelay: sic.rttLast", sic.rttLast == 0);
    assert("noDelay: sic.Wm[0]", sic.Wm[0] == 0);
    assert("noDelay: sic.Wrtt[0]", sic.Wrtt[0] == 0);
    assert("noDelay: sic.Wmedian[0]", sic.Wmedian[0] == 0);
}

void constantDelayTestCase() {
	SicData sic;
	sicInit(&sic, 0);
	sicStep(&sic, 1, 1, 2, 3, 5);
	sicStep(&sic, 2, 1, 2, 3, 5);
	sicStep(&sic, 3, 1, 2, 3, 5);

	printf("\n");
	//assert("constantDelay: sic.state", sic.state == NO_SYNC);
	//assert("constantDelay: sic.phiAccumulator", sic.phiAccumulator == 0);
    assert("constantDelay: sic.medianNextPos", sic.medianNextPos == 0);
    assert("constantDelay: sic.medianSize", sic.medianSize == 3);
    assert("constantDelay: sic.rttNextPos", sic.rttNextPos == 0);
    assert("constantDelay: sic.rttSize", sic.rttSize == 3);
    assert("constantDelay: sic.rttFirst", sic.rttFirst == 4);
    assert("constantDelay: sic.rttLast", sic.rttLast == 4);
    
    for(int i = 0; i < 3; i++){
		printf("Position: %d.\n", i);
	    assert("constantDelay: sic.Wm[i]", sic.Wm[i] == 0.5);
	    assert("constantDelay: sic.Wrtt[i]", sic.Wrtt[i] == 4);
	    assert("constantDelay: sic.Wmedian[i]", sic.Wmedian[i] == 0.5);
	}
}


int main(int argc, char** argv){
	noDelayTestCase();
	constantDelayTestCase();

	return 0;
}
/*
 * tic-toc-sender.h
 *
 *  Created on: May 9, 2020
 *      Author: jaatadia@gmail.com
 */

#ifndef MAIN_TIC_TOC_H_
#define MAIN_TIC_TOC_H_
#include <stdint.h>

#define TIC_TOC_SUCCESS 0
#define TIC_TOC_TIMEOUT -1

struct ticTocSenderData {
	int sock;
};

typedef struct ticTocSenderData ticTocSenderData;

void setupTicToc(ticTocSenderData *);

/* *
 * returns:
 * 	0 everything all right
 * 	TIC_TOC_TIMEOUT in case of timeout
 */
int getTimeStamps(ticTocSenderData *, int64_t *);

#endif /* MAIN_TIC_TOC_H_ */

/*
 * main.c
 *
 *  Created on: May 9, 2020
 *      Author: jaatadia@gmail.com
 */
#include <stdio.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "wifi.h"
#include "tic-toc.h"
#include "microtime.h"

void app_main(void)
{
	connectToWiFi();

	int64_t ticTocBuffer[4];
	ticTocSenderData ticTocData;
	setupTicToc(&ticTocData);

    while(true){
    	vTaskDelay(1000 / portTICK_PERIOD_MS);

    	int status = getTimeStamps(&ticTocData, ticTocBuffer);
    	if(status < TIC_TOC_SUCCESS) {
    		printf("timeout connecting to tic-toc server.");
    	} else {
    		printf("--------\n");
			for(int i=0; i<4; i++){
				char timestamp[64];
				microsToTimestamp(ticTocBuffer[i], timestamp);
				printf("t%d: %lld(%s)\n", i+1, ticTocBuffer[i], timestamp);
			}
    	}
    }
}

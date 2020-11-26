/*
 * tic-toc-sender.c
 *
 *  Created on: May 9, 2020
 *      Author: jaatadia@gmail.com
 */
#include "daemon.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include "microtime.h"
#include "esp_timer.h"
#include <inttypes.h>

#define TICTOC_DAEMON_DEBUG

//#define DAEMON_SERVER

/*
//To use this enable `Component config->FreeRTOS->Enable FreeRTOS trace facility`
void printRemainingStack(const char* identifier){
	TaskStatus_t *pxTaskStatusArray;
	volatile UBaseType_t uxArraySize;
	unsigned long ulTotalRunTime;

	// Take a snapshot of the number of tasks in case it changes while this function is executing.
	uxArraySize = uxTaskGetNumberOfTasks();

	// Allocate a TaskStatus_t structure for each task.
	pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );

	// Generate raw status information about each task.
	uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );

	for(int i = 0; i < uxArraySize; i++) {
		if(strcmp("TicTocDaemon", pxTaskStatusArray[i].pcTaskName) == 0){
			printf("%s - task %s: remaining stack higher mark %u (bytes)\n", identifier, pxTaskStatusArray[i].pcTaskName, pxTaskStatusArray[i].usStackHighWaterMark);
			break;
		}
	}

	vPortFree( pxTaskStatusArray );
} */

/* **************************************************
*	
*                 Server Daemon
*	
**************************************************** */

int setupServerConnection() {
	int socketfd;
	if ( (socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("Failed to create socket"); 
		exit(EXIT_FAILURE); 
	} 

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(8080); 
	
	if (bind(socketfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) 
	{ 
		perror("Failed to bind socket"); 
		exit(EXIT_FAILURE); 
	} 
	return socketfd;
}

void provideTimeStamps(void * parameter) {
	TicTocData* ticTocData = (TicTocData*) parameter;

	int32_t timestamps[2 * 3];
	size_t incomingSize = sizeof(int32_t) * 2;
	size_t outGoingSize = sizeof(int32_t) * 2 * 3;
	
	struct sockaddr_in cliaddr; 
	int socketfd = setupServerConnection();
	socklen_t cliadrrSize = sizeof(cliaddr);

	for(;;) {
		memset(&cliaddr, 0, cliadrrSize);

		if(recvfrom(socketfd, (int32_t *) timestamps, incomingSize, MSG_WAITALL, (struct sockaddr *) &cliaddr, &cliadrrSize) > 0){
			encodeEpochInMicros(esp_timer_get_time(), timestamps, 2);

			#ifdef TICTOC_DAEMON_DEBUG
			if(*(ticTocData->timeRequest) != -1) {
				printf("TicTocDaemon ---------------- timeRequested: %"PRId64"-------------------.\n", *(ticTocData->timeRequest));
				*(ticTocData->timeRequest) = -1;
			}
			#endif

			printf("TicTocDaemon - received t1: %"PRId64".\n", decodeEpochInMicros(timestamps,0));

			encodeEpochInMicros(esp_timer_get_time(), timestamps, 4);
			sendto(socketfd, (const int32_t *)timestamps, outGoingSize, 0, (const struct sockaddr *) &cliaddr, cliadrrSize); 	
		}  
	}

}

/* **************************************************
*	
*                 Client Daemon
*	
**************************************************** */

void setupServerAddr(struct sockaddr_in * servaddr, const char * serverIp, int serverPort) {
	memset(servaddr, 0, sizeof(*servaddr));
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons(serverPort);
	//servaddr->sin_addr.s_addr = INADDR_ANY;
	inet_aton(serverIp, &(servaddr->sin_addr.s_addr));
}

void getTimeStamps(void * parameter){
		int64_t loopCount = 0;
		TicTocData* ticTocData = (TicTocData*) parameter;
		int64_t resultArray[4];
		int32_t timestamps[2 * 4];
		struct sockaddr_in servaddr;
		setupServerAddr(&servaddr, ticTocData->serverIp, ticTocData->serverPort);

		size_t outGoingSize = sizeof(int32_t) * 2;
		size_t incomingSize = sizeof(int32_t) * 2 * 3;
		socklen_t servaddrSize = sizeof(servaddr);

		for(;;) {
			loopCount++;

			#ifdef TICTOC_DAEMON_DEBUG
			if(*(ticTocData->timeRequest) != -1) {
				printf("TicTocDaemon - timeRequested:%"PRId64" tictocTime:%"PRId64".\n",
						*(ticTocData->timeRequest),
						ticTocReady(ticTocData) ? sicTime(&ticTocData->sicdata, *(ticTocData->timeRequest)) : 0);
				*(ticTocData->timeRequest) = -1;
			}	
			#endif

			//printRemainingStack("TicTocDaemon - Start Loop");
			int64_t t1 = esp_timer_get_time();
			encodeEpochInMicros(t1, timestamps, 0);

			//sending t1
			sendto(ticTocData->sock, (const int32_t *)timestamps, outGoingSize,  0, (const struct sockaddr *) &servaddr, servaddrSize);

			if(recv(ticTocData->sock, (int32_t *)timestamps, incomingSize, MSG_WAITALL) == incomingSize){
				resultArray[3]=esp_timer_get_time();
				for(int i = 0; i<3; i++) {
					resultArray[i]=decodeEpochInMicros(timestamps,2*i);
				}

				if(resultArray[0]!=t1){
					//receive old package discard it, and wait for the current one, that we discard due to contamination caused by the old package
					#ifdef TICTOC_DAEMON_DEBUG
					printf("TicTocDaemon timeout - old package\n");
					#endif	
					recv(ticTocData->sock, (int32_t *)timestamps, incomingSize, MSG_WAITALL);

				} else if (loopCount > 10) {
					#ifdef TICTOC_DAEMON_DEBUG
					printf("TicTocDaemon %lld - t1:%"PRId64" t2:%"PRId64" t3:%"PRId64" t4:%"PRId64"\n", loopCount, resultArray[0], resultArray[1], resultArray[2], resultArray[3]);
					#endif
					sicStep(&ticTocData->sicdata, resultArray[0], resultArray[1], resultArray[2], resultArray[3]);	
				}

				
			} else {
				#ifdef TICTOC_DAEMON_DEBUG
				printf("TicTocDaemon timeout\n");
				#endif
				sicStepTimeout(&ticTocData->sicdata);
			}

			// Pause the task for TIC_TOC_PERIOD ms
			vTaskDelay(TIC_TOC_PERIOD / portTICK_PERIOD_MS);
		}
}

void setupTicToc(TicTocData* ticToc, const char * serverIp, int serverPort)
{
	
	#ifndef DAEMON_SERVER
	sicInit(&ticToc->sicdata);
	
	int sockfd;
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		return;
	}
	
	//read timeout
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 800000; // 0.8 seconds
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const struct timeval*)&tv, sizeof(tv));
	

	ticToc->sock=sockfd;
	ticToc->serverIp = serverIp;
	ticToc->serverPort = serverPort;
	#endif

	xTaskCreatePinnedToCore(
		// Function that should be called
		#ifdef DAEMON_SERVER
		provideTimeStamps,
		#else
		getTimeStamps,
		#endif				
	    "TicTocDaemon",				// Name of the task (for debugging)
	    4000,						// Stack size (bytes)
	    ticToc,						// Parameter to pass
		TIC_TOC_DAEMON_PRIORITY,	// Task priority
	    NULL,             			// Task handle
		1						    // Execution Core
	  );
}

int IRAM_ATTR ticTocReady(TicTocData * ticTocData){
	return sicTimeAvailable(&ticTocData->sicdata);
}

int64_t IRAM_ATTR ticTocTime(TicTocData * ticTocData){
	return sicTime(&ticTocData->sicdata, esp_timer_get_time());
}

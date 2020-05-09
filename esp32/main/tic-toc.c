/*
 * tic-toc-sender.c
 *
 *  Created on: May 9, 2020
 *      Author: jaatadia@gmail.com
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include "tic-toc.h"
#include "microtime.h"

#define TICTOC_SERVER "192.168.0.240"
#define TICTOC_PORT 8080

void setupServerAddr(struct sockaddr_in * servaddr) {
	memset(servaddr, 0, sizeof(*servaddr));
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons(TICTOC_PORT);
	//servaddr->sin_addr.s_addr = INADDR_ANY;
	inet_aton(TICTOC_SERVER, &(servaddr->sin_addr.s_addr));
}

void setupTicToc(ticTocSenderData* ticToc)
{
	int sockfd;
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		return;
	}

	//read timeout
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100000; // 0.1 seconds
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const struct timeval*)&tv, sizeof(tv));

	ticToc->sock=sockfd;
}



int getTimeStamps(ticTocSenderData * ticTocData, int64_t * resultArray){
		int32_t timestamps[2 * 4];
		struct sockaddr_in servaddr;
		setupServerAddr(&servaddr);

		size_t outGoingSize = sizeof(int32_t) * 2;
		size_t incomingSize = sizeof(int32_t) * 2 * 3;
		socklen_t servaddrSize = sizeof(servaddr);

		struct timeval t1;
		epoch(&t1);
		timestamps[0]=htonl((int32_t)t1.tv_sec);
		timestamps[1]=htonl((int32_t)t1.tv_usec);

		//sending t1
		sendto(ticTocData->sock, (const int32_t *)timestamps, outGoingSize,  0, (const struct sockaddr *) &servaddr, servaddrSize);

		if(recv(ticTocData->sock, (int32_t *)timestamps, incomingSize, MSG_WAITALL) == incomingSize){
			for(int i = 0; i<3; i++) {
				resultArray[i]=toMicros(ntohl(timestamps[2*i]), ntohl(timestamps[2*i+1]));
			}
			resultArray[3]=epochInMicros();
			return 0;
		} else {
			return -1;
		}
}

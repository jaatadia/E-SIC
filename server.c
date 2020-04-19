// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <signal.h>
#include "microtime.h"

#define PORT 8080 

static int sigintReceived = 0;

void sig_int_handler(int signum, siginfo_t *info, void *ptr)
{
    printf("Received SIGINT signal. Shutting down.\n"); 
    sigintReceived = 1;
}

void catch_sigint()
{
    static struct sigaction _sigact;
    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = sig_int_handler;
    _sigact.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &_sigact, NULL);
}

int setupConnection() {
	int socketfd;
	if ( (socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("Failed to create socket"); 
		exit(EXIT_FAILURE); 
	} 

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(PORT); 
	
	if (bind(socketfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) 
	{ 
		perror("Failed to bind socket"); 
		exit(EXIT_FAILURE); 
	} 
	catch_sigint();
	return socketfd;
}

int main() { 
	int socketfd = setupConnection();
	uint32_t timestamps[2*3]; //TODO remove magic constants
	struct sockaddr_in cliaddr; 

	//we cannot use uint64 due to esp32 being 32bit architecture.
	size_t incomingSize = sizeof(uint32_t) * 2;
	size_t outGoingSize = sizeof(uint32_t) * 2 * 3;
	socklen_t cliadrrSize = sizeof(cliaddr);
	
	printf("Starting server: %lld \n", epochInMicros());

	while(!sigintReceived){
		memset(&cliaddr, 0, cliadrrSize);
		// TODO add verification of client identity	 
		if(recvfrom(socketfd, (uint32_t *) timestamps, incomingSize, MSG_WAITALL, (struct sockaddr *) &cliaddr, &cliadrrSize) > 0){
			encodeEpochInMicros(epochInMicros(), &timestamps, 1*2);
			printf("Received t1: %lu \n", decodeEpochInMicros(&timestamps, 0)); 
			encodeEpochInMicros(epochInMicros(), &timestamps, 2*2);
			sendto(socketfd, (const uint32_t *)timestamps, outGoingSize, 0, (const struct sockaddr *) &cliaddr, cliadrrSize); 	
		}  
	}
	close(socketfd);
	return 0; 
} 

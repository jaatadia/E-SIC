// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <signal.h>
#include <time.h>
#include "microtime.h"

#define PORT 8080 
#define NULL 0 

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

void microsToTimestam(long long micros, char * timestamp){
	time_t     now;
    struct tm  ts;
    char       buf[80];
    now=micros/1000000L;
    ts = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", &ts);
    sprintf(timestamp, "%s", buf);
}

int main() { 
	char timestamp[100];
	int socketfd = setupConnection();
	int32_t timestamps[2*3]; //TODO remove magic constants
	struct sockaddr_in cliaddr; 

	size_t incomingSize = sizeof(int32_t) * 2;
	size_t outGoingSize = sizeof(int32_t) * 2 * 3;
	socklen_t cliadrrSize = sizeof(cliaddr);
	
	long long micros = epochInMicros();
	microsToTimestam(micros, timestamp);
	printf("Starting server: %lld (%s)\n", micros, timestamp);

	while(!sigintReceived){
		memset(&cliaddr, 0, cliadrrSize);
		// TODO add verification of client identity	 
		if(recvfrom(socketfd, (int32_t *) timestamps, incomingSize, MSG_WAITALL, (struct sockaddr *) &cliaddr, &cliadrrSize) > 0){
			encodeEpochInMicros(epochInMicros(), &timestamps, 1*2);

			micros = decodeEpochInMicros(&timestamps, 0);
			microsToTimestam(micros, timestamp);
			printf("Received t1: %lld (%s).\n", micros, timestamp); 
			
			encodeEpochInMicros(epochInMicros(), &timestamps, 2*2);
			sendto(socketfd, (const int32_t *)timestamps, outGoingSize, 0, (const struct sockaddr *) &cliaddr, cliadrrSize); 	
		}  
	}
	close(socketfd);
	return 0; 
} 

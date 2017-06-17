/*
 * server.c
 *
 *  Created on: Jun 16, 2017
 *      Author: Jonathan
 */

#include "server.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>


statistics globalStats = {{0},0,0};
pthread_mutex_t lock;
int socketfd = -1;



int main(int argc, char **argv) {

	if (registerSignalHandler() < 0)
	{
		return -1;
	}

	if (pthread_mutex_init( &lock, NULL ))
	{
		printf("Error: in pthread_mutex_init()\n");
		return -1;
	}

	socketfd = initListenToPort();
	if (socketfd < 0)
	{
		return -1;
	}

	pthread_t threads[MAX_THREAD_NUM];
	int threadNumber = 0;
	while(1)
	{
		struct sockaddr_in clientAddr;
		unsigned int addrLength = sizeof(struct sockaddr_in );
		long int connectionfd = accept(socketfd, (struct sockaddr *) &clientAddr, &addrLength);
		if (connectionfd < 0)
		{
			// the socket is closed or interrupt by signal. should happen upon receiving SIGINT.
			if (errno ==  EBADF || errno == EINTR)
			{
				break;
			}
			else
			{
				printf("Error: accept failed %s\n", strerror(errno));
				return -1;
			}
		}

		int res;
		res = pthread_create(&threads[threadNumber], NULL, processMessage, (void *) &connectionfd);
		if (res)
		{
			printf("Error: pthread create failed\n");
			return -1;
		}
		printf("received a message, thread made is: %d\n", threadNumber);
		threadNumber++;

	}

	// wait for threads:
	int j;
	for (j=0; j<threadNumber; j++)
	{
		void* status;
	    int res = pthread_join(threads[j], &status);
	    if (res)
	    {
	    	printf("res: %d\n", res);


	      printf( "ERROR in pthread_join(): %s\n", strerror(res));
	      return -1;
	    }
	}

	printf("\ntotal bytes counted: %d\nprintable bytes counted: %d\nwe saw:\n",
			globalStats.bytesCounted, globalStats.printableBytesCounted);
	int k;
	for (k=PRINTABLE_MIN; k <= PRINTABLE_MAX; k++)
	{
		if (globalStats.countPerChar[k] != 0)
		{
			printf("\t%d %c's\n", globalStats.countPerChar[k], (char) k);
		}
	}

	pthread_mutex_destroy(&lock);
	pthread_exit(NULL);
}


int registerSignalHandler(void)
{
	// register the signal handler
	struct sigaction new_action;
	sigfillset(&new_action.sa_mask); // fill so ctrl-c doesn't terminate the program!
	new_action.sa_flags = SA_SIGINFO;
	new_action.sa_sigaction = mySignalHandler;
	if (0 != sigaction(SIGINT, &new_action, NULL)) {
		printf("Signal handle registration failed. %s\n", strerror(errno));
		return -1;
	}

	return 1;
}

void mySignalHandler(int signum, siginfo_t* info, void* ptr)
{
	close(socketfd);
}

int initListenToPort()
{
	int socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd < 0)
	{
		printf("Error: could not listen to port\n");
		return -1;
	}

	struct sockaddr_in addr; initSockAddr(&addr);

	if (bind(socketFd, (struct sockaddr *) &addr, sizeof(addr)) != 0)
	{
		printf("Error: binding failed\n");
		return -1;
	}

	if(listen(socketFd,5) < 0)
	{
		printf("Error: listen failed\n");
		return -1;
	}


	return socketFd;
}

void initSockAddr(struct sockaddr_in *addr)
{
	memset( addr, '0', sizeof(*addr));
	(*addr).sin_family = AF_INET;
	(*addr).sin_port = htons(PORT_NUMBER);
	(*addr).sin_addr.s_addr = htonl(INADDR_ANY);
}

void *processMessage(void *connectionfd)
{
	int * confd = connectionfd;
	statistics localStats = {{0},0};
	char buf[MAX_MESSAGE_SIZE];
	int bytesRead,i;

	bytesRead = read(*confd, buf, MAX_MESSAGE_SIZE);
	if (bytesRead < 0)
	{
		printf("Error: read failed\n");
		return NULL;
	}

	for (i=0; i < bytesRead; i++)
	{
		localStats.bytesCounted++;
		if( buf[i] >= PRINTABLE_MIN && buf[i] <= PRINTABLE_MAX)
		{
			localStats.printableBytesCounted++;
			localStats.countPerChar[(int) buf[i]]++;
		}
	}

	char message[MAX_MESSAGE_SIZE];
	sprintf(message, "FROM SERVER: number of printable chars: %d\n", localStats.printableBytesCounted);

	if (write(*confd, message, MAX_MESSAGE_SIZE) < 0)
	{
		printf("Error: write failed\n");
		return NULL;
	}
	close(*confd);

	if (updateGlobalStats(localStats) < 0)
	{
		return NULL;
	}

	return NULL;
}

int updateGlobalStats(statistics localStats)
{
    int lockRes;
    lockRes = pthread_mutex_lock(&lock);
    if( 0 != lockRes )
    {
      printf( "ERROR in pthread_mutex_lock(): %s\n", strerror( lockRes ) );
      return -1;
    }

    // while locked:
    globalStats.bytesCounted += localStats.bytesCounted;
    globalStats.printableBytesCounted += localStats.printableBytesCounted;
    int i;
    for (i=0; i < ASCI_ALPHABET_SIZE; i++)
    {
    	globalStats.countPerChar[i] += localStats.countPerChar[i];
    }

    lockRes = pthread_mutex_unlock(&lock);
    if( 0 != lockRes )
    {
      printf( "ERROR in pthread_mutex_unlock(): %s\n", strerror( lockRes ) );
      return -1;
    }
	return 0;
}

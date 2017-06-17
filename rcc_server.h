/*
 * server.h
 *
 *  Created on: Jun 16, 2017
 *      Author: Jonathan
 */

#ifndef RCC_SERVER_H_
#define RCC_SERVER_H_

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

// boolean for printing log: request arrived, response sent etc.
#define LOG 0

#define ASCI_ALPHABET_SIZE 512
#define PRINTABLE_MIN 32
#define PRINTABLE_MAX 126
#define MAX_THREAD_NUM 32
#define PORT_NUMBER 2233
// same in client!
#define MAX_MESSAGE_SIZE 1024

typedef struct  statistics {
	int countPerChar[ASCI_ALPHABET_SIZE];
	int bytesCounted;
	int printableBytesCounted;
} statistics;

int registerSignalHandler(void);

void mySignalHandler(int signum, siginfo_t* info, void* ptr);

int initListenToPort();

void initSockAddr(struct sockaddr_in *addr);

void * processData(void *connectionfd);

int updateGlobalStats(statistics localStats);

int waitForAllThreadsToFinish(void);

int signalNoActiveThreads(void);

void printGlobalStats(void);

#endif /* RCC_SERVER_H_ */

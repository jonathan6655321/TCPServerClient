/*
 * server.h
 *
 *  Created on: Jun 16, 2017
 *      Author: Jonathan
 */

#ifndef SERVER_H_
#define SERVER_H_

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

#define ASCI_ALPHABET_SIZE 512
#define PRINTABLE_MIN 32
#define PRINTABLE_MAX 126
#define MAX_THREAD_NUM 32
#define PORT_NUMBER 2233
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

void * processMessage(void *connectionfd);

int updateGlobalStats(statistics localStats);

#endif /* SERVER_H_ */

/*
 * client.h
 *
 *  Created on: Jun 16, 2017
 *      Author: Jonathan
 */

#ifndef RCC_CLIENT_H_
#define RCC_CLIENT_H_

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
#include <pthread.h>

#define INPUT_ADDRESS "/dev/urandom"
#define SERVER_PORT 2233
#define SERVER_IP_ADDRESS "127.0.0.1"
// same in server:
#define MAX_MESSAGE_SIZE 1024


//int getInput(unsigned int numBytesToWrite, char **buf);

void initSockAddr(struct sockaddr_in *addr);

// returns connectionFileDescriptor
int connectToServer(void);

//writes the response into response
int getResponse(int *response, int serverfd);

int transferDataToServer(int numBytesToWrite, int clientSocketfd);


#endif /* RCC_CLIENT_H_ */

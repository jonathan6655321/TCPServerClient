/*
 * client.c
 *
 *  Created on: Jun 16, 2017
 *      Author: Jonathan
 */

//#include <sys/socket.h>
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

#include "client.h"

// assumes an integer argument.
int main(int argc, char **argv) {

	if (argc != 2)
	{
		printf("Error: wrong number of args\n");
		return -1;
	}

	unsigned int numCharsToWrite;
	sscanf(argv[1], "%d", &numCharsToWrite);

	int clientSocketfd = connectToServer();
	if (clientSocketfd < 0)
	{
		return -1;
	}

	if (transferDataToServer(numCharsToWrite, clientSocketfd) < 0)
	{
		return -1;
	}

	int response;
	if (getResponse(&response, clientSocketfd) < 0)
	{
		return -1;
	}

	printf("The response is:\n%d\n", response);

	close(clientSocketfd);

	return 0;
}


void initSockAddr(struct sockaddr_in *addr)
{
	memset( addr, '0', sizeof(*addr));
	(*addr).sin_family = AF_INET;
	(*addr).sin_port = htons(SERVER_PORT);
	(*addr).sin_addr.s_addr = inet_addr("127.0.0.1");
}

int connectToServer(void)
{
	int clientSocketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocketfd < 0)
	{
		printf("Error: failed to create client socket\n");
		return -1;
	}

	struct sockaddr_in serverAddr; initSockAddr(&serverAddr);
	if(connect(clientSocketfd,(struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0)
	{
		printf("Error: Connect Failed\n");
		return -1;
	}

	return clientSocketfd;
}

int getResponse(int *response, int serverfd)
{
	if (read(serverfd, response, sizeof(int)) < sizeof(int))
	{
		printf("Error: get response failed\n");
		return -1;
	}
	return 0;
}

int transferDataToServer(int numBytesToWrite, int clientSocketfd)
{
	int fd = open(INPUT_ADDRESS, O_RDONLY);
	if (fd < 0)
	{
		printf("Error: failed to open file for reading\n");
		return -1;
	}

	// write to server the total number of bytes to be written
	if (write(clientSocketfd, &numBytesToWrite, sizeof(int)) != sizeof(int))
	{
		printf("Error: write size failed\n");
		return -1;
	}

	char msg[MAX_MESSAGE_SIZE];
	int currentMessageSize = 0;
	int totalBytesTransfered = 0;
	while (totalBytesTransfered < numBytesToWrite)
	{
		currentMessageSize = read(fd, msg, MAX_MESSAGE_SIZE);
		if (currentMessageSize < 0)
		{
			printf("Error: failed to read from file\n");
			return -1;
		}

		int totalBytesSuccesfullyWritten = 0;
		int numBytesJustWritten = 0;
		while (totalBytesSuccesfullyWritten < currentMessageSize)
		{
			numBytesJustWritten = write(clientSocketfd, msg + totalBytesSuccesfullyWritten, currentMessageSize - totalBytesSuccesfullyWritten);
			if (numBytesJustWritten < 0)
			{
				printf("Error: write failed\n %s\n", strerror(errno));
				return -1;
			}
			totalBytesSuccesfullyWritten += numBytesJustWritten;
		}

		totalBytesTransfered += currentMessageSize;
	}

	close(fd);
	return 0;
}

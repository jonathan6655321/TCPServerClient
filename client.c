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

	char *buf;
	if (getInput(numCharsToWrite, &buf) < 0)
	{
		return -1;
	}

	int clientSocketfd = connectToServer();
	if (clientSocketfd < 0)
	{
		return -1;
	}

	if (write(clientSocketfd, buf, numCharsToWrite) < 0)
	{
		printf("Error: write failed\n");
		return -1;
	}

	char response[MAX_MESSAGE_SIZE];
	if (getResponse(response, clientSocketfd) < 0)
	{
		return -1;
	}

	printf("The response is:\n%s\n", response);

	close(clientSocketfd);
	free(buf);

	return 0;
}

int getInput(unsigned int numCharsToWrite, char **buf)
{
	*buf = (char *) malloc((numCharsToWrite + 1)*sizeof(char));
	if (*buf == NULL)
	{
		printf("Error: malloc for buffer failed\n");
		return -1;
	}

	int fd = open(INPUT_ADDRESS, O_RDONLY);
	if (fd < 0)
	{
		printf("Error: failed to open file for reading\n");
		return -1;
	}

	if (read(fd, *buf, numCharsToWrite) != numCharsToWrite)
	{
		printf("Error: failed to read from file\n");
		return -1;
	}

	(*buf)[numCharsToWrite] = '\0';
	close(fd);
	return 1;
}

void initSockAddr(struct sockaddr_in *addr)
{
	memset( addr, '0', sizeof(*addr));
	(*addr).sin_family = AF_INET;
	(*addr).sin_port = htons(SERVER_PORT);
	(*addr).sin_addr.s_addr = htonl(INADDR_ANY);
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

int getResponse(char *response, int serverfd)
{
	int i = 0;
	int readAmount;
	while(i < MAX_MESSAGE_SIZE)
	{
		printf("client is here\n");
		readAmount = read(serverfd, response + i, MAX_MESSAGE_SIZE - i);
		if( readAmount < 0)
		{
			printf("Error: couldn't read\n");
			return -1;
		}
		else if(readAmount == 0)
		{
			break;
		}
		i += readAmount;
	}
}

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include<stdbool.h>

/* OpenSSL headers */
#include <openssl/sha.h>

#define MAX 49
#define PORT 8080
#define SA struct sockaddr

typedef struct
{
	uint8_t hashvalue[32];
	uint64_t start;
	uint64_t end;
	uint8_t p;
} packet;

typedef struct
{
	uint64_t number;
	uint8_t resultHash[32];
} resultStruct;


bool compareHashes(unsigned char *guess, unsigned char *target) {
	printf("Entered compareHashes");
	for (int i = 0; i < 32; i++)
	{
		if (guess[i] != target[i])
		{
			return false;
		}
	}
	// printf("Found result");
	return true;
}

// Function designed for chat between client and server.
void func(int sockfd)
{
	char buff[MAX];
	int n;
	packet *Packet1;

	bzero(buff, MAX);

	// read the message from client and copy it in buffer
	read(sockfd, buff, sizeof(buff));
	Packet1 = (packet *)buff;
	// print buffer which contains the client contents
	printf("\n\n");
	int i;
	for (i = 0; i < 32; i++)
		printf("%02x", Packet1->hashvalue[i]);
	printf("\n63");
	printf("\nStart: %li", Packet1->start);
	printf("\nEnd: %li", Packet1->end);
	printf("\nbe64toh Start: %li", be64toh(Packet1->start));
	printf("\nbe64toh End: %li", be64toh(Packet1->end));
	printf("\nP: %d", Packet1->p);
	printf("\nFrom start: %li end: %li priority: %d", be64toh(Packet1->start), be64toh(Packet1->end), Packet1->p);
	printf("\nP: %d", Packet1->p);

	fflush(stdout);
	printf("\nP: %d", Packet1->p);

	printf("67");
	uint64_t x;
	uint64_t result;
	uint64_t start = be64toh(Packet1->start);
	uint64_t end = be64toh(Packet1->end);
	printf("Line 71");
	resultStruct *resultTable[1000][sizeof(end) + 32];
	printf("Before bzero line 73");
	for(int i = 0; i < end-start; i++) {
		bzero(resultTable[i], 32);
	}
	int resultCounter = -1;
	bool resultLock = false;

	result = -1;

	printf("Line 81");
	for(int i = 0; i < resultCounter; i++) {
		if(compareHashes(resultTable[x-start], Packet1->hashvalue)){
			result = x;
			resultLock = true;
			break;
		}
	}
	printf("Line 89");

	if (!resultLock)
	{
		for (x = start; x < end; x++)
		{
			unsigned char *guess = SHA256((unsigned char *)&x, 8, 0);

			if (compareHashes(guess, Packet1->hashvalue))
			{
				result = x;
				resultLock = true;
				break;
			}
		}
	}
	printf("Line 105");
	result = htobe64(result);

	// and send that buffer to client
	write(sockfd, &result, sizeof(result));
	// }
}

// Driver function
int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
	{
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	for (;;)
	{
		// Now server is ready to listen and verification
		if ((listen(sockfd, 5)) != 0)
		{
			printf("Listen failed...\n");
			exit(0);
		}
		else
			printf("Server listening..\n");
		len = sizeof(cli);

		// Accept the data packet from client and verification
		connfd = accept(sockfd, (SA *)&cli, &len);
		if (connfd < 0)
		{
			printf("server accept failed...\n");
			exit(0);
		}
		else
			printf("server accept the client...\n");

		// Function for chatting between client and server
		func(connfd);
	}

	// After chatting close the socket
	close(sockfd);
}

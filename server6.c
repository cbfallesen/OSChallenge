#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

/* OpenSSL headers */
#include <openssl/sha.h>

#define MAX 49
#define PORT 8080
#define SA struct sockaddr

typedef struct
{
	uint8_t hash[32];
	uint64_t start;
	uint64_t end;
	uint8_t p;
} Request;

bool compareHashes(unsigned char *guess, unsigned char *target) {
	for (int i = 0; i < 32; i++)
	{
		if (guess[i] != target[i])
		{
			return false;
		}
	}
	return true;
}

void solveSha(int connfd)
{
	char buffer[MAX];
	bzero(buffer, MAX);
	read(connfd, buffer, sizeof(buffer));
	Request *request = (Request*) buffer;

	uint64_t start = be64toh(request->start);
	uint64_t end = be64toh(request->end);
	
	for (uint64_t i = start; i < end; i++)
	{
		unsigned char *guess = SHA256((unsigned char *)&i, 8, 0);
		if(compareHashes(guess, request->hash))
		{
			uint64_t result = htobe64(i);
			write(connfd, &result, sizeof(result));
			close(connfd);
			break;
		}
	}
}

// Function designed for chat between client and server.
void forkStage(int connfd) {
	pid_t pid = fork();
	if( pid == 0 ){
		solveSha(connfd);
		exit(0);
	} else if ( pid == -1){
		printf("Failed creating fork.\n");
		exit(0);
	} else {
		close(connfd);
	}
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

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0)
	{
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	
	len = sizeof(cli);
	
	for (;;)
	{
		// Accept the data packet from client and verification
		connfd = accept(sockfd, (SA *)&cli, &len);
		if (connfd < 0)
		{
			printf("server accept failed...\n");
			exit(0);
		}
		else
			printf("server accept the client...\n");
		
		forkStage(connfd);
	}
	return 0;
}

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

typedef struct
{
	uint64_t number;
	uint8_t resultHash[32];
} resultStruct;

resultStruct results[20];
int resultsCounter = 0;

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

uint64_t searchResults(resultStruct *resultTable, uint8_t *hash) {
	for (int i = 0; i < 20; i++)
	{
		if(compareHashes(resultTable[i].resultHash, hash)) {
			return resultTable[i].number;
		}
	}
	return -1;
}

void printResults(resultStruct *results, uint8_t *hash) {
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 32; j++)
		printf("%02x", results[i].resultHash[j]);
	}
}

//https://stackoverflow.com/questions/49581349/how-to-get-return-value-from-child-process-to-parent
void solveSha(int connfd, int *pipefd)
{
	char buffer[MAX];
	bzero(buffer, MAX);
	read(connfd, buffer, sizeof(buffer));
	Request *request = (Request*) buffer;

	uint64_t start = be64toh(request->start);
	uint64_t end = be64toh(request->end);
	
	// uint64_t prevAnswer = searchResults(results, request->hash);
	// printResults(results, request->hash);
	
	for (uint64_t i = start; i < end; i++)
	{
		unsigned char *guess = SHA256((unsigned char *)&i, 8, 0);
		if(compareHashes(guess, request->hash))
		{
			uint64_t result = htobe64(i);
			write(connfd, &result, sizeof(result));
			write(pipefd[1], result, sizeof(result));
			close(connfd);
			break;
		}
	}
}

// https://www.geeksforgeeks.org/fork-system-call/
// https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/
void forkStage(int connfd) {
	int fd[2];
	if(pipe(fd) == -1){
		printf("Pipe failed\n");
		exit(0);
	}

	pid_t pid = fork();
	if( pid  < 0) {
		//Error
		printf("Failed creating fork.\n");
		exit(0);
	} else if ( pid > 0 ){
		//Parent process
		wait(NULL);
		close(fd[1]);
		char buff[sizeof(uint64_t)];
		bzero(buff, sizeof(buff));
		read(fd[0], buff, sizeof(buff));
		uint64_t result = (uint64_t) buff;
 		printf("Result address: %li\n", result);
		
		close(fd[0]);
		close(connfd);
	} else {
		//Child process
		solveSha(connfd, fd);
		exit(0);
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
			// printf("server accept the client...\n");
		
		forkStage(connfd);
	}
	return 0;
}

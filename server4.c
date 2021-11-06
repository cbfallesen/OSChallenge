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
#define MAX_THREADS 5

typedef struct
{
	uint8_t hashvalue[32];
	uint64_t start;
	uint64_t end;
	uint8_t p;
} packet;

typedef struct
{
	uint8_t localHash[32];
	uint64_t start;
	uint64_t end;
} threadStruct;

uint64_t result;

pthread_mutex_t hashMutex;
pthread_mutex_t resultMutex;
pthread_mutex_t connectionMutex;

bool compareHashes(unsigned char *guess, unsigned char *target) {
	pthread_mutex_lock(&hashMutex);
	for (int i = 0; i < 32; i++)
	{
		if (guess[i] != target[i])
		{
			pthread_mutex_unlock(&hashMutex);
			return false;
		}
	}
	pthread_mutex_unlock(&hashMutex);
	// printf("Found result");
	return true;
}

void *threadFunction(void *arguments)
{
	threadStruct *args = arguments;
	// printf("New thread.\n");
	// printf("Start: %ld\n", args->start);
	// printf("End: %ld\n\n", args->end);
	// printf("Hash: ");
	// for (int i = 0; i < 32; i++)
	// 	printf("%02x", args->localHash[i]);
	// printf("\n");
	for (uint64_t i = args->start; i < args->end; i++)
	{
		unsigned char *guess = SHA256((unsigned char *)&i, 8, 0);
		if(compareHashes(guess, args->localHash)){
			pthread_mutex_lock(&resultMutex);
			result = i;
			pthread_mutex_unlock(&resultMutex);
			// printf("Result was found: %ld\n\n\n", result);
			pthread_exit(NULL);
		}
	}
	// printf("Result was not found: %ld\n\n\n", result);
	pthread_exit(NULL);
}

// Function designed for chat between client and server.
void func(int sockfd)
{
	char buff[MAX];
	bzero(buff, MAX);
	// read the message from client and copy it in buffer
	read(sockfd, buff, sizeof(buff));
	packet *Packet1 = (packet *)buff;
	
	uint64_t start = be64toh(Packet1->start);
	uint64_t end = be64toh(Packet1->end);

	pthread_mutex_init(&hashMutex, NULL);
	pthread_mutex_init(&resultMutex, NULL);

	// // print buffer which contains the client contents
	// printf("\n\n");
	// for (int i = 0; i < 32; i++)
	// 	printf("%02x", Packet1->hashvalue[i]);
	// printf("\n\n");


	pthread_t threads[MAX_THREADS];


	uint64_t partitionSize = (end - start)/MAX_THREADS;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		threadStruct *args = malloc(sizeof (threadStruct));
		args->start = (partitionSize *  i) + start;
		args->end = (partitionSize *  i) + start + partitionSize;
		memcpy(args->localHash, Packet1->hashvalue, sizeof(Packet1->hashvalue));
		
		pthread_create(&threads[i], NULL, threadFunction, args);
		free(args);
	}
	
	for (int i = 0; i < MAX_THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}
	
	
	result = htobe64(result);

	pthread_mutex_destroy(&resultMutex);
	pthread_mutex_destroy(&hashMutex);
	// and send that buffer to client
	write(sockfd, &result, sizeof(result));
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

	pthread_mutex_init(&connectionMutex, NULL);
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
	pthread_mutex_destroy(&connectionMutex);
	// After chatting close the socket
	close(sockfd);
}

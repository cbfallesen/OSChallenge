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
#define MAX_THREADS 3

typedef struct
{
	uint8_t hash[32];
	uint64_t start;
	uint64_t end;
	uint8_t p;
} Request;

typedef struct{
	Request request;
	int socket;
} Packet;

Packet *currentPacket;
bool lock = true;
// request Request currentRequest;

bool compareHashes(unsigned char *guess, unsigned char *target) {
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

void *threadFunction(void *arguments)
{
	for (;;)
	{
		if(!lock) {
			Packet *localPacket = malloc(sizeof(Packet));
			memcpy(localPacket, currentPacket, sizeof(localPacket));
			if(localPacket->socket != -1) {
				printf("\n\n");
				for (int j = 0; j < 32; j++)
					printf("%02x", localPacket->request.hash[j]);

				uint64_t start = be64toh(localPacket->request.start);
				uint64_t end = be64toh(localPacket->request.end);
				
				for (uint64_t i = start; i < end; i++)
				{
					unsigned char *guess = SHA256((unsigned char *)&i, 8, 0);
					if(compareHashes(guess, localPacket->request.hash))
					{
						printf("67\n");
						lock = true;
						uint64_t result = htobe64(i);
						// and send that buffer to client
						write(localPacket->socket, &result, sizeof(result));
						close(localPacket->socket);
						lock = false;
					}
				}
				lock = false;
				break;
			}
		}
	}
	return NULL;
}

// Function designed for chat between client and server.
void func(int sockfd)
{
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

	for(int i = 0; i < MAX_THREADS; i++){
        pthread_t thread;
        pthread_create(&thread, NULL, &threadFunction, NULL);
    }
	
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
		
		lock = false;
		char buff[MAX];
		bzero(buff, MAX);
		// read the message from client and copy it in buffer
		read(sockfd, buff, sizeof(buff));
		currentPacket = (Packet *) buff;
		currentPacket->socket = connfd;
	}
	// After chatting close the socket
	close(sockfd);
}

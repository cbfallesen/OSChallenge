#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

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

packet *Packet1;
uint64_t result;

void *threadfunc(void *threadid)
{
	uint64_t  numGuess = (uint64_t) threadid;

	unsigned char *guess = SHA256((unsigned char *)&numGuess, 8, 0);

		int equal = 1;
		for (int i = 0; i < 32; i++)
		{
			if (guess[i] != Packet1->hashvalue[i])
			{
				equal = 0;
				pthread_exit;
			}
		}

		if (equal == 1)
		{
			result = numGuess;
			pthread_exit;
		}
}

// Function designed for chat between client and server.
void func(int sockfd)
{
	char buff[MAX];
	int n;
	// packet *Packet1;

	bzero(buff, MAX);

	// read the message from client and copy it in buffer
	read(sockfd, buff, sizeof(buff));
	Packet1 = (packet *)buff;

	// print buffer which contains the client contents
	printf("\n\n");
	int i;
	for (i = 0; i < 32; i++)
		printf("%02x", Packet1->hashvalue[i]);

	printf("\nFrom start: %li end: %li priority: %d", be64toh(Packet1->start), be64toh(Packet1->end), Packet1->p);

	uint64_t x;
	// uint64_t result;
	result = -1;


	//Change from hardcoded 10
	pthread_t threads[10];


	for (x = be64toh(Packet1->start); x < be64toh(Packet1->end); x++)
	{
		pthread_create(&threads[x], NULL, threadfunc, (void *) x);
	}

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

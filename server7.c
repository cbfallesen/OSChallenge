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
	uint8_t hash[32];
} resultStruct;

struct Node
{
	resultStruct *data;
	struct Node *next;	
};

struct Node *startNode = NULL;

void* pushResult (struct Node **refNode, resultStruct *newData, size_t dataSize) {
	printf("push");
	struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));

	newNode->data = malloc(dataSize);
	newNode->next = (*refNode);

	newNode->data->number = newData->number;
	memcpy(newNode->data->hash, newData->hash, sizeof(newData->hash));


	printf("newNode:\n");
	for (int i = 0; i < 32; i++)
		printf("%02x", newNode->data->hash[i]);
	printf("\n\n");
	//(*refNode) = newNode;

	return newNode;
}

void print(struct Node *head) {
    struct Node *current_node = head;
   	while ( current_node != NULL) {
        printf("%li ", current_node->data->number);
        current_node = current_node->next;
    }
}

int fd[2];

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

//https://stackoverflow.com/questions/49581349/how-to-get-return-value-from-child-process-to-parent
void solveSha(int connfd)
{
	close(fd[0]);
	char buffer[MAX];
	bzero(buffer, MAX);
	read(connfd, buffer, sizeof(buffer));
	Request *request = (Request*) buffer;

	uint64_t start = be64toh(request->start);
	uint64_t end = be64toh(request->end);
	uint64_t result;
	bool resultLock = false;
	struct Node *node = startNode;
	resultStruct resultData;

	printf("Before print.\n");
	if (startNode == NULL) {
		printf("Null\n");
	} else {
		print(startNode);
	}

	while(node != NULL) {
		if(compareHashes(node->data->hash, request->hash)){
			printf("Found hash in results\n");
			result = htobe64(node->data->number);
			resultLock = true;
			write(connfd, &result, sizeof(result));
			close(connfd);
			break;
		}
		node = node->next;
	}
	
	if (!resultLock) {
		for (uint64_t i = start; i < end; i++)
		{
			unsigned char *guess = SHA256((unsigned char *)&i, 8, 0);
			if(compareHashes(guess, request->hash))
			{
				printf("Found hash\n");
				resultStruct resultStruct;
				resultStruct.number = i;
				memcpy(resultStruct.hash,request->hash, sizeof(request->hash));

				printf("%ld.\n", resultStruct.number);
				printf("resultData:\n");
				for (int i = 0; i < 32; i++)
					printf("%02x", resultStruct.hash[i]);
				printf("\n\n");

				uint64_t result = htobe64(i);
				
				write(connfd, &result, sizeof(result));
				write(fd[1], &resultStruct, sizeof(resultStruct));
				close(fd[1]);
				close(connfd);
				break;
			}
		}
	}
}

// Function designed for chat between client and server.
// https://www.geeksforgeeks.org/fork-system-call/
// https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/
// https://www.youtube.com/watch?v=Mqb2dVRe0uo
void forkStage(int connfd) {
	if(pipe(fd) == -1) {
			printf("Error piping.\n");
			exit(0);
	}

	pid_t pid = fork();
	if( pid  < 0) {
		//Error
		printf("Failed creating fork.\n");
		exit(0);
	} else if ( pid > 0 ){
		//Parent process
		close(fd[1]);
		resultStruct forkresult;
		read(fd[0], &forkresult, sizeof(resultStruct));

		startNode = pushResult(&startNode, &forkresult, sizeof(resultStruct));

		close(fd[0]);
		close(connfd);
		// printf("Received answer from child: %ld \n", forkresult.number);
		// for (int j = 0; j < 32; j++)
		// 		printf("%02x", forkresult.hash[j]);
		// 	printf("\n");
	} else {
		//Child process
		solveSha(connfd);
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

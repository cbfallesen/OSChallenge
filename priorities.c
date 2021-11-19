#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

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

struct Node
{
	packet *data;
	struct Node *next;	
};

struct Node *startNode = NULL;

void* pushResult (struct Node **refNode, packet *newData, size_t dataSize) {
	printf("push");
	struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));

	newNode->data = malloc(dataSize);
	newNode->next = (*refNode);

	newNode->data->start = newData->start;
	newNode->data->end = newData->end;
	newNode->data->p = newData->p;
	memcpy(newNode->data->hashvalue, newData->hashvalue, sizeof(newData->hashvalue));

	printf("\n\n");
	int i;
	for (i = 0; i < 32; i++)
		printf("%02x", newNode->data->hashvalue[i]);

	printf("\nFrom start: %li end: %li priority: %d\n", be64toh(newNode->data->start), be64toh(newNode->data->end), newNode->data->p);
	//(*refNode) = newNode;

	return newNode;
}

// https://www.geeksforgeeks.org/linked-list-set-3-deleting-node/
void* popResult(struct Node** refNode, int priority) {
	printf("pop");

	// Store head node
    struct Node *temp = refNode, *prev;
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->data->p == priority) {
        *refNode = temp->next; // Changed head
        free(temp); // free old head
        return;
    }
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->data != priority) {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (temp == NULL)
        return;
 
    // Unlink the node from linked list
    prev->next = temp->next;
 
    free(temp); // Free memory

	// struct Node *current_node = startNode;
	// while (current_node->next->next != NULL)
	// {
	// 	current_node = current_node->next;
	// }
	// current_node->next = refNode.next;
	// refNode.next = NULL;

	// return refNode;
}

void print(struct Node *head) {
    struct Node *current_node = head;
   	while ( current_node != NULL) {
        // printf("%li ", current_node->data->p);
        current_node = current_node->next;
    }
}

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

// Function designed for chat between client and server.
void func(int sockfd, struct Node* starter)
{
	struct Node *node = startNode;
	struct Node *currentNode = startNode;
	uint64_t currentP = 0;

	while(node != NULL) {
		printf("Searching list\n");
		if(node->data->p > currentP){
			printf("found higher\n");
			currentP = node->data->p;
			currentNode = node;
		}
		node = node->next;
	}

	uint64_t x;
	uint64_t result;
	uint64_t start = be64toh(currentNode->data->start);
	uint64_t end = be64toh(currentNode->data->end);

	for (x = start; x < end; x++)
	{
		unsigned char *guess = SHA256((unsigned char *)&x, 8, 0);

		if (compareHashes(guess, currentNode->data->hashvalue))
		{
			result = x;
			break;
		}
	}

	startNode = popResult(startNode, currentP);

	result = htobe64(result);
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

		char buff[MAX];
		int n;
		packet *Packet1;
		bzero(buff, MAX);// read the message from client and copy it in buffer
		int readId = read(connfd, buff, sizeof(buff));
		printf("%d\n", readId);
		Packet1 = (packet *)buff;

		printf("Before print.\n");
		if (startNode == NULL) {
		printf("Null\n");
		} else {
		print(startNode);
		}

		startNode = pushResult(&startNode, Packet1, sizeof(buff));
	}

	while (startNode != NULL)
	{
		// Function for chatting between client and server
		func(connfd, startNode);
	}
	// After chatting close the socket
	close(sockfd);
}

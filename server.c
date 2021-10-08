#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include "messages.h"

typedef struct { 
    uint8_t hashvalue[32]; 
    uint64_t start; 
    uint64_t end;
    uint8_t p;
} packet;

uint64_t reverse(uint64_t start, uint64_t end, char *hash) {
    printf("Entered reverse\n");
    char *targetHash = hash;
    for (uint64_t i = start; i < end; i++) {

        //Generates a SHA256 hash for the current iteration
        SHA256_CTX shactx;
        char digest[SHA256_DIGEST_LENGTH];
        SHA256_Init(&shactx);
        SHA256_Update(&shactx, i, SHA256_DIGEST_LENGTH);
        SHA256_Final(digest, &shactx);
        char *newHash = digest;

        //Iterate through 
        int j;
        for (j = 0; j < SHA256_DIGEST_LENGTH; i++)
        {
            printf("   %d\n",j);
            if(*(targetHash+j) != *(newHash+j)) {
                break;
            }
        }

        if(j == SHA256_DIGEST_LENGTH - 1) {
            return i;
        }
        

    }

    return -1;
}

int communicate (int sockfd) {
    printf("Entered\n");
    int  n;
    char buffer[49];
    packet *packet1;

    //Start communicating, first read input from client
    bzero(buffer,49);
    n = read(sockfd,buffer,48 );
    packet1 = (packet*) buffer;
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    printf("Read\n");
    
    printf("Message received: \n");
    //Input from client
    uint8_t hashR[32];
    uint64_t startR;
    uint64_t endR;

    for (int i = 0, j=31; i < 32, j >= 0; i++, j--)
    {
        hashR[i] = be64toh(packet1 -> hashvalue[j]);
    }
    startR = be64toh(packet1 -> start);
    endR = be64toh(packet1 -> end);
    printf("flipped byte order\n");

    printf("%ld", reverse(startR, endR, hashR));
    
    //Then write a response
    n = write(sockfd,"I got your message",18);
    
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
}

int main( int argc, char *argv[] ) {
    int serverSock, newsockfd, portNum, clientLen;
    struct sockaddr_in serv_addr, cli_addr;
    
    //Call the socket
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    
    //Handling missing sockets
    if (serverSock < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    
    //Initialising the socket structures
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portNum = 5001;
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNum);
    
    //Binding to the host address + error handling
    if (bind(serverSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    listen(serverSock,5);
    clientLen = sizeof(cli_addr);
    
    //Accept connection from the client
    newsockfd = accept(serverSock, (struct sockaddr *)&cli_addr, &clientLen);

    //Handling missing connection from the client
    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }

    communicate(newsockfd);
        
    return 0;
}
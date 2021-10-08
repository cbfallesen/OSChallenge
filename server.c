#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include "messages.h"

struct Packet { 
    char hashvalue[32]; 
    uint64_t start; 
    uint64_t end;
    uint8_t p;
} packet;

uint64_t reverse(uint64_t start, uint64_t end, char *argv) {
    printf("Entered reverse - ");
    printf("  hash: ");
    for (int i = 0; i < 32; i++)
    {   
        printf("%c",*(argv + i));
    }
    printf("\n");

    unsigned char *targetHash = argv;
    unsigned char newHash[32];
    printf("Target");
    for (uint8_t i = start; i < end; i++) {

        i = htole64(i);
        printf("  i: %ld",i);
        
        //Generates a SHA256 hash for the current iteration
        // SHA256_CTX shactx;
        // static unsigned char digest[SHA256_DIGEST_LENGTH];
        // SHA256_Init(&shactx);
        // SHA256_Update(&shactx, &i, SHA256_DIGEST_LENGTH);
        // SHA256_Final(digest, &shactx);
        // unsigned char *newHash = digest;

        SHA256((unsigned char*) &i, 8, (unsigned char*) newHash);

        //Iterate through 
        int j;
        for (j = 0; j < SHA256_DIGEST_LENGTH; i++)
        {
            printf("     %d\n",j);
            if(*(targetHash+j) != *(newHash+j)) {
                break;
            }
        }

        if(j == SHA256_DIGEST_LENGTH - 1) {
            return i;
        }
        printf("\n");

    }

    return -1;
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

    printf("Entered\n");
    int  n;
    char buffer[49];

    //Start communicating, first read input from client
    bzero(buffer,49);
    n = read(newsockfd,buffer,48 );
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    printf("Read\n");
    
    printf("Message received: \n");
    
    //Input from client
    unsigned char *hashArr = buffer;
    uint64_t startR = be64toh(packet.start);
    uint64_t endR = be64toh(packet.end);

    // for (int i = 0; i < 32; i++)
    // {
    //     ha[i] = packet.hashvalue[i] | ((uint8_t)buffer[31-i] << (i*8));
    //     printf("%02x", hashR[i]);
    // }
    // printf("\n");

    uint8_t outBuff[8];
    bzero(outBuff, sizeof(outBuff)); 

    printf("flipped byte order\n");
    uint64_t ans = reverse(startR, endR, hashArr);
    uint64_t ansR = htobe64(ans);
    memcpy(outBuff, &ansR, (size_t) 8);
    printf("Made it out\n");
    printf("%ld", ans);
    
    //Then write a response
    n = write(newsockfd, outBuff, 8);
    
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    return 0;
}
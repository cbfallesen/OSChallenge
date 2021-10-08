#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

typedef struct { 
    uint8_t hashvalue[32]; 
    uint64_t start; 
    uint64_t end;
    uint8_t p;
} packet;

int communicate (int sockfd) {
    int  n;
    char buffer[49];
    packet *packet1;

    //Start communicating, first read input from client
    bzero(buffer,49);
    n = read(sockfd,buffer,48 );
    
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    
    printf("Message received: \n");
    uint8_t hash[32];

    for (int i = 0; i < 32; i++) {
        hash[i] = buffer[i];
    }
    for (int i = 0; i < 32; i++) {
        printf("%u",hash[i]);
    }
    printf("\n");
    printf("%u", &hash);
    printf("\n");
    
    unsigned char *startArray[8];

    for (int i = 32; i < 40; i++) {
        startArray[i - 32] = buffer[i];
    }
    for (int i = 0; i < 8; i++)
    {
        printf("%u",startArray[i]);
    }
    printf("\n");
    
    
    uint64_t start;
    memcpy(&start, startArray, 4);

    be64toh(packet1->start)
    printf("%u", start);
    printf("\n");

    char endArray[8];

    for (int i = 40; i < 48; i++) {
        endArray[i - 40] = buffer[i];
    }
    for (int i = 0; i < 8; i++)
    {
        printf("%u",endArray[i]);
    }
    printf("\n");
    
    uint64_t end = atoi(&endArray);

    printf("%u", end);
    


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
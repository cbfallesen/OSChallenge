#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

int main( int argc, char *argv[] ) {
    int serverSock, newsockfd, portNum, clientLen;
    char buffer[49];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;
    
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
    
    //Start communicating, first read input from client
    bzero(buffer,49);
    n = read( newsockfd,buffer,48 );
    
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
    
    char startArray[] = malloc((8)*sizeof(char));

    for (int i = 32, j = 0; i < 40; i++, j++) {
        startArray[j] = buffer[i];
    }
    
    uint64_t start = ToUint64(startArray, 0);

    printf("%u", start);

    char endArray[] = malloc((8)*sizeof(char));

    for (int i = 40, j = 0; i < 48; i++, j++) {
        endArray[j] = buffer[i];
    }
    
    uint64_t end = ToUint64(endArray, 0);

    printf("%u", end);


    //Then write a response
    n = write(newsockfd,"I got your message",18);
    
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
        
    return 0;
}
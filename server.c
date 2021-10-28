#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include "messages.h"

typedef struct Packet { 
    char hashvalue[32]; 
    uint64_t start; 
    uint64_t end;
    uint8_t p;
} Packet;

uint64_t reverse(uint64_t start, uint64_t end, char *hash) {
    unsigned char *targetHash = hash;
    
    for (uint8_t i = start; i < end; i++) {
        //reverse endian
        i = htole64(i);
        
        //Client forventer en binary string af hash
        //Sørg for at parse request rigtigt, står som hints under rapporten
        //Kan sende 1 request med 1 som det tal der skal hashes
        //Når det virker kan vi gå videre til at fixe hashe

        //Generates a SHA256 hash for the current iteration
        SHA256_CTX shactx;
        static unsigned char digest[SHA256_DIGEST_LENGTH];
        SHA256_Init(&shactx);
        SHA256_Update(&shactx, &i, SHA256_DIGEST_LENGTH);
        SHA256_Final(digest, &shactx);
        unsigned char *newHash = digest;

        //Iterate through both hashes, to check if current iteration matches.
        int j;
        for (j = 0; j < SHA256_DIGEST_LENGTH; i++)
        {
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


    char buffer[49];
    //Start communicating, first read input from client
    bzero(buffer,49);
    read(newsockfd,buffer, 49);
    

    //Input from client
    Packet packet = {buffer};
    //Data skal parses rigtigt, vigtigt præcist format følges
    unsigned char *hashArr = buffer;
    uint64_t startR = be64toh(packet.start);
    printf("%lu", startR);
    uint64_t endR = be64toh(packet.end);
    printf("%lu", endR);

    uint8_t outBuff[8];
    bzero(outBuff, sizeof(outBuff)); 
    printf("%c", outBuff);

    uint64_t ans = reverse(startR, endR, hashArr);
    printf("%lu", ans);
    uint64_t ansR = htobe64(ans);
    printf("%lu", ansR);
    memcpy(outBuff, &ansR, (size_t) 8);
    
    //Then write a response
    write(newsockfd, outBuff, 8);

    return 0;
}
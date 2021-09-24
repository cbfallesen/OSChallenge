#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main (int argc, char *argv[]) {
    int clientSock, connection, server;
    struct sockaddr_in serverAddr;

    //Input parameters    
    char hostname;
    int port, seed, start;
    unsigned int total, difficulty, rep, lambda, delay;

    //Initializing input parameters
    hostname = argv[1];
    port = atoi(argv[2]);
    // seed = argv[3];
    // total = argv[4];
    // start = argv[5];
    // difficulty = argv[6];
    // rep = argv[7];
    // delay = argv[8];
    // lambda = argv[9];

    //Initializes the socket, using IPv4 adress, stream socket, and system default protocol.
    clientSock = socket(AF_INET, SOCK_STREAM, 0);
    
    if(clientSock < 0) {
        printf("Error connecting to socket.");
        return 1;
    }

    server = gethostbyname(hostname);
    if(server == NULL) {
        printf("No such hostname");
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = port;


    return 0;
}
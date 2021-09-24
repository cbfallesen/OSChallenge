#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main (int argc, char *argv[]) {
    int clientSock, connection;
    struct sockaddr_in serverAddr;
    struct hostent *server;

    //Input parameters    
    int port, seed, start;
    unsigned int total, difficulty, rep, lambda, delay;

    //Initializing input parameters
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

    server = gethostbyname(argv[1]);
    if(server == NULL) {
        printf("No such hostname");
        return 1;
    }

    bzero((char *) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serverAddr.sin_addr.s_addr, server->h_length);
    serverAddr.sin_port = port;

    //Connect to server
    connection = connect(clientSock, (struct sockAddr*) &serverAddr, sizeof(serverAddr));
    if(connection < 0) {
        printf("Error connecting to server");
        return 1;
    }

    return 0;
}
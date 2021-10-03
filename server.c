#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

int main (int argc, char *argv[]) {
    int serverSock, binder, portNum, accepting, clientLength;
    struct sockaddr_in serverAddr, clientAddr;

    //Comment
    // port = atoi(argv[1]);

    //Initialise server socket, using IPv4, stream socket and system default protocol.
    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    //If socket returns -1, there is an error.
    if (serverSock < 0) {
        printf("Error connecting to socket");
        exit(1);
    }

    //bzero?
    bzero((char * ) &serverAddr, sizeof(serverAddr));
    portNum = 5001;

    //Initialise the server address.
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = portNum;

    //Initialise the binder for the host address.
    binder = bind(serverSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    if (binder < 0) {
        printf("Error binding the host address");
        exit(1);
    }

    //Initialise listener.
    listen(serverSock, 5);

    clientLength = sizeof(clientAddr);
    accepting = accept(serverSock, (struct sockaddr *) &clientAddr, &clientLength);

    if (accepting < 0) {
        printf("Error accepting client address");
        exit(1);
    }

    char buffer[256];
    bzero(buffer,256);
    int n = read( serverSock,buffer,256 );
    
    if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
    }
    
    // for (int i = 0; i < sizeof(buffer); i++)
    // {
    //     printf(buffer[i]);
    // }
    
    //Respond
    n = write(serverSock,"I got your message",18);
   
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    return 0;
}
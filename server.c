#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
// #include <netinet/in.h>
#include <arpa/inet.h>

int main (int argc, char *argv[]) {
    int serverSock, binder, port, listener, accepting, clientLength;
    struct sockaddr_in serverAddr, clientAddr;

    //Comment
    port = atoi(argv[1]);

    //Initialise server socket, using IPv4, stream socket and system default protocol.
    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    //If socket returns -1, there is an error.
    if (serverSock < 0) {
        printf("Error connecting to socket");
        return 1;
    }

    //bzero?
    bzero((char * ) &serverAddr, sizeof(serverAddr));
    
    //Initialise the server address.
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = port;

    //Initialise the binder for the host address.
    binder = bind(serverSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    if (binder < 0) {
        printf("Error binding the host address");
        return 1;
    }

    //Initialise listener.
    listener = listen(serverSock, 5);

    clientLength = sizeof(clientAddr);
    accepting = accept(serverSock, (struct sockaddr *) &clientAddr, &clientLength);

    if (accepting < 0) {
        printf("Error accepting client address");
        return 1;
    }

    char buffer[49];
    bzero(buffer,49);
    int n = read( serverSock,buffer,48 );
    
    if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
    }
    
    for (int i = 0; i < sizeof(buffer); i++)
    {
        printf(buffer[i]);
    }
    

    return 0;
}
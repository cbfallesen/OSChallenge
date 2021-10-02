#include <stdio.h>
#include <stdlib.h>
<<<<<<< HEAD
=======
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main (int argc, char *argv[]) {
    int serverSock, binder, port, listener, accepting, clientLength;
    struct sockaddr_in serverAddr, clientAddr;

    //Comment
    // port = atoi(argv[1]);

    char *ip = "127.0.0.1";
    
    //Initialise server socket, using IPv4, stream socket and system default protocol.
    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    //If socket returns -1, there is an error.
    if (serverSock < 0) {
        printf("Error connecting to socket");
        return 1;
    }

    //bzero?
    bzero((char * ) &serverAddr, sizeof(serverAddr));
    port = 5151;
>>>>>>> parent of cbfcd71 (Binding host address to localhost)

#include <netdb.h>
#include <netinet/in.h>

<<<<<<< HEAD
#include <string.h>
=======
    //Initialise the server address.
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    // serverAddr.sin_addr.s_addr = inet_addr(ip);
    serverAddr.sin_port = htons(port);
    // serverAddr.sin_port = port;

    //Initialise the binder for the host address.
    binder = bind(serverSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    if (binder < 0) {
        printf("Error binding the host address");
        return 1;
    }

    //Initialise listener.
    listen(serverSock, 5);

    clientLength = sizeof(clientAddr);
    accepting = accept(serverSock, (struct sockaddr *) &clientAddr, &clientLength);
>>>>>>> parent of cbfcd71 (Binding host address to localhost)

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5001;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
    printf(inet_ntop(serv_addr.sin_addr));

   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
      
   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	
   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }
   
   /* If connection is established then start communicating */
   bzero(buffer,256);
   n = read( newsockfd,buffer,255 );
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
   
   printf("Here is the message: %s\n",buffer);
   
   /* Write a response to the client */
   n = write(newsockfd,"I got your message",18);
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
      
   return 0;
}
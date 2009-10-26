/**********************************************************************
 * quote_server.c --- Server portion of quote_client.  Listens and 
 						creates threads upon requests.
 *                     - change SERVER_PORT value to that of your server
 **********************************************************************/

/* Requisite includes */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define SERVER_PORT 5001 /* CHANGE THIS TO THE PORT OF YOUR SERVER */
#define BUFFER_SIZE 1024

/*

    *  Sockets
          o socket()
          o bind()
          o accept()
          o listen()
          o close()

    * Threads
          o pthread_create(pthread_exit()

    * File locking
          o flock()
          o funlock()
          o fcntl()

    * You can also use normal POSIX mutex or semaphores to deal with concurrency issues pertaining to the log file.
    * File handling
          o fopen()
          o fgets()
     
 */     


/**********************************************************************
 * functions
 **********************************************************************/

void handle_connection (int sock) {
int msgsock, rval;
int i;
char buf[512] ; 
    msgsock = accept(sock,0,0);
    for ( i = 0 ; i < 512 ; i++) buf[i] = ' ' ;   /* clear buf array  with spaces  */

    if ( (rval = read(msgsock, buf , 512)) < 0) {   /* read from msgsock into buf    */
         perror("while reading from socket");
          return;
     }
    write(1,buf,512);
    close(msgsock) ;  /* close socket -- connection is closed and socket is deallocated. */
}


/**********************************************************************
 * main
 **********************************************************************/

int main(int argc, char *argv[]) {

  int sock;                    /* fd for socket connection */
  struct sockaddr_in server;   /* Socket info. for server */
  struct hostent *host;        /* Server info */  
  char selection[BUFFER_SIZE]; /* User's input */
  char request[BUFFER_SIZE];   /* String rec'd from client */
  char response[BUFFER_SIZE];  /* String sent to client */
  int finished = 0;  

  if (argc != 2) {
    printf("Usage: %s address\n", argv[0]);
  }
     
  /* Fill out sockaddr struct */
  server.sin_family = AF_INET;
  if ((host = gethostbyname(argv[1])) == NULL){
    printf("%s: unknown host '%s'\n", argv[0], argv[1]);
    exit(1);
  }
  server.sin_family = host->h_addrtype;
  memcpy((char *) &server.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
  server.sin_port = htons((u_short) SERVER_PORT);
  
  /* Create socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    printf("%s: Unable to open socket\n",argv[0]);
    perror("Aborting");
    exit(1);
  }

  /* Try to connect */
  if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0){
    printf("%s: Unable to connect to %s\n", argv[0], argv[1]);
    perror("Aborting");
    exit(1);
  } else {
    printf("\nConnection to the quote server established\n");
  }
  
}  

void main( int argc,  char*  argv[] )
{
   int i, ready, sock1, sock2, length; 
   /* This receiver process will receive connection requests on these two sockets */ 
   int msgsock ; 

   fd_set fdin;
   struct timeval TO ; 

   struct sockaddr_in server, server2;  

   /* CREATE FIRST SOCKET 				   			*/
   sock1  = socket(AF_INET, SOCK_STREAM,0); 

   if ( sock1 < 0 ) { 
      perror("Problem in creating the first socket");
       exit(0); 
    } 

   server.sin_addr.s_addr = INADDR_ANY ; 
   server.sin_port = 0 ; 
   if ( bind(sock1,(struct sockaddr *)&server, sizeof(server))) {   /* Bind socket to a port */ 
      perror ( "Binding stream socket");
      exit(0);
   }

   length = sizeof(server); 
   if ( getsockname(sock1,(struct sockaddr *)&server, &length)) {  /* get the port number assigned to this socket */ 
        perror("Getting socket's port number");
        exit(0);
   }

   fprintf(stderr, "Socket 1 is connected to port%5d\n\n", ntohs(server.sin_port)); 

   /* CREATE SECOND SOCKET 				 			*/
   sock2 = socket(AF_INET, SOCK_STREAM, 0);
 
   if ( sock2 < 0 ) {
      perror("problem in creating the second  socket");
      exit(0);
   } 

   server2.sin_addr.s_addr = INADDR_ANY; 
   server2.sin_port = 0 ; 
   if ( bind(sock2, (struct sockaddr * )&server2,sizeof(server2))){ /* bind second socket to a port */
        perror("Binding stream socket");
        exit(0);
   }

   length = sizeof(server2);
   if ( getsockname(sock2,(struct sockaddr *)&server2,&length)){  /* get the port number for socket2 */
      perror("getting socket's port number");
      exit(0);
    }

    fprintf(stderr,"Socket 2  is connected to port %5d\n", ntohs(server2.sin_port));

    /* Initialize socket fo receiving messages.						*/
    /*  ready to accept connection to sock1, up to 3 requests can be kept buffered */
    listen(sock1,3);     
    listen(sock2,3);     

    /* Continuously wait for receiving messages on either of the two sockets.		*/
    while (1) { 
        FD_ZERO( &fdin );
        FD_SET( sock1, &fdin);
        FD_SET( sock2, &fdin);
        TO.tv_sec = 10 ; 
        TO.tv_usec = 0 ; 
  
        select(32, (fd_set *)&fdin, 0, 0, &TO);  /* Block to receive call on either sock1 or sock2 */
                       /* timeout and continue after 10 seconds if no connection requests */

        if (FD_ISSET(sock1, &fdin )) {   /* check if connection requests for sock1  */ 
             printf("\nMessage on the first socket\n");

             /* accept connection on sock1. This will return a new socket indentifier, stored 
             in msgsock for the connection. All I/O for that connection is through msgsock.  */
              handle_connection(sock1);
              FD_CLR(sock1, &fdin);
         }
         else if (FD_ISSET(sock2, &fdin)) {    /* check if connection requests for sock2  */
              printf("\nMessage on the second socket\n");
              /* accept connection on sock2. This will return a new socket indentifier, stored 
              in msgsock for the connection. All I/O for that connection is through msgsock.  */

              handle_connection(sock2);
              FD_CLR(sock2,  &fdin);
             } 
            else  /* Select call completed due to timeout and there is no connection on any socket */
                  printf("\nTimeout occured\n"); 
      };

}  

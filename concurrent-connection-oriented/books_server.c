#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "data.h"
#include "helpers.c"
#include "helpers.h"
# define PORT 3000
#define BUFFERSIZE 1024

void *handle_client(void *arg){
int sockFd= *(int *)arg;
struct sockaddr_in clientaddr; /* client addr */
int clientlen=sizeof(clientaddr);/* byte size of client's address */
ssize_t recv_len;
char buffer[BUFFERSIZE];


while(1){
    struct Data data;
     char *response;
    recv_len=recvfrom(sockFd,buffer,BUFFERSIZE,0,(struct sockaddr *)&clientaddr,&clientlen);
if(recv_len < 0){
      perror("recvfrom");
      break;
    }
  struct Data *incoming_data=(struct Data *)buffer;
    if (incoming_data->choice == 1)
    {
        // Display catalogue
        // TODO: Make displayCatalog() function return  type *char
       response=DisplayCatalog(//2,1,3
        incoming_data->m,
        incoming_data->X,
        incoming_data->z
      );
        //return "Catalogue displayed";
    }
    else if (incoming_data->choice == 2)
    {
        // Search for book
        printf("receiving search query: %s",incoming_data->search);
        response= SearchBook(incoming_data->search);
    }
    else if (incoming_data->choice == 3)
    {
        // Order a book
        // TODO: Implement orderBook() function
       // return "Book ordered";
      int orderno=OrderBook(incoming_data->x,incoming_data->y,incoming_data->n);
      sprintf(response,"%d",orderno);
      //PurchaseItem();
    }
    else if (incoming_data->choice == 4)
    {
        // Pay for book
        // TODO: Make payForItem() function return  type *char
       bool isSuccessful=PayForBook(incoming_data->orderno,incoming_data->amount);//PayForItem();
       // return "Payment successful";
      isSuccessful ? response="payment successful" : "payment failed";
    }
    else
    {
        return "Invalid option";
    }

  //response = "...received";
  sendto(sockFd, response, strlen(response), 0, (struct sockaddr *)&clientaddr,clientlen);
  }
    
  free(arg);//free socket descriptor mem
  
close(sockFd);
pthread_exit(NULL);
return NULL;
}

/*int main (){
//The server socket id 
  int sockFd , ret;
struct sockaddr_in serveraddr; server's addr 
struct sockaddr_in clientaddr;  client addr  
int clientlen;                 byte size of client's address  
int childfd;                    child socket 
struct hostent *hostp;          client host info 
char buf[BUFSIZE];              message buffer 
char *hostaddrp;                dotted decimal host addr string 
int n;                          message byte size 
// Child process id
    pid_t childpid;
    

    // Create socket
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0)
    {
        perror("Error opening socket");
        exit(1);
    }
    // Bind to well known address

    
    // build the server's Internet address
     
    bzero((char *)&serveraddr, sizeof(serveraddr));
    // this is an Internet address 
    serveraddr.sin_family = AF_INET;

    // let the system figure out our IP address 
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // this is the port we will listen on 
    serveraddr.sin_port = htons((unsigned short)PORT);

    
     // bind: associate the parent socket with a port
     
    if (bind(sockFd, (struct sockaddr *)&serveraddr,
             sizeof(serveraddr)) < 0)
    {
        perror("ERROR on binding");
        exit(2);
    }

    //
    // Place connection in listen mode
    //
    
     //listen: make this socket ready to accept connection requests
     
    if (listen(sockFd, 10) == 0) // allow 5 requests to queue up 
    {
        perror("ERROR on listen");
        exit(3);
    }
  int count=0;

    // Obtain a new socket for connection when a client connects
    //
    // main loop: wait for a connection request, echo input line,then close connection.
  
    clientlen = sizeof(clientaddr);
 while(1){
printf("server waiting for connection...\n");
     childfd = accept(sockFd, (struct sockaddr *)&clientaddr, &clientlen);
        if (childfd < 0) {
            perror("ERROR on accept");
            exit(4);
        }
printf("accepted connection from %s:%d\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));

printf("%d clients connected \n",count++);
if((childpid =fork())==0){
      close(sockFd);
    }
  }
}*/
int main (){
 // sockFd is the file descriptor for the socket 
  // client sock is a ptr to the dynaminc int  for the socket descriptor 
  int sockFd , *client_sock;
  //hold the server and client addressses. 
  struct sockaddr_in server_addr,clientaddr;
  //hold thread id
  pthread_t thread_id;
  //length of the client addr structure
 int clientlen = sizeof(clientaddr);
 //create the socket (TCP)
  sockFd=socket(AF_INET,SOCK_STREAM,0);
  if (sockFd==-1){
    perror("Failed to create the socket");
    exit(EXIT_FAILURE);
  }
 //setup the server address 
  server_addr.sin_family = AF_INET; //specify IPv4
  server_addr.sin_addr.s_addr=htonl(INADDR_ANY); //bind socket to all available interfaces
  server_addr.sin_port=htons(PORT); //convert port_no to network byte order

  //binding , associate the socket with the given addr and port 
  if(bind(sockFd,(struct sockaddr *)&server_addr , sizeof(server_addr))==-1){
    perror("Binding failed");
    close(sockFd);
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
    if (listen(sockFd, 10) == -1) {
        perror("Listen failed");
        close(sockFd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    // Accept and handle connections
  while(1){
    //dynamically allocate space for a client socket descriptor
      client_sock = malloc(sizeof(int));
        if (!client_sock) {
            perror("Malloc failed");
            continue;
        }
       //accept a new connection 
        *client_sock = accept(sockFd, (struct sockaddr *)&clientaddr, &clientlen);
        if (*client_sock == -1) {
            perror("Accept failed");
            free(client_sock);
            continue;
        }

        printf("A connection has been accepted from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        // Create a new thread for each client
        if (pthread_create(&thread_id, NULL, handle_client, (void *)client_sock) != 0) {
            perror("Unable to create the thread");
            close(*client_sock);
            free(client_sock);
        }

        // Detach the thread so that resources are released when it terminates
        pthread_detach(thread_id);
   
  }
  close(sockFd);
  return 0;
  
}


/*char *server_response(struct Data data)
{
    if (data.choice == 1)
    {
        // Display catalogue
        // TODO: Make displayCatalog() function return  type *char
        DisplayCatalog();
        return "Catalogue displayed";
    }
    else if (data.choice == 2)
    {
        // Search for book
        return searchInFile(data.search);
    }
    else if (data.choice == 3)
    {
        // Order a book
        // TODO: Implement orderBook() function
        return "Book ordered";
    }
    else if (data.choice == 4)
    {
        // Pay for book
        // TODO: Make payForItem() function return  type *char
        PayForItem();
        return "Payment successful";
    }
    else
    {
        return "Invalid option";
    }
}*/

#include "calculator.c"
#include "calculator.h"
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
    struct Calculator_data data;
     char *response;
    recv_len=recvfrom(sockFd,buffer,BUFFERSIZE,0,(struct sockaddr *)&clientaddr,&clientlen);
if(recv_len < 0){
      perror("recvfrom");
      break;
    }
  struct Calculator_data *incoming_data=(struct Calculator_data *)buffer;
    if (incoming_data->choice == 1)
    { //sum
     int result=add(incoming_data->a,incoming_data->b);
     sprintf(response,"%d",result);
    }
    else if (incoming_data->choice == 2)
     //multiply
    {
      int result=multiply(incoming_data->a,incoming_data->b);
      sprintf(response,"%d",result);
           
    }
    else if (incoming_data->choice == 3)
    {  //square root
       int result=square_root(incoming_data->a);
       sprintf(response,"%d",result);
           
    }
    else if (incoming_data->choice == 4)
    {  //square root
       int result=square(incoming_data->a);
       sprintf(response,"%d",result);
           
    }else
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




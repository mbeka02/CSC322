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
#include "../data.h"
#include "../interface.h"
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
        char* response = malloc(sizeof(char) * 100);
        recv_len=read(sockFd,buffer,BUFFERSIZE);
        if(recv_len < 0){
            perror("unable to read");
            break;
        }else if(recv_len == 0){
            printf("Client disconnected\n");
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
            if (orderno == -1) {
                printf("The book does not exist!\n");
                response = "The book does not exist\n";
            } else {
                sprintf(
                    response,
                    "Order No: %d: Number of books ordered: %d; Book title: %s; Book ISBN: %s",
                    orderno,
                    incoming_data->n,
                    incoming_data->x,
                    incoming_data->y
                );
            }
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
        //  sendto(sockFd, response, strlen(response), 0, (struct sockaddr *)&clientaddr,clientlen);
        write(sockFd,response,strlen(response));
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

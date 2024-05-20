
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "data.h"

#define PORT 3000
#define BUFFER_SIZE 1024

struct Data which_functionality();

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("error opening socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change to server's IP if needed
// Allow TCP to dynamically choose port client will use (so don't bind)
    
    // Connect socket to server

    // connect the client socket to server socket
    if (connect(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr))
        != 0) {
        perror("connection with the server failed...\n");
        exit(0);
    }

    while (1) {
        struct Data data = which_functionality();
        if (data.choice == 0) {
            printf("Invalid option. Exiting...\n");
            break;
        }
      
        // Send data to server
      //  sendto(sockfd, &data, sizeof(data), 0, (const struct sockaddr *)&server_addr, addr_len);
     write(sockfd,&data,sizeof(data));    
    // Receive data from server
        //ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
  ssize_t recv_len= read(sockfd,buffer,BUFFER_SIZE);   
    if (recv_len < 0) {
            perror("unable to read");
            break;
        }
        buffer[recv_len] = '\0';
        printf("Received from server: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}

/**
 * Function to display the functionality options
 * @return struct Data containing the user's choice and the parameters needed for the chosen functionality
 */
struct Data which_functionality() {
    struct Data data = {0}; // Initialize all fields to 0
    printf("Which functionality do you want to use?\n");
    printf("1. Display catalogue\n");
    printf("2. Search for a book\n");
    printf("3. Order book\n");
    printf("4. Pay for a book\n");
    printf("5. Exit\n");
    printf("Enter option: ");
    scanf("%d", &data.choice);
    switch (data.choice) {
        case 1:
            printf("***** Display catalogue *****\n");
            printf("Enter the maximum number of books to be displayed: ");
            scanf("%d", &data.m);
            printf("Enter the value of x:");
            scanf("%d",&data.X);
            printf("Enter the value of z:");
            scanf("%d",&data.z);
            break;
        case 2:
            printf("***** Search for book *****\n");
            printf("Enter the title of the book you want to search for: ");
            scanf("%s", data.search);
            break;
 
        case 3:
            printf("***** Order book *****\n");
            printf("Enter the number of books you want to order: ");
            
            scanf("%d", &data.n);
            printf("Enter the title:");
            scanf("%s",data.x);
            printf("Enter the ISBN Number:");
            scanf("%s",data.y);
            break;
        case 4:
            printf("***** Pay for book *****\n");
            printf("Enter the order number of the book you want to pay for: ");
            scanf("%d", &data.orderno);
            printf("Enter the amount you want to pay: ");
            scanf("%lf", &data.amount);
            break;
        
       case 5:
            //for now do the same as default case , the server should log out that the client has disconnected
            printf("***** Exiting *****\n");
            data.choice=0;
            break;
        default:
            printf("Invalid option\n");
            data.choice = 0;
            break;
    }
    return data;
}

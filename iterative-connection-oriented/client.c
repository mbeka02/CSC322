#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include "../interface.h"
#include "data_iter.h"
#define BUFSIZE 4096
struct Data which_functionality();
int main() {
    struct sockaddr_in servaddr, cli;
    // Define IP address of server you want to communicate with (target server)
    char * serverIPAddress = "127.0.0.1";
 
    // Define port number used by target server
    int serverPortNumber = 3000;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(serverIPAddress);
    servaddr.sin_port = htons(serverPortNumber);

    // Create socket
    int clientSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocketFD < 0)
    {
        perror("Error opening socket");
        exit(1);
    }

    // Allow TCP to dynamically choose port client will use
    
    // Connect socket to server

    // connect the client socket to server socket
    if (connect(clientSocketFD, (const struct sockaddr*)&servaddr, sizeof(servaddr))
        != 0) {
        perror("connection with the server failed...\n");
        exit(0);
    }

    bool stopLooping = false;
    char *message = malloc(sizeof(char) * 500);

    // Loop
    while(stopLooping == false) {
        // Get user input
         struct Data data = which_functionality();
        if (data.choice == 0)
        {
            perror("Invalid option");
            break;
        }
        // Send data to server
        printf("Connected to the server");
        char buffer_serve[BUFSIZE];
        memcpy(buffer_serve, &data, sizeof(data));


        // Send a request (use our defined message format)
        int isWritten = write(clientSocketFD, buffer_serve, sizeof(buffer_serve));

        if (isWritten == -1) {
            perror("Could Not Send Data\n");
            exit(2);
        }

        // Await response
        // Create buffer to hold server response
        char buffer[BUFSIZE];
        // Get response
        bzero(buffer, BUFSIZE); 
        // read the message from client and copy it in buffer 
        read(clientSocketFD, buffer, sizeof(buffer));
        // Print Response
        printf("\nServer responded with %s\n", buffer);
    }

    // Close the connection
    close(clientSocketFD);
    printf("Client socket closed\n"); 
}


/**
 * Function to display the functionality options
 * @return struct Data containg the user's choice and the parameters needed for the chosen functionality
 */
struct Data which_functionality()
{
    struct Data data;
    printf("which functionality do you want to use?\n");
    printf("1. Display catalogue");
    printf("2. Pay for book");
    printf("3. Order book");
    printf("search for book");
    printf("Order a book");
    printf("Enter message: ");
    scanf("%d", &data.choice);
    switch (data.choice)
    {
    case 1:
        printf("*****Display catalogue*****\n");
        printf("Enter the maximum bnumber of books to be displayed: ");
        scanf("%d", &data.m);
        printf("Enter value for x: ");
        scanf("%d", &data.x);
        printf("Enter value for y: ");
        scanf("%d", &data.z);
        return data;
    case 2:
        printf("*****Pay for book*****\n");
        printf("Enter the order number of the book you want to pay for: ");
        scanf("%d", &data.orderno);
        printf("Enter the amount you want to pay: ");
        scanf("%lf", &data.amount);

        return data;
    // case 3:
    //     printf("*****Order book*****\n");
    //     printf("Enter the number of books you want to order: ");
    //     scanf("%d", &data.number_ordered);
    //     return data;
    case 4:
        printf("*****search for book*****\n");
        printf("Enter the title of the book you want to search for: ");
        scanf("%s", data.search);
        return data;
    case 5:
        printf("*****Order a book*****\n");
        printf("Enter the title of the book you want to order");
        scanf("%s", data.y);
        printf("Enter the value for n:");
        scanf("%s", data.n);
        printf("Enter the number of books you want to order: ");
        scanf("%d", &data.number_ordered);

        return data;
    default:
        printf("Invalid option\n");
        data.choice = 0;
        return data;
    }
}

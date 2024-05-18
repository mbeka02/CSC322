
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        sendto(sockfd, &data, sizeof(data), 0, (const struct sockaddr *)&server_addr, addr_len);
        // Receive data from server
        ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
        if (recv_len < 0) {
            perror("recvfrom");
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
    printf("2. Pay for book\n");
    printf("3. Order book\n");
    printf("4. Search for book\n");
    printf("5. Order a book\n");
    printf("Enter option: ");
    scanf("%d", &data.choice);
    switch (data.choice) {
        case 1:
            printf("***** Display catalogue *****\n");
            printf("Enter the maximum number of books to be displayed: ");
            scanf("%d", &data.m);
            break;
        case 2:
            printf("***** Pay for book *****\n");
            printf("Enter the order number of the book you want to pay for: ");
            scanf("%d", &data.orderno);
            printf("Enter the amount you want to pay: ");
            scanf("%lf", &data.amount);
            break;
        case 3:
            printf("***** Order book *****\n");
            printf("Enter the number of books you want to order: ");
            scanf("%d", &data.number_ordered);
            break;
        case 4:
            printf("***** Search for book *****\n");
            printf("Enter the title of the book you want to search for: ");
            scanf("%s", data.search);
            break;
        case 5:
            printf("***** Order a book *****\n");
            printf("Enter the title of the book you want to order: ");
            scanf("%s", data.y);
            break;
        default:
            printf("Invalid option\n");
            data.choice = 0;
            break;
    }
    return data;
}

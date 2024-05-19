
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "data.h"
#include "calculator.h"

#define PORT 5173
#define BUFFER_SIZE 1024

struct Calculator_data which_functionality();

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
        struct Calculator_data data = which_functionality();
        if (data.choice == 0) {
            printf("Invalid option. Exiting...\n");
            break;
        }
        // Send data to server
//        sendto(sockfd, &data, sizeof(data), 0, (const struct sockaddr *)&server_addr, addr_len);
       write(sockfd,&data,sizeof(data));    

    // Receive data from server
    //    ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
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
struct Calculator_data which_functionality() {
    struct Calculator_data data = {0}; // Initialize all fields to 0
    printf("Which functionality do you want to use?\n");
    printf("1. Add two numbers\n");
    printf("2. Multiply two Numbers\n");
    printf("3. Get the square root\n");
    printf("4. Get the square\n");
    printf("5. Exit (NOT IMPLEMENTED YET)\n");
    printf("Enter option: ");
    scanf("%d", &data.choice);
    switch (data.choice) {
        case 1:
            printf("***** Add two numbers *****\n");
           printf("Enter the first number:");
            scanf("%d",&data.a);
            printf("Enter the second number:");
            scanf("%d",&data.b);
            printf("%d",data.a);
            break;
        case 2:
            printf("***** Multiply two numbers *****\n");
            printf("Enter the first number:");
            scanf("%d",&data.a);
            printf("Enter the second number:");
            scanf("%d",&data.b);
            break;

        case 3:
            printf("***** Get the square root *****\n");
            printf("Enter the  number:");
            scanf("%d",&data.a);
            break;

        case 4:
            printf("***** Get the square *****\n");
            printf("Enter the  number:");
            scanf("%d",&data.a);
            break;
        default:
            printf("Invalid option\n");
            data.choice = 0;
            break;
    }
    return data;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <data_conn.h>
#define PORT 8989
#define BUFFER_SIZE 1024


//function prototypes
struct Data which_functionality();
int main()
{   
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change to server's IP if needed

    while (1)
    {
        struct Data data = which_functionality();
        if (data.choice == 0)
        {
            perror("Invalid option");
            break;
        }
        // Send data to server
        printf("Connected to the server");
        char buffer[sizeof(data)];
        memcpy(buffer, &data, sizeof(data));
        sendto(sockfd, buffer, sizeof(buffer), 0, (const struct sockaddr *)&server_addr, addr_len);
        // Receive data from server
        printf("Awaiting server response...\n");
        ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
        if (recv_len < 0)
        {
            perror("recvfrom");
            break;
        }
        buffer[recv_len] = '\0';
        printf("Received from server: %s\n",buffer);
    }

    close(sockfd);
    return 0;
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <calcdata.h>
#define PORT 8989
#define BUFFER_SIZE 1024


//function prototypes
struct Calc_data which_functionality();

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
        printf("Connected to the server\n");
        struct Calc_data data = which_functionality();
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

struct Calc_data which_functionality()
{
    struct Calc_data data;
    printf("Enter the operation you want to perform\n");
    printf("1. Add\n");
    printf("2. Multiply\n");
    printf("3. Square root\n");
    printf("4. Square\n");
    printf("0. Exit\n");
    scanf("%d", &data.choice);
    switch (data.choice)
    {
    case 1:
        printf("Enter the first number: ");
        scanf("%d", &data.sum_1);
        printf("Enter the second number: ");
        scanf("%d", &data.sum_2);
        break;
    case 2:
        printf("Enter the first number: ");
        scanf("%d", &data.mult_1);
        printf("Enter the second number: ");
        scanf("%d", &data.mult_2);
        break;
    case 3:
        printf("Enter the number: ");
        scanf("%d", &data.sqrt_value);
        break;
    case 4:
        printf("Enter the number: ");
        scanf("%d", &data.square_value);
        break;
    default:
        data.choice = 0;
        break;
    }
    return data;
}

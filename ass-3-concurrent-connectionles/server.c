#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <data.h>
#include <helpers.h>
#define PORT 8080
#define BUFFER_SIZE 1024

// Create a udp socket
// Bind the socket to an address and port
// Receive messages from clients
// Send responses back to clients

// for concurrency, use threads

void *handle_client(void *arg)
{
    int sockfd = *(int *)arg;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    ssize_t recv_len;

    // receiving messages from clients
    while (1)
    {
        struct Data data;

        recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (recv_len < 0)
        {
            perror("recvfrom");
            break;
        }
        struct Data *received_data = (struct Data *)buffer;
        // Call the appropriate function based on the choice

        // buffer[recv_len] = '\0';
        // printf("Received from client: %s\n", buffer);

        // Responding to the client
        char *response = "Message received";
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    close(sockfd);
    pthread_exit(NULL);
}

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d\n", PORT);

    pthread_t thread_id;
    while (1)
    {
        if (pthread_create(&thread_id, NULL, handle_client, (void *)&sockfd) != 0)
        {
            perror("pthread_create");
        }
        pthread_detach(thread_id);
    }

    close(sockfd);
    return 0;
}

/**
 * Function to handle the client's request
 *
 */
char *server_response(struct Data data)
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
}
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../data.h"
#include "../interface.h"

#define PORT 3000
#define BUFFERSIZE 1024

void handle_client(int sockFd, struct sockaddr_in *clientaddr) {
    char buffer[BUFFERSIZE];
    ssize_t recv_len;
    socklen_t addr_len = sizeof(*clientaddr);
    int first_message = 1;

    while (1) {
        recv_len = recvfrom(sockFd, buffer, BUFFERSIZE, 0, (struct sockaddr *)clientaddr, &addr_len);
        if (recv_len < 0) {
            perror("unable to read");
            break;
        }

        // Display message when a client sends the first request
        if (first_message) {
            printf("A connection has been received from %s:%d\n", inet_ntoa(clientaddr->sin_addr), ntohs(clientaddr->sin_port));
            first_message = 0;
        }

        struct Data *incoming_data = (struct Data *)buffer;
        char response[5000] = {0};

        if (incoming_data->choice == 1) {
            DisplayCatalog(incoming_data->m, incoming_data->X, incoming_data->z, response);
        } else if (incoming_data->choice == 2) {
            char *search_result = SearchBook(incoming_data->search);
            snprintf(response, sizeof(response), "%s", search_result);
            if (search_result != defaultSearchStringResult) {
                free(search_result);
            }
        } else if (incoming_data->choice == 3) {
            int orderno = OrderBook(incoming_data->x, incoming_data->y, incoming_data->n);
            if (orderno == -1) {
                snprintf(response, sizeof(response), "Order Failed! Book %s does not exist", incoming_data->x);
            } else {
                snprintf(response, sizeof(response), "Order successful! Order No: %d", orderno);
            }
        } else if (incoming_data->choice == 4) {
            int total = PayForBook(incoming_data->orderno, incoming_data->amount);
            if (total < 0) {
                snprintf(response, sizeof(response), "Transaction failed! Book does not exist or cost more than sent amount");
            } else {
                snprintf(response, sizeof(response), "Transaction successful! Books will arrive in 2 business days");
            }
        } else {
            snprintf(response, sizeof(response), "Invalid option");
        }

        sendto(sockFd, response, strlen(response), 0, (struct sockaddr *)clientaddr, addr_len);
    }
}

int main() {
    int sockFd;
    struct sockaddr_in server_addr, clientaddr;

    sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockFd == -1) {
        perror("Failed to create the socket");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(sockFd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        close(sockFd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        handle_client(sockFd, &clientaddr);
    }

    close(sockFd);
    return 0;
}

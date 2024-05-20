#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "data.h"
#include "helpers.c"
#include "helpers.h"

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
        char response[BUFFERSIZE] = {0};

        if (incoming_data->choice == 1) {
            snprintf(response, BUFFERSIZE, "%s", DisplayCatalog(incoming_data->m, incoming_data->X, incoming_data->z));
        } else if (incoming_data->choice == 2) {
            snprintf(response, BUFFERSIZE, "%s", SearchBook(incoming_data->search));
        } else if (incoming_data->choice == 3) {
            int orderno = OrderBook(incoming_data->x, incoming_data->y, incoming_data->n);
            snprintf(response, BUFFERSIZE, "%d", orderno);
        } else if (incoming_data->choice == 4) {
            bool isSuccessful = PayForBook(incoming_data->orderno, incoming_data->amount);
            snprintf(response, BUFFERSIZE, "%s", isSuccessful ? "payment successful" : "payment failed");
        } else {
            snprintf(response, BUFFERSIZE, "Invalid option");
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

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
#define BUFFER_SIZE 1024

void handle_request(int sockfd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    ssize_t recv_len;

    while (1) {
        recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0) {
            perror("Unable to read");
            continue;
        }

        struct Data *incoming_data = (struct Data *)buffer;
        char response[BUFFER_SIZE];

        if (incoming_data->choice == 1) {
            // Display catalogue
            char *catalog = DisplayCatalog(incoming_data->m, incoming_data->X, incoming_data->z);
            snprintf(response, BUFFER_SIZE, "Catalogue: %s", catalog);
            free(catalog);
        } else if (incoming_data->choice == 2) {
            // Search for book
            printf("Receiving search query: %s\n", incoming_data->search);
            char *search_result = SearchBook(incoming_data->search);
            snprintf(response, BUFFER_SIZE, "Search result: %s", search_result);
            free(search_result);
        } else if (incoming_data->choice == 3) {
            // Order a book
            int order_no = OrderBook(incoming_data->x, incoming_data->y, incoming_data->n);
            snprintf(response, BUFFER_SIZE, "Order number: %d", order_no);
        } else if (incoming_data->choice == 4) {
            // Pay for book
            bool is_successful = PayForBook(incoming_data->orderno, incoming_data->amount);
            snprintf(response, BUFFER_SIZE, "Payment %s", is_successful ? "successful" : "failed");
        } else {
            snprintf(response, BUFFER_SIZE, "Invalid option");
        }

        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, client_len);
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    handle_request(sockfd);

    close(sockfd);
    return 0;
}

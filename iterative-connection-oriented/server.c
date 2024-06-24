#include <asm-generic/socket.h>
#include <stdio.h>
#include <strings.h>
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

#define BUFFERSIZE 1024

void handle_client(int sockFd) {
    struct sockaddr_in clientaddr; /* client addr */
    int clientlen = sizeof(clientaddr); /* byte size of client's address */
    ssize_t recv_len;
    char buffer[BUFFERSIZE];

    while (1) {
        struct Data data;
        char* response = malloc(sizeof(char) * 5000);
        recv_len = read(sockFd, buffer, BUFFERSIZE);
        if (recv_len < 0) {
            perror("unable to read");
            break;
        } else if (recv_len == 0) {
            printf("\n...client disconnected\n");
            break;
        }
        struct Data *incoming_data = (struct Data *)buffer;
        if (incoming_data->choice == 1) {
            // Display catalogue
            DisplayCatalog(
                incoming_data->m,
                incoming_data->X,
                incoming_data->z,
                response
            );
        } else if (incoming_data->choice == 2) {
            // Search for book
            printf("receiving search query: %s", incoming_data->search);
            response = SearchBook(incoming_data->search);
        } else if (incoming_data->choice == 3) {
            // Order a book
            int orderno = OrderBook(incoming_data->x, incoming_data->y, incoming_data->n);
            if (orderno == -1) {
                sprintf(response, "Order Failed! Book %s does not exist", incoming_data->x);
            } else if (orderno == -2) {
                response = "Order Failed! Internal Server Error";
            } else {
                sprintf(response, "Order successful! Order No: %d", orderno);
            }
        } else if (incoming_data->choice == 4) {
            // Pay for book
            int total = PayForBook(incoming_data->orderno, incoming_data->amount);
            if (total > incoming_data->amount) {
                response = "Transaction failed! Books cost more than sent amount";
            } else {
                response = "Transaction successful! Books will arrive in 2 business days";
            }
        } else {
            response = "Invalid option";
        }

        write(sockFd, response, strlen(response));
        strcpy(response, "");
        free(response);
    }

    close(sockFd);
}

int main(int argc, char const *argv[]) {
    // Get Server Port
    if (argc != 2) {
        fprintf(stderr, "(Failed) usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int SERVER_PORT = atoi(argv[1]);

    int sockFd, client_sock;
    struct sockaddr_in server_addr, clientaddr;
    int clientlen = sizeof(clientaddr);

    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd == -1) {
        perror("Failed to create the socket");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(sockFd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        close(sockFd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockFd, 10) == -1) {
        perror("Listen failed");
        close(sockFd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", SERVER_PORT);

    while (1) {
        client_sock = accept(sockFd, (struct sockaddr *)&clientaddr, &clientlen);
        if (client_sock == -1) {
            perror("Accept failed");
            continue;
        }

        printf("A connection has been accepted from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        handle_client(client_sock);
    }

    close(sockFd);
    return 0;
}

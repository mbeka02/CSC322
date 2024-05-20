
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "helpers.c"  
//#include "helpers.h"
#include "calculator.h"
#include "calculator.c"
#define PORT 5173
#define BUFFERSIZE 1024

void *handle_client(void *arg) {
    int sockFd = *(int *)arg;
    free(arg);
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    ssize_t recv_len;
    char buffer[BUFFERSIZE];
    char response[BUFFERSIZE];

    while (1) {
        struct Calculator_data data;
        recv_len = read(sockFd, buffer, BUFFERSIZE);

        if (recv_len < 0) {
            perror("Unable to read");
            break;
        } else if (recv_len == 0) {
            printf("Client disconnected\n");
            break;
        }

        struct Calculator_data *incoming_data = (struct Calculator_data *)buffer;
        int result;

        switch (incoming_data->choice) {
            case 1:  // Sum
                result = add(incoming_data->a, incoming_data->b);
                sprintf(response, "%d", result);
                break;
            case 2:  // Multiply
                result = multiply(incoming_data->a, incoming_data->b);
                sprintf(response, "%d", result);
                break;
            case 3:  // Square root
                result = square_root(incoming_data->a);
                sprintf(response, "%d", result);
                break;
            case 4:  // Square
                result = square(incoming_data->a);
                sprintf(response, "%d", result);
                break;
            default:
                snprintf(response, sizeof(response), "Invalid option");
                break;
        }

        write(sockFd, response, strlen(response));
    }

    close(sockFd);
    pthread_exit(NULL);
    return NULL;
}

int main() {
    int sockFd, *client_sock;
    struct sockaddr_in server_addr, clientaddr;
    pthread_t thread_id;
    socklen_t clientlen = sizeof(clientaddr);

    sockFd = socket(AF_INET, SOCK_STREAM, 0);
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

    if (listen(sockFd, 10) == -1) {
        perror("Listen failed");
        close(sockFd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        client_sock = malloc(sizeof(int));
        if (!client_sock) {
            perror("Malloc failed");
            continue;
        }

        *client_sock = accept(sockFd, (struct sockaddr *)&clientaddr, &clientlen);
        if (*client_sock == -1) {
            perror("Accept failed");
            free(client_sock);
            continue;
        }

        printf("A connection has been accepted from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        if (pthread_create(&thread_id, NULL, handle_client, (void *)client_sock) != 0) {
            perror("Unable to create the thread");
            close(*client_sock);
            free(client_sock);
        }

        pthread_detach(thread_id);
    }

    close(sockFd);
    return 0;
}

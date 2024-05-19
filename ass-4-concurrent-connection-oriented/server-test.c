
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to handle client communication
void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    free(arg); // Free the dynamically allocated memory for the socket descriptor
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = read(client_sock, buffer, BUFFER_SIZE)) > 0) {
        printf("Received: %s", buffer);
        write(client_sock, buffer, bytes_read); // Echo back the received message
    }

    close(client_sock);
    return NULL;
}

int main() {
    int server_sock, *client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t thread_id;

    // Create a socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to an address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 10) == -1) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    // Accept and handle connections
    while (1) {
        client_sock = malloc(sizeof(int));
        if (!client_sock) {
            perror("Malloc failed");
            continue;
        }

        *client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (*client_sock == -1) {
            perror("Accept failed");
            free(client_sock);
            continue;
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Create a new thread for each client
        if (pthread_create(&thread_id, NULL, handle_client, (void *)client_sock) != 0) {
            perror("Thread creation failed");
            close(*client_sock);
            free(client_sock);
        }

        // Detach the thread so that resources are released when it terminates
        pthread_detach(thread_id);
    }

    // Close the server socket
    close(server_sock);
    return 0;
}

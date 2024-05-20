#include <sys/types.h>
#include <signal.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#define SERVER_PORT  8989
#define BUFSIZE  4096
#define SOCKETERROR  (-1)

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

// Function prototypes
void handle_connection(int, SA_IN*);
int check(int exp, const char *msg);
int setup_server(short port);

int main(int argc, char **argv) {
    int server_socket = setup_server(SERVER_PORT);

    while (true) {
        printf("Waiting for connections...\n");
        SA_IN client_addr;
        socklen_t addr_len = sizeof(SA_IN);
        char buffer[BUFSIZE];

        // Receive a message from a client
        ssize_t bytes_received = recvfrom(server_socket, buffer, BUFSIZE, 0, (SA*)&client_addr, &addr_len);
        check(bytes_received, "recvfrom error");

        buffer[bytes_received] = '\0'; // Null terminate the received data
        printf("Received message: %s\n", buffer);

        // Handle the received message
        handle_connection(server_socket, &client_addr);
    }

    return EXIT_SUCCESS;
}

int setup_server(short port) {
    int server_socket;
    SA_IN server_addr;

    check((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0, "Failed to create socket");

    // Initialize the address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)), "Bind failed");

    return server_socket;
}

int check(int exp, const char *msg) {
    if (exp == SOCKETERROR) {
        perror(msg);
        exit(1);
    }
    return exp;
}

void handle_connection(int server_socket, SA_IN* client_addr) {
    char buffer[BUFSIZE];
    size_t bytes_read;
    socklen_t addr_len = sizeof(SA_IN);
    char actualpath[PATH_MAX + 1];

    // Receive the file name from the client
    ssize_t bytes_received = recvfrom(server_socket, buffer, BUFSIZE, 0, (SA*)client_addr, &addr_len);
    check(bytes_received, "recvfrom error");

    buffer[bytes_received] = '\0'; // Null terminate the received data
    printf("REQUEST: %s\n", buffer);
    fflush(stdout);

    // Validity check
    if (realpath(buffer, actualpath) == NULL) {
        printf("ERROR(Bad path): %s\n", buffer);
        return;
    }

    // Read file and send its content to the client
    FILE *fp = fopen(actualpath, "r");
    if (fp == NULL) {
        printf("ERROR(open): %s\n", buffer);
        return;
    }

    // Read file and send its content to the client
    while ((bytes_read = fread(buffer, 1, BUFSIZE, fp))) {
        printf("Sending %zu bytes\n", bytes_read);
        sendto(server_socket, buffer, bytes_read, 0, (SA*)client_addr, addr_len);
    }
    fclose(fp);
    printf("Closing connection\n");
}

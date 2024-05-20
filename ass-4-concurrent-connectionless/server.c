
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
#include <math.h>
#include "calculator.h"
#include "calcdata.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#define SERVER_PORT 8989
#define BUFSIZE 4096
#define SOCKETERROR (-1)

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

// Function prototypes
void handle_connection(int, SA_IN *);
int check(int exp, const char *msg);
int setup_server(short port);
int server_response(struct Calc_data data);
int main(int argc, char **argv)
{
    int server_socket = setup_server(SERVER_PORT);

    while (true)
    {
        printf("Waiting for the connections...\n");
        SA_IN client_addr;
        socklen_t addr_len = sizeof(SA_IN);
        char buffer[BUFSIZE];
        // Handle the received message
        handle_connection(server_socket, &client_addr);
    }

    return EXIT_SUCCESS;
}

int setup_server(short port)
{
    int server_socket;
    SA_IN server_addr;

    check((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0, "Failed to create socket");

    // Initialize the address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(SERVER_PORT);

    check(bind(server_socket, (SA *)&server_addr, sizeof(server_addr)), "Bind failed");

    return server_socket;
}

int check(int exp, const char *msg)
{
    if (exp == SOCKETERROR)
    {
        perror(msg);
        exit(1);
    }
    return exp;
}

void handle_connection(int server_socket, SA_IN *client_addr)
{
    char buffer[BUFSIZE];
    size_t bytes_read;
    socklen_t addr_len = sizeof(SA_IN);
    char actualpath[PATH_MAX + 1];
    printf("Handling connection\n");
    // Receive the file name from the client
    ssize_t bytes_received = recvfrom(server_socket, buffer, BUFSIZE, 0, (SA *)client_addr, &addr_len);
    struct Calc_data data;
    memcpy(&data, buffer, sizeof(data));
    printf("Received message: %d\n", data.choice);
    struct Calc_data *received_data = (struct Calc_data *)buffer;
    int response = server_response(*received_data);
    printf("RESPONSE: %d\n", response); // Fix printf format specifier to print an integer
    printf("RESPONSE: %d\n", response); // Fix printf format specifier to print an integer

    check(bytes_received, "recvfrom error");

    buffer[bytes_received] = '\0'; // Null terminate the received data
    printf("REQUEST: %s\n", buffer);
    fflush(stdout);

    sendto(server_socket, &response, sizeof(response), 0, (SA *)client_addr, addr_len); // Fix sendto function call by passing the correct length of the response string

    // fclose(fp)
    printf("Closing connection\n");
}

int server_response(struct Calc_data data)
{
    int response;
    switch (data.choice)
    {
    case 1:
        return add(data.sum_1, data.sum_2);
        break;
    case 2:
        return multiply(data.mult_1, data.mult_2);
        break;
    case 3:
        response = square(data.square_value);
        return response;
        break;
    case 4:
        response = sqrt(data.sqrt_value);
        return response;
        break;
    default:
        printf("Invalid choice\n");
        data.choice = -1;
        return data.choice;
        break;
    }
}

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
#include "data_conn.h"
#include "helpers.c"

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
char *server_response(struct Data data);
int main(int argc, char **argv)
{
    int server_socket = setup_server(SERVER_PORT);

    while (true)
    {
        printf("Waiting for  the connections...\n");
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

    int optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    // Initialize the address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    // Print the address the server is running on
    char server_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server_addr.sin_addr), server_ip, INET_ADDRSTRLEN);
    printf("Server is running on %s:%d\n", server_ip, ntohs(server_addr.sin_port));
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
    struct Data data;
    memcpy(&data, buffer, sizeof(data));
    printf("Received message: %d\n", data.choice);
    struct Data *received_data = (struct Data *)buffer;
    char *response = server_response(*received_data);
    printf("RESPONSE: %s\n", response);
    printf("RESPONSE: %s\n", response);

    check(bytes_received, "recvfrom error");

    buffer[bytes_received] = '\0'; // Null terminate the received data
    printf("REQUEST: %s\n", buffer);
    fflush(stdout);

    sendto(server_socket, response, strlen(response), 0, (SA *)client_addr, addr_len);

    // fclose(fp);
    printf("Closing connection\n");
}

// struct Data *received_data = (struct Data *)buffer;

/**
 * Function to handle the client's request
 *
 */
char *server_response(struct Data data)
{
    printf("In server response\n");
    // TODO:
    if (data.choice == 1)
    {
        // Display catalogue
        // TODO: Make displayCatalog() function return  type *char
        
        printf("Catalogue displayed\n");
        return DisplayCatalog(data.m, data.x, data.z);
    }
    else if (data.choice == 4)
    {
        // Search for book
        return searchInFile(data.search);
    }
    else if (data.choice == 5)
    {
        // Order a book
        // TODO: Implement orderBook() function
        int order_no = OrderBook(data.y, data.n, data.number_ordered);
        return "Book ordered";
    }
    else if (data.choice == 2)
    {
        // Pay for book
        // TODO: Make payForItem() function return  type *char
        PayForItem(data);
        return "Payment successful";
    }
    else
    {
        return "Invalid option";
    }
}

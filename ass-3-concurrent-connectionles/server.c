#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "data_conn.h"
#include "helpers.c"
#include "helpers.h"

#define PORT 3000
#define BUFFERSIZE 1024

struct ClientRequest {
    struct sockaddr_in clientaddr;
    char buffer[BUFFERSIZE];
    int sockfd;
    socklen_t addr_len;
};

void *handle_client(void *arg) {
    struct ClientRequest *request = (struct ClientRequest *)arg;
    struct Data *incoming_data = (struct Data *)request->buffer;
    char response[BUFFERSIZE];

    if (incoming_data->choice == 1) {
        // Display catalogue
        snprintf(response, BUFFERSIZE, "%s", DisplayCatalog(
            incoming_data->m,
            incoming_data->X,
            incoming_data->z
        ));
    } else if (incoming_data->choice == 2) {
        // Search for book
        snprintf(response, BUFFERSIZE, "%s", SearchBook(incoming_data->search));
    } else if (incoming_data->choice == 3) {
        // Order a book
        int orderno = OrderBook(incoming_data->x, incoming_data->y, incoming_data->n);
        snprintf(response, BUFFERSIZE, "%d", orderno);
    } else if (incoming_data->choice == 4) {
        // Pay for book
        bool isSuccessful = PayForBook(incoming_data->orderno, incoming_data->amount);
        snprintf(response, BUFFERSIZE, "%s", isSuccessful ? "Payment successful" : "Payment failed");
    } else {
        snprintf(response, BUFFERSIZE, "Invalid option");
    }

    sendto(request->sockfd, response, strlen(response), 0, (struct sockaddr *)&request->clientaddr, request->addr_len);
    free(request);
    pthread_exit(NULL);
    return NULL;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, clientaddr;
    char buffer[BUFFERSIZE];
    socklen_t addr_len = sizeof(clientaddr);
    ssize_t recv_len;
    pthread_t thread_id;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    // Setup the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    // Main loop: handle incoming datagrams
    while (1) {
        recv_len = recvfrom(sockfd, buffer, BUFFERSIZE, 0, (struct sockaddr *)&clientaddr, &addr_len);
        if (recv_len < 0) {
            perror("Error receiving data");
            continue;
        }
        buffer[recv_len] = '\0';
        printf("Received packet from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        // Allocate memory for the request and copy data
        struct ClientRequest *request = (struct ClientRequest *)malloc(sizeof(struct ClientRequest));
        if (!request) {
            perror("Memory allocation failed");
            continue;
        }
        request->clientaddr = clientaddr;
        memcpy(request->buffer, buffer, BUFFERSIZE);
        request->sockfd = sockfd;
        request->addr_len = addr_len;

        // Create a new thread to handle the client request
        if (pthread_create(&thread_id, NULL, handle_client, (void *)request) != 0) {
            perror("Unable to create thread");
            free(request);
            continue;
        }

        // Detach the thread so that resources are released when it terminates
        pthread_detach(thread_id);
    }

    close(sockfd);
    return 0;
}

// #include <sys/types.h>
// #include <signal.h>
// #include <stdarg.h>
// #include <errno.h>
// #include <fcntl.h>
// #include <sys/time.h>
// #include <sys/ioctl.h>
// #include <netdb.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <stdbool.h>
// #include <limits.h>
// #include "data_conn.h"
// #include "helpers.c"

// #ifndef PATH_MAX
// #define PATH_MAX 4096
// #endif
// #define SERVER_PORT 8989
// #define BUFSIZE 4096
// #define SOCKETERROR (-1)

// typedef struct sockaddr_in SA_IN;
// typedef struct sockaddr SA;

// // Function prototypes
// void handle_connection(int, SA_IN *);
// int check(int exp, const char *msg);
// int setup_server(short port);
// char *server_response(struct Data data);
// int main(int argc, char **argv)
// {
//     int server_socket = setup_server(SERVER_PORT);

//     while (true)
//     {
//         printf("Waiting for  the connections...\n");
//         SA_IN client_addr;
//         socklen_t addr_len = sizeof(SA_IN);
//         char buffer[BUFSIZE];

//         // Handle the received message
//         handle_connection(server_socket, &client_addr);
//     }

//     return EXIT_SUCCESS;
// }

// int setup_server(short port)
// {
//     int server_socket;
//     SA_IN server_addr;

//     check((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0, "Failed to create socket");

//     // Initialize the address struct
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//     server_addr.sin_port = htons(SERVER_PORT);
//     // Print the address the server is running on
//     char server_ip[INET_ADDRSTRLEN];
//     inet_ntop(AF_INET, &(server_addr.sin_addr), server_ip, INET_ADDRSTRLEN);
//     printf("Server is running on %s:%d\n", server_ip, ntohs(server_addr.sin_port));
//     check(bind(server_socket, (SA *)&server_addr, sizeof(server_addr)), "Bind failed");

//     return server_socket;
// }

// int check(int exp, const char *msg)
// {
//     if (exp == SOCKETERROR)
//     {
//         perror(msg);
//         exit(1);
//     }
//     return exp;
// }

// void handle_connection(int server_socket, SA_IN *client_addr)
// {
//     char buffer[BUFSIZE];
//     size_t bytes_read;
//     socklen_t addr_len = sizeof(SA_IN);
//     char actualpath[PATH_MAX + 1];
//     printf("Handling connection\n");
//     // Receive the file name from the client
//     ssize_t bytes_received = recvfrom(server_socket, buffer, BUFSIZE, 0, (SA *)client_addr, &addr_len);
//     struct Data data;
//     memcpy(&data, buffer, sizeof(data));
//     printf("Received message: %d\n", data.choice);
//     struct Data *received_data = (struct Data *)buffer;
//     char *response = server_response(*received_data);
//     printf("RESPONSE: %s\n", response);
//     printf("RESPONSE: %s\n", response);

//     check(bytes_received, "recvfrom error");

//     buffer[bytes_received] = '\0'; // Null terminate the received data
//     printf("REQUEST: %s\n", buffer);
//     fflush(stdout);

//     sendto(server_socket, response, strlen(response), 0, (SA *)client_addr, addr_len);

//     // fclose(fp);
//     printf("Closing connection\n");
// }

// // struct Data *received_data = (struct Data *)buffer;

// /**
//  * Function to handle the client's request
//  *
//  */
// char *server_response(struct Data data)
// {
//     printf("In server response\n");
//     // TODO:
//     if (data.choice == 1)
//     {
//         // Display catalogue
//         // TODO: Make displayCatalog() function return  type *char
        
//         printf("Catalogue displayed\n");
//         return DisplayCatalog(data.m, data.x, data.z);
//     }
//     else if (data.choice == 4)
//     {
//         // Search for book
//         return searchInFile(data.search);
//     }
//     else if (data.choice == 5)
//     {
//         // Order a book
//         // TODO: Implement orderBook() function
//         int order_no = OrderBook(data.y, data.n, data.number_ordered);
//         return "Book ordered";
//     }
//     else if (data.choice == 2)
//     {
//         // Pay for book
//         // TODO: Make payForItem() function return  type *char
//         PayForItem(data);
//         return "Payment successful";
//     }
//     else
//     {
//         return "Invalid option";
//     }
// }
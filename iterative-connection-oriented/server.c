#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../helpers.c"
#include "data_iter.h"

#define BUFSIZE 4096

// Function prototypes
char *server_response(struct Data data);
int main()
{
    struct sockaddr_in serveraddr; /* server's addr */
    int portno = 3000;             /* port to listen on */
    struct sockaddr_in clientaddr; /* client addr */
    int clientlen;                 /* byte size of client's address */
    int childfd;                   /* child socket */
    struct hostent *hostp;         /* client host info */
    char buf[BUFSIZE];             /* message buffer */
    char *hostaddrp;               /* dotted decimal host addr string */
    int n;                         /* message byte size */
    // Create socket
    int serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketFD < 0)
    {
        perror("Error opening socket");
        exit(1);
    }

    int optval = 1;
    setsockopt(serverSocketFD, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    // Bind to well known address

    /*
     * build the server's Internet address
     */
    bzero((char *)&serveraddr, sizeof(serveraddr));
    /* this is an Internet address */
    serveraddr.sin_family = AF_INET;

    /* let the system figure out our IP address */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* this is the port we will listen on */
    serveraddr.sin_port = htons((unsigned short)portno);

    /*
     * bind: associate the parent socket with a port
     */
    if (bind(serverSocketFD, (struct sockaddr *)&serveraddr,
             sizeof(serveraddr)) < 0)
    {
        perror("ERROR on binding");
        exit(2);
    }

    //
    // Place connection in listen mode
    //
    /*
     * listen: make this socket ready to accept connection requests
     */
    if (listen(serverSocketFD, 5) < 0) /* allow 5 requests to queue up */
    {
        perror("ERROR on listen");
        exit(3);
    }

    // Obtain a new socket for connection when a client connects
    //
    /*
     * main loop: wait for a connection request, echo input line,
     * then close connection.
     */
    clientlen = sizeof(clientaddr);
    while (1)
    {
        printf("server waiting for connection...\n");
        /*
         * accept: wait for a connection request
         */
        childfd = accept(serverSocketFD, (struct sockaddr *)&clientaddr, &clientlen);
        if (childfd < 0)
        {
            perror("ERROR on accept");
            exit(4);
        }

        // Read request from client
        //

        /*
         * read: read input string from the client
         */

        bool shouldWrite = true;

        bzero(buf, BUFSIZE);
        n = read(childfd, buf, BUFSIZE);
        struct Data data;
        memcpy(&data, buf, sizeof(data));

        if (n < 0)
        {
            exit(5);
            printf("server received %d bytes: %s", n, buf);
        }
        printf("Server received: %d\n", data.choice);

        char *response = server_response(*(struct Data *)buf);
        // Send reply
        //
        /*
         * write: echo the input string back to the client
         */
        if (shouldWrite == true)
        {
            n = write(childfd, response, strlen(response));
            if (n < 0)
            {
                perror("ERROR writing to socket");
                exit(6);
            }
        }
    }
}

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

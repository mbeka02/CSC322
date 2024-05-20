#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <time.h>
#include <unistd.h> // read(), write(), close()

#include "helpers.c"

#define BUFSIZE 1024

int main() {
    struct sockaddr_in servaddr, cli;

    // Get hostname of where you want to connect to
    char hostname[100];
    int result = getLine("Enter hostname of server: ", hostname, 100);
    if (result != 0) {
        perror("Error Getting hostname\n");
        exit(1);
    }
    printf("Entered hostname is => %s\n", hostname);
    
    // Get details such as port number and IP from hostname
    struct hostent *he = gethostbyname(hostname);
    if (he == NULL) {
        perror("Error Getting details of host");
        exit(2);
    } 

    char* serverIPAddress = inet_ntoa(*((struct in_addr*) he->h_addr_list[0]));
    printf("Host name is => %s\n", he->h_name);
    printf("Server IP is: %s\n", serverIPAddress);

    // Define port number used by target server
    int serverPortNumber = 80;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(serverIPAddress);
    servaddr.sin_port = htons(serverPortNumber);

    // Create socket
    int clientSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocketFD < 0)
    {
        perror("Error opening socket");
        exit(2);
    }

    // Allow TCP to dynamically choose port client will use (so don't bind)
    
    // Connect socket to server

    // connect the client socket to server socket
    if (connect(clientSocketFD, (const struct sockaddr*)&servaddr, sizeof(servaddr))
        != 0) {
        perror("connection with the server failed...\n");
        exit(3);
    }
    
    char *message = "GET / HTTP/1.1\r\nAccept: */*\r\nUser-Agent: curl/7.81.0\r\n\r\n";
    // Send request
    int isWritten = write(clientSocketFD, message, strlen(message));
    if (isWritten == -1) {
        perror("Could Not Send HTTP request\n");
        exit(4);
    }

    char response[30000];
    read(clientSocketFD, response, sizeof(response));
    printf("Response from server: %s\n", response);
    // Print response

    // Close the connection
    close(clientSocketFD);
    printf("Client socket closed\n"); 
}

#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define BUFSIZE 1024

int main() {
    struct sockaddr_in servaddr, cli;
    // Define IP address of server you want to communicate with (target server)
    char * serverIPAddress = "127.0.0.1";
 
    // Define port number used by target server
    int serverPortNumber = 3000;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(serverIPAddress);
    servaddr.sin_port = htons(serverPortNumber);

    // Create socket
    int clientSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocketFD < 0)
    {
        perror("Error opening socket");
        exit(1);
    }

    // Allow TCP to dynamically choose port client will use (so don't bind)
    
    // Connect socket to server

    // connect the client socket to server socket
    if (connect(clientSocketFD, (const struct sockaddr*)&servaddr, sizeof(servaddr))
        != 0) {
        perror("connection with the server failed...\n");
        exit(0);
    }

    // Loop
    while(1) {
        // Send a request (use our defined message format)
        char *message = "Hello There Server!";
        int isWritten = write(clientSocketFD, message, strlen(message));

        if (isWritten == -1) {
            perror("Could Not Send Data");
            exit(2);
        }

        // Await response
        // Create buffer to hold server response
        char buffer[BUFSIZE];
        // Get response
        bzero(buffer, BUFSIZE); 
        // read the message from client and copy it in buffer 
        read(clientSocketFD, buffer, sizeof(buffer));
        // Print Response
        printf("Server responded with %s", buffer);
        break;
    }

    // Close the connection
    close(clientSocketFD); 
}

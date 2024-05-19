#include  <sys/types.h>
#include  <signal.h>
#include  <stdarg.h>
#include  <errno.h>
#include  <fcntl.h>
#include  <sys/time.h>
#include  <sys/ioctl.h>
#include  <netdb.h>

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <sys/socket.h>
#include  <arpa/inet.h>
#include  <stdbool.h>
#include  <limits.h>

#define SERVER_PORT  8989
#define BUFSIZE  4096
#define SOCKETERROR  (-1)
#define SERVER_BACKLOG  100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

//prototypes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>

#define SERVER_PORT  8989
#define BUFSIZE  4096
#define SOCKETERROR  (-1)
#define SERVER_BACKLOG  100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

// Function prototypes
void *handle_connection(int);
int check(int exp, const char *msg);
int accept_new_connection(int server_socket);
int setup_server(short port, int backlog);

int main(int argc, char **argv)
{
    int server_socket = setup_server(SERVER_PORT, SERVER_BACKLOG);

    fd_set current_sockets, ready_sockets;

    // Initialize my current set
    FD_ZERO(&current_sockets);
    FD_SET(server_socket, &current_sockets);

    while (true)
    {
        // Temporary copy, because select is destructive
        ready_sockets = current_sockets;

        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
        {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &ready_sockets))
            {
                if (i == server_socket)
                {
                    // This is a new connection
                    int client_socket = accept_new_connection(server_socket);
                    FD_SET(client_socket, &current_sockets);
                }
                else
                {
                    // Do whatever we do with connections
                    handle_connection(i);
                    FD_CLR(i, &current_sockets);
                }
            }
        }
    } // while

    return EXIT_SUCCESS;
}

int setup_server(short port, int backlog)
{
    int server_socket, client_socket, addr_size;
    SA_IN server_addr;
    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0, "Failed to create socket");

    // Initialize the address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    check(bind(server_socket, (SA *)&server_addr, sizeof(server_addr)), "bind failed");
    check(listen(server_socket, backlog), "listen failed");

    return server_socket;
}

int accept_new_connection(int server_socket)
{
    int addr_size = sizeof(SA_IN);
    int client_socket;
    SA_IN client_addr;
    check(client_socket = accept(server_socket, (SA *)&client_addr, (socklen_t *)&addr_size), "accept failed");
    return client_socket;
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

void *handle_connection(int client_socket)
{
    char buffer[BUFSIZE];
    size_t bytes_read;
    int msg_size = 0;
    char actualpath[PATH_MAX + 1];

    // Read the client's message -- the name of the file to read
    while ((bytes_read = read(client_socket, buffer + msg_size, sizeof(buffer) - msg_size - 1)))
    {
        msg_size += bytes_read;
        if (msg_size > BUFSIZE - 1 || buffer[msg_size - 1] == '\n')
            break;
    }
    check(bytes_read, "recv error");
    buffer[msg_size - 1] = 0; // Null terminate the message and remove the \n
    printf("REQUEST: %s \n", buffer);
    fflush(stdout);

    // Validity check
    if (realpath(buffer, actualpath) == NULL)
    {
        printf("ERROR(Bad path): %s \n", buffer);
        close(client_socket);
        return NULL;
    }

    // Read file and send its content to client
    FILE *fp = fopen(actualpath, "r");
    if (fp == NULL)
    {
        printf("ERROR(open): %s \n", buffer);
        close(client_socket);
        return NULL;
    }

    // Read file and send its content to client
    while ((bytes_read = fread(buffer, 1, BUFSIZE, fp)))
    {
        printf("sending %zu bytes \n", bytes_read);
        write(client_socket, buffer, bytes_read);
    }
    close(client_socket);
    fclose(fp);
    printf("closing connection \n");
    return NULL;
}

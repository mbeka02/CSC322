#include  <sys/socket.h>
#include  <sys/types.h>
#include  <signal.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <arpa/inet.h>
#include  <stdarg.h>

#include  <errno.h>
#include  <fcntl.h>
#include  <sys/time.h>
#include  <sys/ioctl.h>
#include  <netdb.h>

#define SERVER_PORT  80
#define MAXLINE  4096
#define SA  struct sockaddr

void err_n_die (const char* fmt, ….);
char* bin2hex (const unsigned  char  *input, size_t len);

int main (int argc, char ** argv)
{ 
int sockfd;n;
Int sendbytes;
struct sockaddr_in servaddr;
char   sendline[MAXLINE];
char recvline[MAXLINE];

If  (argc ! == 2)
err_n_die( “usage: %s <server address>, argv[0] ”); 

If (( sockfd = socket (AF_INET, SOCKSTREAM, 0)) <  0)
err_n_die( “Error while creating the socket ”); 

bzero (&servaddr, sizeof (servaddr));
servaddr.sinfamily = AFINET;
servaddr.sin_addr.s_addr = htonl (INNADDR_ANY);
servaddr.sinport = htons (SERVER_PORT);

If (inet_pton (AF_INET, argv[1], &servaddr.sin_addr)  <= 0)
err_n_die ( “inet_pton error for %s”, argv[1]); 

If ((connect (sockfd, (SA*),  &servaddr, sizeof (servaddr))) < 0 )
err_n_die( “connect failed ”); 

snprintf (sendline, “GET  /  HTTP/1.1\r\n\r\n”);
sendbytes = strlen(sendline);

If write (sockfd, sendline, sendbytes)  != sendbytes);
err_n_die( “write error ”); 

memset (recvline, 0, MAXLINE);

while ( (n= read ( sockfd, recvline, MAXLINE-1) ) > 0)
{
 	 print I(“%s”,  recvline);
}
If (n < 0)
err_n_die( “read error ”); 
exit(0);

}

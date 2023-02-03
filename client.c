#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#include <netinet/in.h>
#include <unistd.h>

#define MAX 80
#define PORTA 7001
#define PORTB 7002

void dealServer(int client_socket)
{
    char buff[MAX];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(client_socket, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(client_socket, buff, sizeof(buff));
        printf("From Server : %s", buff);
        if ((strncmp(buff, "quit", 4)) == 0) {
            printf("Client Exit...\n");
            close( client_socket);
            exit(0);
        }
    }
}
int main(){
    int client_socket_a, client_socket_b, connection_socket;
    struct sockaddr_in server_address_a, server_address_b;
    char command[256];

    client_socket_a = socket(AF_INET, SOCK_STREAM,0);
    client_socket_b = socket(AF_INET, SOCK_STREAM,0);
    if (client_socket_a == -1){
        printf("[-] SOCKET A CREATION FAILED\n");
        exit(0);
    }
    else
        printf("[+] SOCKET CREATED\n");
    if (client_socket_b == -1){
        printf("[-] SOCKET B CREATION FAILED\n");
        exit(0);
    }
    else
        printf("[+] SOCKET CREATED\n");
    bzero(&server_address_a, sizeof(server_address_a));

    server_address_a.sin_family = AF_INET;
    server_address_a.sin_port = htons(PORTA);
    server_address_a.sin_addr.s_addr = INADDR_ANY;

    if( connect(client_socket_a,(struct sockaddr*) &server_address_a, sizeof(server_address_a)) != 0){
        printf ("[-] CONNECTION WITH SERVER A FAILED\n");
        bzero(&server_address_b, sizeof(server_address_b));

        server_address_b.sin_family = AF_INET;
        server_address_b.sin_port = htons(PORTB);
        server_address_b.sin_addr.s_addr = INADDR_ANY;

        if( connect(client_socket_b,(struct sockaddr*) &server_address_b, sizeof(server_address_b)) != 0){
            printf ("[-] CONNECTION WITH SERVER B FAILED\n");
            exit(0);
        }
        else
            printf("[+] SUCCESSFUL CONNECTION\n");
        dealServer(client_socket_b);
        close(client_socket_b);
    }
    else
        printf("[+] SUCCESSFUL CONNECTION\n");
        dealServer(client_socket_a);
        close(client_socket_a);
}
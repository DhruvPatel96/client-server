#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <unistd.h>
#include <string.h>

#define MAX 80
#define PORTA 7001
#define PORTB 7002


void serviceClient(int client_socket)
{
    char cmd[MAX];
    int cmd_output;
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(cmd, MAX);
        // read the message from client and copy it in cmder
        cmd_output = read(client_socket, cmd, sizeof(cmd));
        if(cmd_output > 0){
            cmd[cmd_output] = '\0';
            dup2(client_socket,STDOUT_FILENO);
            dup2(client_socket,STDERR_FILENO);
        }
        if (strncmp("quit", cmd, 4) == 0) {
            printf("Server Exit...\n");
            close( client_socket);
            exit(0);
        }
        else {
            system(cmd);
        }
    }
}

int loadBalancer(int server_socket_a, int server_socket_b, int client){

    if (client < 5) return server_socket_a;
    if (client > 5 && client <10) return server_socket_b;
    if (client % 2 == 0 ) return server_socket_b;
    else return server_socket_a;

}
int main(){
    char command[256];
    int server_socket_a, server_socket_b, client_socket, len;
    struct sockaddr_in server_address_a, server_address_b, temp_client;

    server_socket_a = socket(AF_INET,SOCK_STREAM,0);
    if (server_socket_a == -1){
        printf("[-] SOCKET CREATION FAILED\n");
        exit(0);
    }
    else
        printf("[+] SOCKET A CREATED\n");

    bzero(&server_address_a, sizeof(server_address_a));

    server_address_a.sin_family = AF_INET;
    server_address_a.sin_port = htons(PORTA);
    server_address_a.sin_addr.s_addr = INADDR_ANY;

    if ((bind(server_socket_a, (struct sockdaddr*)&server_address_a, sizeof(server_address_a))) !=0){
        printf ("[-] SOCKET A BIND UNSUCCESSFUL\n");
        printf ("[=] TRYING SOCKET B BIND \n");
        server_socket_b = socket(AF_INET,SOCK_STREAM,0);
        if (server_socket_b == -1)
            printf("[-] SOCKET  B CREATION FAILED\n");
        bzero(&server_address_b, sizeof(server_address_b));

        server_address_b.sin_family = AF_INET;
        server_address_b.sin_port = htons(PORTB);
        server_address_b.sin_addr.s_addr = INADDR_ANY;

        if ((bind(server_socket_b, (struct sockdaddr*)&server_address_b, sizeof(server_address_b))) !=0){
            printf ("[-] SOCKET BIND B UNSUCCESSFUL\n");
            exit(0);
            }
        else 
            printf("[+] SOCKET BIND B SUCCESSFUL\n");

        if ((listen(server_socket_b,5)) !=0) {
            printf("[-] SOCKET B LISTENING FAILED\n");
            exit(0);
        }
        else
            printf("[+] SERVER B STARTED LISTENING\n");
        }

    else {
        printf("[+] SOCKET A BIND SUCCESSFUL\n");

        if ((listen(server_socket_a,5)) !=0) {
            printf("[-] SOCKET A LISTENING FAILED\n");
            exit(0);
        }
        else
            printf("[+] SERVER A STARTED LISTENING\n");
        }
    
    len = sizeof(temp_client);

    int pid, client = 4, serverSelector;
    while(1){
        serverSelector = loadBalancer(server_socket_a,server_socket_b,client);
        client_socket = accept(serverSelector,(struct sockdaddr*)&temp_client, &len);
        // if (client_socket < 0) {}
        // else {
            if (serverSelector == server_socket_a)
                printf("[+] SERVER A ACCEPT THE CLIENT %d\n", client);
            else    
                printf("[+] SERVER B ACCEPT THE CLIENT %d\n", client);
            client ++;
            pid = fork();
            if (pid < 0) {
                printf("[-] NEW PROCESS CREATION FAILED");
            }
            if (pid == 0) {
                printf("[+] NEW PROCESS CREATION SUCCESS");
                serviceClient(client_socket);
                // close(client_socket);
            // }
        }
    } 
    return 0;
}
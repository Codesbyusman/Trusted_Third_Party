
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main()
{
    char clientMessage[200] = "usman-usman ";

    printf("\n\t ----------------------------- [+] Registering with the details [+] -------------------------------- \n\n");

    // the socket
    int client_soc = socket(AF_INET, SOCK_STREAM, 0);

    // setting the address
    struct sockaddr_in client_address;
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = INADDR_ANY;
    client_address.sin_port = htons(8080);

    // the connect from client
    connect(client_soc, (struct sockaddr *)&client_address, sizeof(client_address));

    // start sending and reciving receving messages
    send(client_soc, clientMessage, sizeof(clientMessage), 0);
   
    char buf[200];
    recv(client_soc, &buf, sizeof(buf), 0);
    printf("\n %s \n", buf);

    close(client_soc);

    return 0;
}
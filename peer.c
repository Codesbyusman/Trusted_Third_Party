#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAXLINE 1024

char name[20];
char completeAddress[100];
int PORT;

void sending(char[200], const int);
void receiving(int server_sock);
void *receive_thread(void *server_sock);

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("\n\t\t\t\t\t :::::: Bad Arguments ::::::\n");
        printf("\n :::: Please Enter the port number where you want to run the client other than 8080 (server port) :::::");
        exit(EXIT_FAILURE);
    }

    PORT = atoi(argv[1]);

    printf("Enter username:");
    scanf("%s", name);

    char password[20];
    printf("Enter password:");
    scanf("%s", password);

    int server_sock, new_socket, n;

    int k = 0;

    // socket creation
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // making the socket the main on the specified port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // binding
    if (bind(server_sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("\n\t\t ::::: Error in binding --- run again ::::::\n\n");
        exit(EXIT_FAILURE);
    }

    // listening
    if (listen(server_sock, 10) < 0)
    {
        perror("\n\t\t ::::: Error in listening --- run again ::::::\n\n");
        exit(EXIT_FAILURE);
    }

    // the credentials
    char credentials[MAXLINE];
    strcpy(credentials, name);
    strcat(credentials, "-");
    strcat(credentials, password);

    // sending(credentials);
    // // start sending and reciving receving messages
    // send(server_sock, credentials, sizeof(credentials), 0);

    // char buf[200];
    // recv(server_sock, &buf, sizeof(buf), 0);
    // printf("\n %s \n", buf);

    // the complete address that will uniquely identify the client
    
    strcpy(completeAddress, inet_ntoa(address.sin_addr));
    strcat(completeAddress, ":");
    strcat(completeAddress, argv[1]);

    int option;
    pthread_t tid;
    pthread_create(&tid, NULL, &receive_thread, &server_sock); // Creating thread to keep receiving message in real time
    
    printf("\n*****At any point in time press the following:*****\n1.Send message\n0.Quit\n");
    printf("\nEnter choice:");
    do
    {

        scanf("%d", &option);
        switch (option)
        {
        case 1:
            sending(NULL, 8080);
            break;
        case 0:
            printf("\nLeaving\n");
            break;
        default:
            printf("\nWrong choice\n");
        }
    } while (option);

    close(server_sock);

    return 0;
}

// Sending messages to port
void sending(char message[MAXLINE], const int send_to_PORT)
{

    char buffer[2000] = {0};
    char hello[1024] = {0};

    // Fetching port number

    // IN PEER WE TRUST
    printf("Enter the port to send message:"); // Considering each peer will enter different port
    scanf("%d", &send_to_PORT);

    int sock = 0, n;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error in socket creation\n");
        return;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(send_to_PORT);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return;
    }

    char dummy;
    printf("Enter your message:");
    scanf("%c", &dummy); // The buffer is our enemy
    scanf("%[^\n]s", hello);
    sprintf(buffer, "%s says:-%s",completeAddress , name, PORT, hello);
    send(sock, buffer, sizeof(buffer), 0);
    printf("\nMessage sent\n");

    char buf[200];
    recv(sock, &buf, sizeof(buf), 0);
    printf("\n %s \n", buf);

    close(sock);
}

// Calling receiving every 2 seconds
void *receive_thread(void *server_sock)
{
    int s_fd = *((int *)server_sock);
    while (1)
    {
        sleep(2);
        receiving(s_fd);
    }
}

// Receiving messages on our port
void receiving(int server_sock)
{
    struct sockaddr_in address;
    int n;
    char buffer[2000] = {0};
    int addrlen = sizeof(address);
    fd_set current_sockets, ready_sockets;

    // Initialize my current set
    FD_ZERO(&current_sockets);
    FD_SET(server_sock, &current_sockets);
    int k = 0;
    while (1)
    {
        k++;
        ready_sockets = current_sockets;

        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
        {
            perror("Error");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &ready_sockets))
            {

                if (i == server_sock)
                {
                    int client_socket;

                    if ((client_socket = accept(server_sock, (struct sockaddr *)&address,
                                                (socklen_t *)&addrlen)) < 0)
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(client_socket, &current_sockets);
                    
                }
                else
                {
                    printf("GGGGGGGGGG");
                    n = recv(i, buffer, sizeof(buffer), 0);
                    printf("\n%s\n", buffer);
                    FD_CLR(i, &current_sockets);
                }
            }
        }

        if (k == (FD_SETSIZE * 2))
            break;
    }
}
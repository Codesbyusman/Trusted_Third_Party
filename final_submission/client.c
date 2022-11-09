// Muhammad Usman shahid
//       20i-1797
//         CY-T

//   ------ client -----

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>

#define MAXLINE 1024

char name[20];
char completeAddress[100];
int PORT;

// my keys
double nKey = 0;
double e = 0;
double d = 0;

// to communicate with
double nOther = 0;
double eOther = 0;

// the operation functions
void sendMessages();
void generateKeys();
void getOtherKeys();

// for the reciving making a socket with thread help to listen after each 2 seconds
void receiveMsg(int server_sock);
void *receiveThread(void *server_sock);

// the encryption decryption
double encryptDecrypt(double msg, double eORd, double n);

int main(int argc, char const *argv[])
{
    // command line checking
    if (argc != 2)
    {
        printf("\n\t\t\t\t\t :::::: Bad Arguments ::::::\n");
        printf("\n\n :::: Please Enter the port number where you want to run the client other than 8080 (server port) ::::: \n");
        exit(EXIT_FAILURE);
    }

    // got the port for the client
    PORT = atoi(argv[1]);

    printf("Enter username : ");
    scanf("%s", name);

    char password[20];
    printf("Enter password : ");
    scanf("%s", password);

    int server_sock, new_socket, n;

    // int k = 0;

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
    pthread_create(&tid, NULL, &receiveThread, &server_sock); // Creating thread to keep receiving message in real time

    do
    {
        printf("\n\n1. Generate Keys\n2. Get Public key \n3. Communicate with others \n0. Quit\n");
        printf("\nEnter the desired option : ");

        scanf("%d", &option); // asking the option
        switch (option)
        {
        case 1:
            generateKeys(); // generate keys
            break;
        case 2:
            getOtherKeys(); // get other client keys
            break;
        case 3:
            // for commmunicating with anyone
            sendMessages();
            break;
        case 0:
            printf("\n\t\t [+] --------------- See you soon :( --------------- [+]\n");
            break;
        default:
            printf("\n :::::::: Please Choose the correct option in the menu :::::::::\n");
        }
    } while (option); // if 0 the while will be stopped

    close(server_sock);

    return 0;
}

// Sending messages to port
void sendMessages()
{

    int send_to_PORT = 0;

    // taking the port number
    printf("\n Enter the port to send message (address of other) : ");
    scanf("%d", &send_to_PORT);

    int sock = 0, n;

    if (send_to_PORT == 8080)
    {
        // server things
        printf("\n\t\t :::::: Get server key by entering 3 ::::::\n\n");

        getOtherKeys();
    }

    if (nOther == 0)
    {
        printf("\n\t\t :::::: Get key from server to whon you want to communicate ::::::\n\n");
        return;
    }

    // making the sockets
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n\t\t ::::: Error in socket creation--- run again ::::::\n\n");
        return;
    }

    // the server address
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(send_to_PORT);

    // connecting the client
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n\t\t ::::: Error in connecting --- run again ::::::\n\n");
        return;
    }

    char cinIgnore; // to avoid the input problems
    scanf("%c", &cinIgnore);

    while (1)
    {
        // the aeearys for the storage
        char message[MAXLINE] = {0};
        char sendThis[MAXLINE] = {0};

        message[0] = '\0';

        printf("\n Enter the message to sent : ");
        scanf("%[^\n]%*c", message); // taking input

        if (!strcmp(message, "exit"))
        {

            // creating the string to send
            sprintf(sendThis, "%s", message);
            send(sock, sendThis, sizeof(sendThis), 0);

            break;
        }

        double ciphertext = encryptDecrypt(strtod(message, NULL), eOther, nOther);
        printf("\n encrypted is : %lf\n", ciphertext);

        // creating the string to send
        sprintf(sendThis, "From %s: (%s) :%lf", completeAddress, name, ciphertext);

        send(sock, sendThis, sizeof(sendThis), 0);

        printf("\n\n\t :::: Message Send to the other end ::: \n\n");

        // char buf[200];
        // recv(sock, &buf, sizeof(buf), 0);
        // printf("\n %s \n", buf);
    }

    close(sock);
}

// generating keys
void generateKeys()
{

    int send_to_PORT = 8080; // server port

    int sock = 0, n;

    // making the sockets
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n\t\t ::::: Error in socket creation--- run again ::::::\n\n");
        return;
    }

    // the server address
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(send_to_PORT);

    // connecting the client
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n\t\t ::::: Error in connecting --- run again ::::::\n\n");
        return;
    }

    char cinIgnore; // to avoid the input problems
    scanf("%c", &cinIgnore);

    // the aeearys for the storage
    char message[MAXLINE] = {0};
    sprintf(message, "keys-%d", PORT); // sending message with port

    send(sock, message, sizeof(message), 0);

    char buf[MAXLINE] = {0};
    recv(sock, &buf, sizeof(buf), 0);

    nKey = strtod(strtok(buf, "-"), NULL);
    e = strtod(strtok(NULL, "-"), NULL);
    d = strtod(strtok(NULL, "-"), NULL);

    close(sock);
}

// Calling receiving after every 2 seconds
void *receiveThread(void *server_sock)
{
    int s_fd = *((int *)server_sock);
    while (1)
    {
        sleep(2);
        receiveMsg(s_fd);
    }
}

// Receiving messages on our port
void receiveMsg(int server_sock)
{

    int n;

    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // the two sets of the file descriptor
    fd_set current_sockets, ready_sockets;

    // initalize with 0 and set
    FD_ZERO(&current_sockets);
    FD_SET(server_sock, &current_sockets);

    // for maping file descriptors
    int k = 0;

    while (true)
    {
        k++;
        ready_sockets = current_sockets;

        // selecting error
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
        {
            perror("\n\n error ---- run again \n\n");
            exit(EXIT_FAILURE);
        }

        // till the file descritor to see wi]hch is ready
        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &ready_sockets))
            {

                if (i == server_sock)
                {
                    int client_socket;

                    // accept the connection
                    if ((client_socket = accept(server_sock, (struct sockaddr *)&address,
                                                (socklen_t *)&addrlen)) < 0)
                    {
                        perror("\n\naccept error ---- run again \n\n");
                        exit(EXIT_FAILURE);
                    }

                    // again the socket
                    FD_SET(client_socket, &current_sockets);
                }
                else
                {
                    // recieve accordingly
                    char buffer[2000] = {0};
                    n = recv(i, buffer, sizeof(buffer), 0);
                    printf("\nrecieved : %s\n", buffer);

                    // taking and breaking the message
                    strtok(buffer, ":");
                    int portUsed = atoi(strtok(NULL, ":"));
                    strtok(NULL, ":");
                    char *a = strtok(NULL, ":");
                    double messageGot = strtod(a, NULL);

                    // if (!strcmp(a, "exit"))
                    //     break;

                    // decrypting by server private key
                    double messageRecovered = encryptDecrypt(messageGot, d, nKey);
                    printf("\nMessage decrypted from : (%d) : %lf \n\n", portUsed, messageRecovered);

                    // referesh the descriptor used
                    FD_CLR(i, &current_sockets);
                }
            }
        }

        if (k == (FD_SETSIZE * 2))
            break;
    }
}

// generating keys
void getOtherKeys()
{

    int send_to_PORT = 8080; // server port

    int sock = 0, n;

    // making the sockets
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n\t\t ::::: Error in socket creation--- run again ::::::\n\n");
        return;
    }

    // the server address
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(send_to_PORT);

    // connecting the client
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n\t\t ::::: Error in connecting --- run again ::::::\n\n");
        return;
    }

    char cinIgnore; // to avoid the input problems
    scanf("%c", &cinIgnore);

    // the aeearys for the storage
    char message[MAXLINE] = {0};
    sprintf(message, "otherkeys-%d", PORT); // sending message with port

    send(sock, message, sizeof(message), 0);

    char buf[MAXLINE] = {0};
    recv(sock, &buf, sizeof(buf), 0);
    puts(buf);

    message[0] = '\0';

    // taking the option from the user
    printf("\n\t\t Enter the option : ");
    scanf("%s", message);
    send(sock, message, sizeof(message), 0);

    bzero(message, sizeof(message));

    char buff3[MAXLINE] = {0};
    recv(sock, &buff3, sizeof(buff3), 0);

    if (!strcmp(buff3, "\n\t\t :::::::: wrong option ::::::::\n"))
        puts(buff3);
    else
    {
        // the available key  the users
        nOther = strtod(strtok(buff3, "-"), NULL);
        eOther = strtod(strtok(NULL, "-"), NULL);
    }

    close(sock);
}

// the main RSA formula for encryption decryption
double encryptDecrypt(double msg, double eORd, double n)
{
    return fmod(pow(msg, eORd), n);
}
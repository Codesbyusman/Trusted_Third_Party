#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/types.h>

#define PORT 8080
#define MAXSIZE 1024

bool authenticate(char *username, char *password)
{
	if (strcmp(username, "usman") == 0 && strcmp(password, "usman") == 0)
		return true;

	return false;
}

int main()
{

	// the messages
	char buffer[MAXSIZE];
	char message[200] = "Hello Client";

	// the descriptors that we will use
	int tcpListen,	// tcp listening
		connection, // connection
		nready,		// the ready discriptors for the select system call
		maxfd;		// for storing maximum file descriptor

	pid_t child;

	fd_set fdset; // the fild discriptor sets
	ssize_t n;
	socklen_t len;

	const int on = 1;

	// void sig_chld(int);

	struct sockaddr_in client_address, server_address;

	tcpListen = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&server_address, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;

	// binding
	if (bind(tcpListen, (struct sockaddr *)&server_address, sizeof(server_address)) != 0)
	{
		// not binded
		printf("\n\t\t ::::: Error in binding --- run again ::::::\n\n");
		exit(EXIT_FAILURE);
	}

	// listen as tcp connection
	if (listen(tcpListen, 10) != 0)
	{
		// not binded
		printf("\n\t\t ::::: Error in listening --- run again ::::::\n\n");
		exit(EXIT_FAILURE);
	}

	// descriptor sets is emptied for the select
	FD_ZERO(&fdset);

	// get maximum file descriptor adding 1 so select include all file descriptorss
	maxfd = tcpListen + 1;

	while (true)
	{

		// set tcpListen in readset for the usage by select
		FD_SET(tcpListen, &fdset);

		// select the ready descriptor
		nready = select(maxfd, &fdset, NULL, NULL, NULL);

		// if the descriptor is ready to read go for it
		if (FD_ISSET(tcpListen, &fdset))
		{
			len = sizeof(client_address);
			connection = accept(tcpListen, (struct sockaddr *)&client_address, &len);

			// making sub process
			child = fork();

			// making a child to handle the connection
			if (child == 0)
			{
				close(tcpListen);

				buffer[0] = '\0';

				// connection established
				// start reciving and sending messages
				recv(connection, &buffer, sizeof(buffer), 0);
				puts(buffer);
				// authenticating the user
				strtok(buffer, "-"); // the address
				char *username = strtok(NULL, "-");
				char *password = strtok(NULL, "-");

				if (authenticate(username, password))
				{

					// sending to the client
					send(connection, message, sizeof(message), 0);
				}
				else
				{
					char disconnectiong[200] = "\n\t :::: Can not authenticate you ::: \n";
					// sending to the client
					send(connection, disconnectiong, sizeof(disconnectiong), 0);
				}

				close(connection);
				exit(0);
			}

			// closed the made connection
			close(connection);
		}
	}

	return 0;
}

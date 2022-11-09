// Muhammad Usman shahid
//       20i-1797
//         CY-T
//   ------ server -----

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/types.h>

// the port
#define PORT 8080
#define MAXSIZE 1024

// 2 users data
int userPort1 = 0;
int userPort2 = 0;

double n1 = 0;
double n2 = 0;
double ns = 0;

double e1 = 0;
double e2 = 0;
double es = 0;

double d1 = 0;
double d2 = 0;
double ds = 0;

// prime numbers array
// double primes[12] = {2633, 9349, 5531, 5851, 5689, 7853, 7481, 8923, 1523, 8101};

// double primes[12] = {93059, 21493, 26387, 12899, 70937, 60923, 37963, 64157, 89051, 6976};

// double primes[12] = {857, 997, 967, 809, 757, 461, 313, 151, 691, 439};

double primes[12] = {59, 97, 43, 31, 79, 23, 67, 11, 53, 89};

// double primes[10] = {1, 5, 7, 9, 11, 5, 7, 23, 19, 7};


double encryptDecrypt(double msg, double eORd, double n);

// for the authentication
bool authenticate(char *username, char *password)
{
	if (strcmp(username, "usman") == 0 && strcmp(password, "usman") == 0)
		return true;

	return false;
}

void generateAll(double *n, double *e, double *d); // generating the credentials
double findGCD(double a, double b);				   // for finding the gcd

int main()
{

	// for randomness
	srand(time(NULL));

	printf("\n\t\t [+] ------------------ Trusted Third Party ------------------ [+]\n");

	// generating the keys for the server
	generateAll(&ns, &es, &ds);

	// the descriptors that we will use
	int tcpListen,	// tcp listening
		connection, // connection
		nready,		// the ready discriptors for the select system call
		maxfd;		// for storing maximum file descriptor

	pid_t child;

	fd_set fdset; // the file discriptor sets
	ssize_t n;
	socklen_t len;

	// the socket addresses
	struct sockaddr_in client_address, server_address;

	tcpListen = socket(AF_INET, SOCK_STREAM, 0);

	// bzero(&server_address, sizeof(server_address));

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

	// always on server
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

			// the messages
			char buffer[MAXSIZE];
			buffer[0] = '\0';

			// connection established
			// start reciving and sending messages
			recv(connection, &buffer, sizeof(buffer), 0);

			char buff2[MAXSIZE] = {0};
			strcpy(buff2, buffer);

			if (!strcmp(strtok(buff2, "-"), "keys"))
			{
				// the port number for saving the things
				int portU = atoi(strtok(NULL, "-"));

				// generating the key components
				double nn = 0;
				double e = 0;
				double d = 0;

				// generating all
				generateAll(&nn, &e, &d);

				if (portU == userPort1)
				{
					// the first user use that variables and send them
					e1 = e;
					d1 = d;
					n1 = nn;
				}
				else if (portU == userPort2)
				{
					// the second user -- use that variables
					e2 = e;
					d2 = d;
					n2 = nn;
				}
				else if (userPort1 == 0)
				{
					// mean no port go for one
					e1 = e;
					d1 = d;
					n1 = nn;

					userPort1 = portU;
				}
				else
				{
					// only second left go for that variables
					e2 = e;
					d2 = d;
					n2 = nn;

					userPort2 = portU;
				}

				char message[MAXSIZE] = {0};
				// the key generation
				sprintf(message, "%lf-%lf-%lf", nn, e, d);

				send(connection, message, sizeof(message), 0);
				printf("\n\n\t\t :::: Keys generated ::::\n\n");
				// break; // done
			}
			else
			{

				// making sub process
				child = fork();

				// making a child to handle the connection
				if (child == 0)
				{
					printf("\n\n\t [+] ------------ New Client session ------------ [+] \n\n");

					close(tcpListen);

					while (true)
					{
						// the messages
						// char buffer[MAXSIZE];
						char message[MAXSIZE] = {0};

						// buffer[0] = '\0';

						if (!strcmp(buffer, "exit"))
							break; // remove the client connection

						char buff2[MAXSIZE] = {0};
						strcpy(buff2, buffer);

						if (!strcmp(strtok(buff2, "-"), "otherkeys"))
						{
							// the port number for saving the things
							int portU = atoi(strtok(NULL, "-"));

							char sendTHis[MAXSIZE] = {0};

							sprintf(sendTHis, "\n\t\t Available options are (0 mean no user till now ) : \n\n\t\t1. User 1 port : %d \n\t\t2. User 2 port : %d \n\t\t3. Server Port : 8080 \n\n", userPort1, userPort2);
							send(connection, sendTHis, sizeof(sendTHis), 0);

							bzero(buffer, sizeof(buffer));
							buffer[0] = '\0';
							recv(connection, &buffer, sizeof(buffer), 0);

							int option = atoi(buffer);
							sendTHis[0] = '\0';

							// checking the options
							if (option == 1)
							{
								// user 1
								sprintf(sendTHis, "%lf-%lf", n1, e1);
							}
							else if (option == 2)
							{
								// user 2
								sprintf(sendTHis, "%lf-%lf", n2, e2);
							}
							else if (option == 3)
							{
								// server
								// user 2
								sprintf(sendTHis, "%lf-%lf", ns, es);
							}
							else
							{
								sprintf(sendTHis, "\n\t\t :::::::: wrong option ::::::::\n\0");
							}

							send(connection, sendTHis, sizeof(sendTHis), 0);
							printf("\n\n\t\t :::: Keys granted if exist ::::\n\n");

							break;
						}

						// puts(buffer);
						// // authenticating the user
						// strtok(buffer, "-"); // the address
						// char *username = strtok(NULL, "-");
						// char *password = strtok(NULL, "-");

						// authenticating with the server
						// if (authenticate(username, password))
						// {

						// 	// sending to the client
						// 	send(connection, message, sizeof(message), 0);
						// }
						// else
						// {
						// 	char disconnectiong[200] = "\n\t :::: Can not authenticate you ::: \n";
						// 	// sending to the client
						// 	send(connection, disconnectiong, sizeof(disconnectiong), 0);
						// }

						// send(connection, message, sizeof(message), 0);
						// connection established
						// // start reciving and sending messages
						// recv(connection, &buffer, sizeof(buffer), 0);


						puts(buffer);

						// taking and breaking the message
						strtok(buffer , ":");
						int portUsed = atoi(strtok(NULL, ":"));
						strtok(NULL, ":");
						double messageGot = strtod(strtok(NULL , ":") , NULL);

						// decrypting by server private key
						double messageRecovered = encryptDecrypt(messageGot , ds , ns);
						printf("\nMessage decrypted from : (%d) : %lf \n\n" , portUsed, messageRecovered);

						buffer[0] = '\0';
						// start reciving and sending messages
						recv(connection, &buffer, sizeof(buffer), 0);
					}

					printf("\n\n\t [+] ------------ Client Gone ------------ [+] \n\n");

					// close the current connection
					close(connection);

					// exit the child
					exit(0);
				}
			}
			// closed the made connection
			close(connection);
		}
	}

	return 0;
}

// generating the parameters
void generateAll(double *n, double *e, double *d)
{

	// picking p and q randomly from the array
	double p = primes[rand() % 10];
	double q = primes[rand() % 10];

	// rsa implementation
	*n = p * q;

	double phi = (p - 1) * (q - 1);

	// *e = 1.0;
	// selection of e
	do
	{
		*e = (rand() % (long)100) + 1; // the random e
	} while (!((findGCD(*e, phi) == 1) && (*e < phi)));

	long i = 0;
	// selecting d on the basis of e
	do
	{
		i++;
		*d = fmod((1 + (i * phi)), *e);

	} while (*d != 0);

	*d = ((1 + (i)*phi) / (*e));

	// all things are done
	return;
}

// finding the gcd
double findGCD(double a, double b)
{
	// going down and down till both are equal as
	// a= 2 , b = 3
	// 1-step gcd(2, 1)
	// 2-step gcd(1, 1)
	// 3-step 1 as both are same

	// base case
	if (a == b)
		return a;
	else if (a > b)
		findGCD(a - b, b); // recursive call
	else
		findGCD(a, b - a); // recursive call
}

// the main RSA formula for encryption decryption
double encryptDecrypt(double msg, double eORd, double n)
{
    return fmod(pow(msg, eORd), n);
}

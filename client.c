/*
Filename: client.c
File Description: A TCP client for a socket based application 
				  for the following commands:
				  1. help
				  2. fibonacci
				  3. sort
				  4. random
				  5. show history
				  6. exit
Execution Commands: Compile the client by the command:
					gcc client.c -o client.out
					./client.out localhost 4550
 */

#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
	int newSocket;
	struct sockaddr_in server_addr;
	int port_number;
	int value;
	struct hostent *server;
	int i;

	char buffer[1024];

	if(argc < 3)
	{
		fprintf(stderr, "No Port number provided.\n");
		exit(1);
	}

	//Obtain port number from command line argument
	port_number = atoi(argv[2]);

	//------Create a TCP Socket

	newSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(newSocket <= 0)
	{
		perror("Failed to create a socket...\n");
		exit(1);
	}

	server = gethostbyname(argv[1]);
	if(server == NULL)
	{
		fprintf(stderr, "No Host found...\n");
		exit(0);
	}

	//Configure the server_addr structure settings
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);

	server_addr.sin_port = htons(port_number);

	//------Connect the server_addr

	if(connect(newSocket, &server_addr, sizeof(server_addr)) < 0)
	{
		perror("Connection Failed...\n");
		exit(1);
	}

	int a;
	do
	{
		//input the command from the client
		fgets(buffer,1024,stdin);

		//send the value to the server for evaluation
		value = write(newSocket,buffer,strlen(buffer));

		if(value < 0)	
		{
			perror("Writing to Socket Failed...\n");
			exit(1);
		}

		a = strcmp(buffer,"exit\n");
		if(strcmp(buffer,"exit\n") == 0)
		{
			close(newSocket);
			exit(0);
		}

		bzero(buffer,1024);

		//Read the results from the server
		value = read(newSocket,buffer,1024);

		if(value < 0)	
		{
			perror("Reading from Socket Failed...\n");
			exit(1);
		}
	}while(a != 0);

	close(newSocket);
	return 0;
}

/*
Filename: server.c
File Description: A TCP server for a socket based application 
				  for the following commands:
				  1. help
				  2. fibonacci
				  3. sort
				  4. random
				  5. show history
				  6. exit
Execution Commands: Compile the server by the command:
					gcc server.c -o server.out
					./server.out 4550
 */

#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>

void toLower(char *str);

void doStuff(int newSocket);

//Method to calculate the nth fibonacci number
int fibonacci(int number)
{
	int first = 0, second = 1, next;
	int i;
	if(number == 0)
		return 0;
	else if(number == 1)
		return 1;
	for(i = 2; i < number; i++)
	{
		next = first + second;
		first = second;
		second = next;
	}
	return second;
}

int main(int argc, char *argv[])
{
	//Create variables

	int newSocket;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int mySocket;
	char buffer[1024];
	int port_number;
	socklen_t clilen;
	int value;
	int i;
	int pid;

	if(argc < 2)
	{
		fprintf(stderr, "No Port number provided.\n");
		exit(1);
	}

	//------Create a TCP Socket

	newSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(newSocket <= 0)
	{
		perror("Failed to create a socket...\n");
		exit(1);
	}

	//Obtain port number from command line argument
	port_number = atoi(argv[1]);

	//Configure the server_addr structure settings
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port_number);

	//------Bind the server_addr structure to socket
	if(bind(newSocket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		perror("Bind Failed...\n");
		exit(1);
	}

	//------Listen on the socket
	while(1)
	{
		listen(newSocket, 10);
		clilen = sizeof(client_addr);

		//------Accept the Client connection

		mySocket = accept(newSocket, (struct sockaddr *)&client_addr, &clilen );

		if(mySocket < 0)
		{
			perror("Accept Failed...\n");
			exit(1);
		}

		//Creating child process to establish multiple connections
		pid = fork();
		if(pid < 0)
			perror("Fork Failed...\n");
		if(pid == 0)
		{
				close(newSocket);
				doStuff(mySocket);
				exit(0);
		}
	}
	return 0;
}

//Handles all communications once the client is established
void doStuff(int mySocket)
{		
	char arr[100][1024];
	int arrCounter = 0;
	int a;
	char buffer[1024];
	int value;
	do
	{
		//------Read from the Socket
		bzero(buffer,1024);
		value = read(mySocket, &buffer, sizeof(buffer));
		char history[1024];
		strcpy(history,buffer);
		if(value < 0)
		{
			perror("Reading Failed...\n");
			exit(1);
		}

		char *cmd = malloc(strlen(buffer) + 1);
		char args_list[10][20];
		strcpy(cmd, buffer);
		char *command;
		int space = 0;
		int z = 0;
		int step;
		while(buffer[z] != '\0')
		{
			if(buffer[z] == ' ')
			{
				space++;
			}
			z++;
		}
		if(space == 0)
			command = strtok(cmd, "\n");
		else 
			command = strtok(cmd, " ");

		//Implementation of show history command
		if(strcmp(buffer,"show history\n") == 0)
		{	
			printf("%s\n","=============Command History===========");
			for(step = arrCounter - 1; step >= 0; step--)
			{
				printf("%s",arr[step]);
			}
		}
		//toLower(command);
		int i = 0;
		char *tok = strtok(buffer, " ");
		tok = strtok(NULL, " ");
		while (tok != NULL)
		{
			strcpy(args_list[i], tok);
			i++;
			tok = strtok(NULL, " ");
		}
		int count;
		int number_array[10];
		for(count = 0; count < i ; count++)
		{
			number_array[count] = atoi(args_list[count]);
		}

		//Implementation of help command
		if(strcmp(command,"help") == 0)
		{
			printf("\t---Enter any command from the following: ---\n");
			printf("\thelp \n");
			printf("\tfibonacci \n");
			printf("\tsort \n");
			printf("\trandom \n");
			printf("\tshow history \n");
			printf("\texit \n");

			strcpy(arr[arrCounter],history);
			arrCounter++;
		}

		//Implementation of fibonacci command
		if(strcmp(command,"fibonacci") == 0)
		{
			int fibonacci_result = fibonacci(number_array[0]);
			printf("The %dth fibonacci number is: %d\n", number_array[0], fibonacci_result);
			strcpy(arr[arrCounter],history);
			arrCounter++;			
		}

		a = strcmp(command,"exit");

		//Implementation of exit command
		if(strcmp(command,"exit") == 0)
		{
			return;
		}

		//Implementation of sort command
		if(strcmp(command, "sort") == 0)
		{
			int temp;
			int j,k;
			for(j = 0; j < i; j++)
			{
				for(k = j+1; k < i; k++)
				{
					if(number_array[j] < number_array[k])
					{
						temp = number_array[j];
						number_array[j] = number_array[k];
						number_array[k] = temp;
					}
				}
			}

			printf("The Sorted array in descending order is: ");
			for(j = 0; j<i; j++)
			{
				printf("%d ", number_array[j]);
			}
			printf("\n");

			strcpy(arr[arrCounter],history);
			arrCounter++;
		}

		//Implementation of random command
		if(strcmp(command,"random") == 0)
		{
			int first;
			int last;
			int random;
			first = number_array[0];
			last = number_array[1];
			random = (rand() % (last + 1 - first)) + first;
			printf("The random number in the range %d to %d is: %d \n", first, last, random);

			strcpy(arr[arrCounter],history);
			arrCounter++;
		}

		write(mySocket, buffer, strlen(buffer));

	}while(a != 0);
}

//Function to convert all the commands obtained to lowercase
void toLower(char *str)
{
	while(*str != '\0')
	{
		*str = tolower(*str);
	}
}

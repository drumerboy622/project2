/************************************************************************************
 * Author: Matthew Solbrack
 * Date: 08/08/2019
 * Class Information: Introduction to Computer Networks CS372/400
 * Name: ftserver.c / Programming Assignment #2
 * Description: This script inplements the server part of the 2-connection client-
 *    server network application. This is another example of using sockets API.
 *    This is a refresh of programming skills.
 *    *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <time.h>
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>
#include <arpa/inet.h>


/************************************************************************************
 * Name: 
 * Input:
 * Return: 
 * Description:
 * **********************************************************************************/

/************************************************************************************
 * Name: Main() 
 * Input: Port Number
 * Return: 0
 * Description: The main part of the server part of the 2-connection client-server
 *   network application. 
 * **********************************************************************************/

int main(int argc, char* argv[])
{
	//variables
	struct addrinfo 	infor; //used to get address information
	struct sockaddr_storage their_addr; //client address struct
	
	struct addrinfo * 	results; //used to point to info struct
	int 			socketNum, //variable to hold port number
				new_fd;
	socklen_t 		addr_size; 
	char 			buf[100], //holds filp info
				instructions[5]; //holds instructions from client
	
	DIR *folder; //Open's directory


	//Make sure port is included at startup
	if (argc != 2)
	{
		printf("Please enter ./ftserver [port #] to run this script!\n");
		exit (1);
	}

	memset(&infor, 0, sizeof infor);
	// get address infomration
	infor.ai_family = AF_UNSPEC;
	infor.ai_socktype = SOCK_STREAM;
	infor.ai_flags = AI_PASSIVE;

	//add address information to results variable
	if(getaddrinfo(NULL, argv[1], &infor, &results))
	{
		printf("Error gathering the correct IP/Port number\n");
		exit(1);
	}

	//create the socket
	if(socketNum = socket(results->ai_family, results->ai_socktype, results->ai_protocol) == -1)
	{
		printf("Cannot create socket\n");
		exit(1);
	}

	printf("%d\n", socketNum);

	//bind socket
	if(bind(socketNum, results->ai_addr, results->ai_addrlen) == -1)
	{
		close(socketNum);
		printf("Cannot bind socket\n");
		exit(1);
	}

	//listen
	if(listen(socketNum, 5) == -1)
	{
		close(socketNum);
		printf("Cannot listen to socket.\n");
		exit(1);
	}
	else
		printf("Server open on %d\n", socketNum);
/*
	//Wait for connection and accept request from client
	while(1)
	{
		addr_size = sizeof(their_addr);
		if((new_fd = accept(socketNum, &their_addr, &addr_size)) == -1)
			continue;
	}
		

	recv(new_fd, buf, sizeof(buf)-1, 0);
	
	//output the server name
	printf("Connection from %s\n", buf);

	recv(new_fd, instructions, sizeof(instructions)-1, 0);	
	
	if(strcmp(instructions, "l") == 0)
	{
		if((folder = opendir('.')) == NULL)
		{
			printf("Unable to read directory\n");
			exit(1);
		}


	}


*/	


	//Free the address information
	freeaddrinfo(results);
	

	return 0;
}



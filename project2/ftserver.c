/************************************************************************************
 * Author: Matthew Solbrack
 * Date: 08/08/2019
 * Class Information: Introduction to Computer Networks CS372/400
 * Name: ftserver.c / Programming Assignment #2
 * Description: This script inplements the server part of the 2-connection client-
 *    server network application. This is another example of using sockets API.
 *    This is a refresh of programming skills. Almost all information was taken
 *    from Beej's guide to network programming. Other references are noted below.
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
 * Name: socket_2_info
 * Input: location (domain name), port
 * Return: struct object
 * Description: This method creates an object struct to hold pertinent information 
 *    about connection with socket_2
 * **********************************************************************************/
struct addrinfo * socket_2_info(char * location, char * port){
	int status;
	struct addrinfo hints;
	struct addrinfo * res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if((status = getaddrinfo(location, port, &hints, &res)) != 0)
	{
		printf("getaddrinfo error\n");
		exit(1);
	}

	return res;
}


/************************************************************************************
 * Name: Main() 
 * Input: Port Number
 * Return: 0
 * Description: The main section of the server part of the 2-connection client-server
 *   network application. For ease of reading, the main function was created to hold
 *   all important duties of the server. It allows clients to see what files 
 *   are in it's directory. And it allows the client to get a file from the directory. 
 * **********************************************************************************/

int main(int argc, char* argv[])
{
	//variables
	struct addrinfo 	infor; //used to get address information
	struct sockaddr_storage their_addr; //client address struct
	
	struct addrinfo * 	results; //used to point to info struct
	struct addrinfo *	res; // used to point to info struct for second port
	int 			socketNum, //variable to hold port number
				data_socket, //variable to hold 2nd port num
				new_fd,
				nbytes, //tracks the number of bytes sent
				fd, //used for open file
				file_exist, // used as switch if file exists
				i, // increment through data
				num_files; // tracks number of files sent
	socklen_t 		addr_size; 
	char *	file_not_found = "File not found";
	char *	files[100]; //pointer to array 
	char * 	done = "done"; // sent when finished transmitting
	char * 	file_found = "File found";
	char *	standardResponse = "1"; // response when information is received correctly
	char *	errorResponse = "2"; // response when information is not received correct
        char 	port[10], // port number
		command[3], // command from client
		buf[1000], //holds filp info
		hostname[6], // client hostname 
		location[30], // location to send information
		filename[100], // for the filename
		file[100][25]; // holds file names

	void * 	point_to_buf = 0; // points to buffer 

	// to open the directory
	DIR * 	directory;
	struct dirent * dir;


	//Make sure port is included at startup
	if (argc != 2)
	{
		printf("Please enter ./ftserver [port #] to run this script!\n");
		exit (1);
	}

	memset(&infor, 0, sizeof infor);
	// get address infomration
	infor.ai_family = AF_INET;
	infor.ai_socktype = SOCK_STREAM;
	infor.ai_flags = AI_PASSIVE;

	//add address information to results variable
	if(getaddrinfo(NULL, argv[1], &infor, &results))
	{
		printf("Error gathering the correct IP/Port number\n");
		exit(1);
	}

	//create the socket
	if((socketNum = socket(results->ai_family, results->ai_socktype, results->ai_protocol)) == -1)
	{
		printf("Cannot create socket\n");
		exit(1);
	}

	//bind socket
	if(bind(socketNum, results->ai_addr, results->ai_addrlen) == -1)
	{
		close(socketNum);
		printf("Cannot bind socket. Try another port.\n");
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
		printf("Server open on %s\n", argv[1]);

	//Wait for connection and accept request from client
	while(1)
	{
		addr_size = sizeof(their_addr);
		if((new_fd = accept(socketNum, (struct sockaddr *)&their_addr, &addr_size)) == -1)
			continue;

		// get the command from the client
        	memset(command,0,sizeof(command));
        	recv(new_fd, command, sizeof(command)-1, 0);

		// return mesage of receipt
		send(new_fd, standardResponse, strlen(standardResponse),0); 

		// get the port number 
		memset(port, 0, sizeof(port)); 
        	recv(new_fd, port, sizeof(port)-1, 0); 
   
		// return message of receipt
		send(new_fd, standardResponse, strlen(standardResponse),0);
        
		// receive the hostname of the client
        	memset(hostname,0,sizeof(hostname));
        	recv(new_fd, hostname, sizeof(hostname),0);

		// return message of receipt
		send(new_fd, standardResponse, strlen(standardResponse),0);
 
		// print that we have a connection
        	printf("Connection from %s\n", hostname);

		// Create domain name
		memset(location,0,sizeof(location));
		sprintf(location, "%s.engr.oregonstate.edu", hostname);

		//clear the file array so we have a blank slate
		for (i=0; i < 100; i++)
			memset(file[i], 0, sizeof(char)*25);

		
		// Get file names and put them into the array 
		directory = opendir(".");
		num_files = 0;
		if (directory)
		{
			while ((dir = readdir(directory)) != NULL)
			{
				//check to make sure they are regular files
				//http://man7.org/linux/man-pages/man3/readdir.3.html
				if (dir->d_type == DT_REG)
				{
					
					strcpy(file[num_files], dir->d_name);
					files[num_files]=file[num_files];
			 		num_files++;
 				}
			}
		closedir(directory);
		}

		// Get socket ready to deliver by creating socket then connecting socket
        	res = socket_2_info(location, port);
        	if ((data_socket = (socket(res->ai_family, res->ai_socktype, res->ai_protocol))) == -1)
		{
                	printf("Error in creating socket\n");
                	exit(1);
        	}
		
		if ((i = connect(data_socket, res->ai_addr, res->ai_addrlen)) == -1)
		{
                	printf("Error in connecting socket\n");
                	exit(1);
        	}

		// Handle 'l' if commanded too
		if(strcmp(command,"l") == 0)
		{
			printf("List directory requested on port %s\n", port);
			printf("Sending directory contents to %s:%s\n", hostname, port);

        		for (i=0; i < num_files; i++)
                		send(data_socket, files[i],25,0);
        		
			send(data_socket, done, strlen(done),0);
 		}
        
		else if(strcmp(command, "g") == 0)
		{
	
			// open up to receive filename and print information
			memset(filename, 0, sizeof(filename));
			recv(new_fd, filename, sizeof(filename)-1,0);
			printf("File \"%s\" requested on port %s.\n", filename, port);
	
			// Check to see if the file is in the directory
			file_exist=0;
			for(i=0; i < num_files; i++)
			{
				if(strcmp(filename, files[i]) == 0)
				{
					file_exist = 1;
					break;
				}
			}			

			// If in directory send file, if not send error
			if(file_exist)
			{
				printf("Sending \"%s\" to %s:%s\n", filename, hostname, port);
				send(new_fd, file_found, strlen(file_found),0);
			
				//Open file
				fd = open(filename, O_RDONLY);
        			while (1) 
				{
					memset(buf, '\0', sizeof(buf));
                			i = read(fd, buf, sizeof(buf)-1);
                			if (i == 0)
                	        		break;

                			if (i < 0) 
					{
                        			printf("Error reading file\n");
			 			return;
					}

					point_to_buf = buf;
					while (i > 0) 
					{
						if ((nbytes = send(data_socket, point_to_buf, sizeof(buf)-1,0)) < 0)
						{
							printf("Error writing to socket\n");
							return;
						}
						i = i -  nbytes;
					}
				}

				close(fd);
				//Send a "done" message to client when the file is finished
				memset(buf, '\0', sizeof(buf));
				strcpy(buf, "__done__");
				send(data_socket, buf, sizeof(buf),0);
                	}
			else
			{
				printf("File not found. Sending error message to %s:%s\n", hostname, argv[1]);
				send(new_fd, file_not_found, 100, 0);
                	}
	        }
	        else
		{
	                send(new_fd, errorResponse, strlen(errorResponse), 0);
	                printf("Invalid command sent\n");
	        }
	
		
        	close(data_socket);
    		freeaddrinfo(res);
		close(new_fd);
	}		

	//free
	freeaddrinfo(results);
	return 0;
}



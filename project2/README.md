Author: Matthew Solbrack
Date: 08/12/2019
Project: Program 2
Class: CS372/400 Intro to Computer Netwroks

To run this ft application please put files on server and type "make".

On one terminal (flip1 to flip3) type the following commnad (add the port number):

./ftserver.c [port#]

On another terminal (flip1 to flip3) type the following commands (add the port number and flip server used on the previous terminal) for different functionalities:

To check the files in directory [command -l]
./ftclient.py [server] [port# of server] -l [different port#]

To copy a file over to client server [command -g]
./ftclient.py [server] [port# of server] -g [file name] [different port#]

To check a bad file nmae [command -g]
./ftclient.py [server] [port# of server] -g [bad file name] [different port#]
                                                                                                                                                                                                          
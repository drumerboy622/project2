#!/bin/python

######################################################################################
# Author: Matthew Solbrack
# Date Last Modified: August 12, 2019
# Program Name: ftclient.py
# Course Name: Intro to Computer Networks (CS372/400)
# Program Description: This program servers as the client for a network application
#  that transfers files and displays directory file names. The program uses
#  information gathered from https://docs.python.org/release/2.6.5/library/internet.html,
#  from program 1 of this class, and lecture material. 
#  
##############################################################################i

import socket
import sys

if __name__ == "__main__":
	# Make sure there are enough arguments
	if len(sys.argv) < 5 or len(sys.argv) > 6:
		print "Please enter ./ftclient.py <flip#> <port#> <either '-l' or ('-g <filename>') > <port#>"
		exit(1)
	
	# create variable for server port
	serverport = sys.argv[2]

	# create variable for location of server 
	servername = sys.argv[1]+".engr.oregonstate.edu"

	# create a socket
	clientsocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

	# connect the socket
	clientsocket.connect((servername, int(serverport)))


	# Send message to client user and send command to server
	if sys.argv[3] == "-l":
		print ("Receiving directory structure from {}:{}".format(sys.argv[1], sys.argv[4]))

		clientsocket.send("l") # send to server
		command = 4
	elif sys.argv[3] == "-g":
		print ("Receiving \"{}\" from {}:{}".format(sys.argv[4], sys.argv[1], sys.argv[5]))

        	clientsocket.send("g") # send to server
		command = 5

	# receive notification and proceed
	clientsocket.recv(1024)

	# Send socket port number
	clientsocket.send(sys.argv[command])
	
	# receiver notification and proceed
	clientsocket.recv(1024)
	
	# Get hostname information and send it to server
	# reference: saltycrane.com/blog/2011/11/how-get-username-home-directory-and-hostname-python/
	hostname = socket.gethostname() [0:5]
	clientsocket.send(hostname)

	# receiver notification and proceed
	clientsocket.recv(1024)

	# check to make sure file can be found by server
    	if command == 5:
        	clientsocket.send(sys.argv[4])
        	response = clientsocket.recv(1024)
        	if response != "File found":
			print "Client responded with 'File not found message'"
			exit(1)

	# Create 2nd socket to receive information
	clientport = sys.argv[command]
	client2socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	client2socket.bind(('', int(clientport)))
	client2socket.listen(1)
	data_socket, addr = client2socket.accept()

	# if 'l' receive data from server and print to screen
	if command == 4:
		fileName = data_socket.recv(200)
		while fileName != "done":
        		print fileName
        		fileName = data_socket.recv(200)

	# if 'g', create file and add contents sent from server to it
	else:
		f = open(sys.argv[4],"w")
		buff = data_socket.recv(1600)
		while "__done__" not in buff:
			f.write(buff)
			buff = data_socket.recv(1600)
		print "File transfer complete."

	data_socket.close()


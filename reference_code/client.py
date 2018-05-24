"""
simple python code for testing socket communication with the server c program
"""


import socket


host = '127.0.0.1'
port = 8989
 
mySocket = socket.socket()
mySocket.connect((host,port))
 
message = input(" -> ")
 
while message != 'q':
	print("trying to send {}".format(message))
	mySocket.send(message.encode())
	data = mySocket.recv(1024).decode()
	print('Received from server: ' + data)
	message = input(" -> ")
         
mySocket.close()


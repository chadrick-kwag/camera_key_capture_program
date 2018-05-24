# this program will capture the image from webcam and capture the keypress in keyboard
# at the same time and save it in two different directories.
# the invertal is controlled by the `set_captureflag` function.

# author: chadrick
# please run the keyserver program first.


import socket
import time
import numpy as np
import cv2, threading, copy, os


IMAGEOUT='images'
ANNOTATIONOUT='annotations'


commonframe = None
lock=threading.Lock()

captureflag = True

def set_captureflag():
	time.sleep(0.1) # the invertal is 0.1 second
	global captureflag
	captureflag = True



def setup_server_connection():
	host = '127.0.0.1'
	port = 8989
	 
	# mySocket = socket.socket()
	# mySocket.connect((host,port))
	# print("default timeout={}".format( socket.getdefaulttimeout() ))
	mySocket= socket.create_connection((host,port))
	# print("socket connect result={}".format(mySocket))

	return mySocket
	


def setup_camera():
	cap = cv2.VideoCapture(0)

	return cap

## main

try:
	cap = setup_camera()
	socket = setup_server_connection()

	
	print("starting code")

	while True:
		# global commonframe,lock

		if captureflag:
			captureflag = False
			_,frame = cap.read()
			cv2.imshow('www',frame)

			cv2.waitKey(1)
			
			message='a'
			socket.send(message.encode())
			data=socket.recv(1024).decode()
			print("recv: {}".format(data))

			t = threading.Thread(target=set_captureflag)
			t.start()
		else:
			# simply consume the frames
			_,frame = cap.read()



	# end of while loop

	socket.close()
	cap.release()
	cv2.destroyAllWindows()
	
	print("finished")

except KeyboardInterrupt:
	print("exiting")
	socket.close()
	cap.release()
	cv2.destroyAllWindows()
	













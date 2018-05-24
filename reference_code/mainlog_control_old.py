# logging

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

class camerathread (threading.Thread):
	def __init__(self,cap):
		threading.Thread.__init__(self)
		self.cap=cap
		self.stopflagopposite=True
      
	def run(self):
		while self.stopflagopposite:
			_,frame = self.cap.read()
			global commonframe,lock
			lock.acquire()
			commonframe=copy.deepcopy(frame)
			lock.release()
			# print("camerathread while loop done")

	def stop(self):
		self.stopflagopposite=False

      


def setup_server_connection():
	host = '127.0.0.1'
	port = 8989
	 
	# mySocket = socket.socket()
	# mySocket.connect((host,port))
	# print("default timeout={}".format( socket.getdefaulttimeout() ))
	mySocket= socket.create_connection((host,port))
	# print("socket connect result={}".format(mySocket))

	

	return mySocket
	 
	
 
	# while message != 'q':
	# 	print("trying to send {}".format(message))
	# 	mySocket.send(message.encode())
	# 	data = mySocket.recv(1024).decode()
	# 	print('Received from server: ' + data)
	# 	message = input(" -> ")


	# mySocket.close()


def setup_camera():
	cap = cv2.VideoCapture(0)

	# print("return cap value {}".format(cap))
	return cap

## main

try:
	cap = setup_camera()
	socket = setup_server_connection()

	# print("socket: {}".format(socket))
	# message=input(">>")

	camthr = camerathread(cap)

	camthr.start()



	# while True:
	# 	# capture camera
	# 	_,frame = cap.read()
	# 	cv2.imshow('www',frame)
	# 	cv2.waitKey(1)

	# 	message='a'
	# 	socket.send(message.encode())
	# 	data=socket.recv(1024).decode()
	# 	print("recv: {}".format(data))

		# time.sleep(5)

	while True:
		# global commonframe,lock
		lock.acquire()
		if commonframe is not None:

			cv2.imshow('www',commonframe)

			cv2.waitKey(1)
		lock.release()

		message='a'
		socket.send(message.encode())
		data=socket.recv(1024).decode()
		print("recv: {}".format(data))

		time.sleep(0.1)





	# while message!='q':
	# 	print("inside while loop")
	# 	socket.send(message.encode())
	# 	print("after send")
	# 	data = socket.recv(1024).decode()
	# 	print("recv: {}".format(data))
	# 	message=input(">>")

	socket.close()
	cap.release()
	cv2.destroyAllWindows()
	camthr.stop()
	print("finished")

except KeyboardInterrupt:
	print("exiting")
	socket.close()
	cap.release()
	cv2.destroyAllWindows()
	camthr.stop()

# message=input(">>")
# while message!='q':
# 	print("inside while loop")
# 	socket.send(message.encode())
# 	print("after send")
# 	data = socket.recv(1024).decode()
# 	print("recv: {}".format(data))
# 	message=input(">>")











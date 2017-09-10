#!/usr/bin/env python
#coding=utf-8

import sys
from socket import *
import time


class UdpClient:
	
	def __init__(self):
		self.HOST = "localhost"
		
		self.PORT = 8000
		
		self.ADDR = (self.HOST, self.PORT)
		
		try:
			#创建套接字
			print "Creating socket"
			self.client = socket(AF_INET, SOCK_DGRAM)
			
			#设置可复用
			self.client.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
			print "done"
		except error, e:
			print "Strange error creating socket: %s" % e
			sys.exit(1)
		
		while True:
			print "Send Data to %s ", self.ADDR
			
			try:
				self.client.sendto("Hello, this is a test info !", self.ADDR)
			except error, e:
				print "Send Error : %s" % e
				sys.exit(1)
			
			time.sleep(1)
		
if __name__=="__main__":
	client = UdpClient()
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

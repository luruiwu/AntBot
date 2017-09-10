#!/usr/bin/env python
#coding=utf-8

import sys
import json

from socket import *


class UdpServer:
	
	def __init__(self):
	
		self.PORT = 8000
		
		self.HOST = ""
		
		self.ADDR = (self.HOST, self.PORT)
		
		try:
			#数据分析
			self.server = socket(AF_INET, SOCK_DGRAM)
			
			#告诉内核套接字在TIME_WAIT时间内可以重新使用
			self.server.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
		except error, e:
			print "Creating socket error !"
			sys.exit(1)
		
		try:
			print "binding ..."
			self.server.bind(self.ADDR)
		except error, e:
			print "bind error"
			sys.exit(1)
	
	
		print "Server is running on port %d; press Ctrl-C to terminate."%self.PORT
	
		while True:
	
			#等待客户连接
			print "waiting for client ..."
		
			while True:
				#接受数据
				buf = self.server.recv(1024)
				if not len(buf):
					break
				print "Recv from client :  ",buf
		
			#断开连接
			self.server.close()
			print "connection closed !"
			
			
if __name__=="__main__":
	
	server = UdpServer()
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

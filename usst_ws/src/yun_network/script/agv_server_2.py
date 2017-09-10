#!/usr/bin/env python
#coding=utf-8

import sys
import rospy
import binascii
from socket import *

class UdpServer:
	
	def __init__(self):
	
		rospy.init_node("network_server", anonymous = False)
		
		rospy.on_shutdown(self.shutdown)
	
		self.PORT = 8090
		
		self.HOST = ""
		
		self.ADDR = (self.HOST, self.PORT)
		
		self.client_addr = ""
		
		self.i = 0
		##CCS
		self.CCS_DATA ="1332000653444950413203C4025800044147563100044155544F000C646973636F6E6E6563746564000473687264000430303030"
		
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
				buf, self.client_addr = self.server.recvfrom(1024)
				if not len(buf):
					break
				str_1 = binascii.b2a_hex(buf).encode("utf8")
				print "[%d] Recv from client:"%self.i, str_1
				self.i = self.i + 1
				
				#判断是否接受到CCS请求
				if buf == self.CCS_DATA.decode('hex'):
					print "CCS_ECHO"
					self.CCS_ECHO()
				else:
					print "No CCS_ECHO"
				
			#断开连接
			self.server.close()
			print "connection closed !"
	def CCS_ECHO(self):
	
		CCS_ECHO = "20020000".decode('hex')
		
		#发送反馈信息
		self.server.sendto(CCS_ECHO, self.client_addr)
		
			
	def shutdown(self):
		print "network_server is shutdown!\n"
		
if __name__=="__main__":
	
	try:
		UdpServer()
	except:
		print "Error"
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

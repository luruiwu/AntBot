#!/usr/bin/env python
#coding=utf-8

"""
如何区分不同的命令，通过命令字标识吗？
区分度太低。

"""


import sys
import rospy
import time
import binascii
import json # for JSON
from socket import *

device_cmd = {
	"action":move,
	"x":200,
	"y":300
}

#CSS to AGV
ConfirmRecHeartbeat = "D000"



class UdpClient:
	
	def __init__(self):
	
		rospy.init_node("network_client", anonymous = False)
		
		rospy.on_shutdown(self.shutdown)
		
		self.HOST = "192.168.1.2"
		
		self.PORT = 8000
		
		self.ADDR = (self.HOST, self.PORT)
		
		self.i = 0
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
		
		#连接
		self.client.connect(self.ADDR)
		
		while True:
			
			#发送CCS连接请求
			self.CCS()
			
			#读取服务器的数据
			data, addr = self.client.recvfrom(1024)
			
			#判断数据
			i = 0
			while True:
				if(data[i]) == 0xD000:
					print "CSS ---> AGV: CSS ConfirmRecHeartbeat\n"
				if(data[i]) == 0x32:
					print "CSS ---> AGV: CSS DeviceCmd\n"
				if(data[i]) == 0x40:
					print "CSS ---> AGV: CSS ConfirmRecDeviceState\n"
				
				
				i = i + 1
				if(len(data) == i):
					break
				
			print "Recv:",binascii.b2a_hex(data).encode("utf8"), "from: ", addr
			
			time.sleep(1)
	
	#连接通信控制系统
	def CCS(self):
		#16进制CCS连接命令，字符转换为16进制
		data = "1332000653444950413203C4025800044147563\
				100044155544F000C646973636F6E6E65637465\
				64000473687264000430303030".decode("hex")
		
		print "[%d] SendData: "%self.i, data
		
		#发送到主机
		try:
			self.client.sendto(data, self.ADDR)
		except error, e:
			print "Send Error: %s" %e
	
		self.i = self.i + 1
	
	#与CCS断开连接
	def BreakCCS(self):
		print "BreakCCS"
		data = "E000".decode("hex")
		
		self.client.sendto(data, self.ADDR)
		
	#发送设备状态
	def SendDeviceStates(self):
		print "SendDeviceStates"
		
		device_state = {
			"state":0,
			"power":100,
			"direction":0,
			"x":100,
			"y":200,
			"IR.state":0,
			"IR.hand":1
		}
		#转换为str
		str = json.dumps(device_state)
		
		#发送到主机
		try:
			self.client.sendto(data, self.ADDR)
		except error, e:
			print "Send Error: %s" %e
		
	
	#确认接受到设备命令
	def ConfirmDeviceCmd(self):
		print "ConfirmDeviceCmd"
		
		data = "40020001".decode("hex")
		
		self.client.sendto(data, self.ADDR)
	
	#发送心跳包
	def SendHeartbeat(self):
		print "SendHeartbeat"
		
		while True:
			
			data = "C000".decode("hex")
			
			time.sleep(10)
			
	#解码数据包剩余数据长度
	def Decoder(self ,buff):
		print "Decoder"
		m = 1
		length = 0
		d = 0
		i = 0
		while True:
			d = buff[i]
			length = length + (d & 0x7F) * m
			m = m * 0x80
			if((d & 0x80) == 0):
				break
				
		return length
	
	#编码数据包剩余数据长度
	def Encoder(self, length):
		print "Encoder"
		
		buff = []
		i = 0
		x = length
		d = 0
		while True:
			d = x % 0x080
			x = x / 0x80
			if(x > 0):
				d = d | 0x80
			buff[i] = d
			i++
			if (x <= 0):
				break
				
		#返回编码结果
		return buff
	
	def shutdown(self):
		print "network_client is shutdown!\n"
		
if __name__=="__main__":
	try:
		UdpClient()
	except:
		print "Error"
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

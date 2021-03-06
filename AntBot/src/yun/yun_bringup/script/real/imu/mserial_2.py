#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial 
import thread

class MSerialPort(threading.Thread):
	def __init__(self,port,buand,timeout):
		threading.Thread.__init__(self)
		try:
			self.port=serial.Serial(port,buand)  
			self.port.timeout=timeout
			if not self.port.isOpen():
				self.port.open()
		except Exception,msg:
				raise Exception("串口出错")
	def run(self):
		
	   
	def in_waiting(self) :
		return self.port.in_waiting
	def port_open(self):
		if not self.port.isOpen():
			self.port.open()  
	def port_close(self):
		self.port.close()
	def send(self,data):
		self.port.write(data)  
	def readLine(self):
		data=self.port.readline()
		return data
	def thread_read(self):
		thread.start_new_thread(self.readLine,())
		return data
	def read(self,count):
		try:
			data=self.port.read(count)
			return data
		except Exception as e:
			return '#'
			
			
			
			
			

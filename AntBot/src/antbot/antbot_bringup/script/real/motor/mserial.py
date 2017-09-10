#!/usr/bin/env python
import serial 
import thread
class MSerialPort:
	def __init__(self,port,buand,timeout):
		self.port=serial.Serial(port,buand)  
		self.port.timeout=timeout
		if not self.port.isOpen():
			self.port.open()
		#thread.start_new_thread(self.read_data,())    
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
	def read(self,count):
		try:
			data=self.port.read(count)
			return data
		except Exception as e:
			return '#'
			

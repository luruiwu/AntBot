#!/usr/bin/env python
#coding=utf-8
import serial
import time
import threading

	#创建threading.Thread的子类SerialData
class SerialData(threading.Thread):
	#初始化线程
	def __init__(self):
		threading.Thread.__init__(self)
	#打开串口
	def open_com(self,port,baud):
		self.ser = serial.Serial(port,baud,timeout=0.5)
		return self.ser
	#判断串口是否打开
	def com_isopen(self):
		return self.ser.isOpen()
	#发送数据
	def send_data(self,send_data):
		self.ser.write(send_data)
	#读取一行数据
	def readLine(self):
		rece_data = self.ser.readline()
		return rece_data
	#读取count个数据
	def read(self,count):
		rece_data = self.ser.read(count)
		return rece_data
	#关闭串口
	def close_com(self):
		return self.ser.close()

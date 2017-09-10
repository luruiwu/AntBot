#!/usr/bin/env python
#coding=utf-8
"""
2017-01-11 by Bob

"""
import rospy
import time
import logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s- %(message)s')
from antbot_bringup_2.msg 		import Encoder
from geometry_msgs.msg 			import Twist
from mserial 					import MSerialPort 
from std_msgs.msg 				import String
from math 						import sin, cos, pi, radians


class Motor():
	def __init__(self):
		
		rospy.init_node("Motor", anonymous=True)
		
		rospy.on_shutdown(self.nodeShutdown)
		
		r = rospy.Rate(50)
		
		self.serialport = rospy.get_param("~port", "/dev/ttyUSB0")
		self.baudrate   = rospy.get_param("~baudrate", 115200)
		self.timeout	= rospy.get_param("~timeout", 0.2)
		self.serial		= MSerialPort(self.serialport, self.baudrate, self.timeout)
		
		#订阅速度话题
		rospy.Subscriber("/cmd_vel", Twist, self.cmd_callback)
		
		#发布编码器数据
		self.pub = rospy.Publisher("/encoder", Encoder, queue_size=1)
		
		encoder_msg = Encoder()
		encoder_msg.encoder = [0 for i in range(4)]
		
		#判断第一次
		self.flag = True
		
		self.cmd_msg = Twist()
		
		#前后轮距的一半
		self.length = 0.65
		#左右轮距的一半
		self.width = 0.65
		#麦克纳姆轮的半径
		self.radius = 0.076#76.2mm
		#四个电机的速度
		self.w = [0 for j in range(4)]
		#命令
		self.SS_CMD = [0 for i in range(14)]
		self.SS_CMD[0] = 0x54
		self.SS_CMD[1] = 0x45
		self.SS_CMD[2] = 0x0B
		self.SS_CMD[3] = 0x53
		self.SS_CMD[4] = 0x53
		
		self.GE_CMD = [0x54, 0x45, 0x02, 0x47, 0x45]
		
		#接收缓存
		self.rec_buf = [0]*100
		
		#电机的编码器数据
		self.Motor_Encoder_Last = [0 for i in range(4)]
		self.Motor_Encoder      = [0 for i in range(4)]
		
		while not rospy.is_shutdown():
			#start_time = time.time()
			r.sleep()
			self.setSpeed()
			self.getEncoder()
			
			encoder_msg.encoder[0] = self.Motor_Encoder[0]
			encoder_msg.encoder[1] = self.Motor_Encoder[1]
			encoder_msg.encoder[2] = self.Motor_Encoder[2]
			encoder_msg.encoder[3] = self.Motor_Encoder[3]
			
			self.pub.publish(encoder_msg)
			
			#end_time = time.time()
			#logging.debug("%d"%(end_time - start_time))
			
	def cmd_callback(self, msg):
		self.cmd_msg = msg 
		#接受到的速度命令
		vx = msg.linear.x
		vy = msg.linear.y
		vth = msg.angular.z
		
		#轮子的速度
		v1 = vx + vy + (self.length + self.width) * vth
		v2 = vx - vy + (self.length + self.width) * vth
		v3 = vx + vy - (self.length + self.width) * vth
		v4 = vx - vy - (self.length + self.width) * vth
		
		#电机应该设置的速度单位rpm
		#w(rpm) = v(m/s) * 60/(2 * PI * R) * 25
		#w = v * (60 /(3.14159 * 6 * 25.4 * 10^-3) * 25)= v * 3132.9
		self.w[0] = int(v1 * 3132.9)
		self.w[1] = int(v2 * 3132.9)
		self.w[2] = int(v3 * 3132.9)
		self.w[3] = int(v4 * 3132.9)
		
		
	def setSpeed(self):
		value = [0 for i in range(8)]
		for i in range(4):
			real = self.w[i] & 0xffff
			high = (real >> 8) & 0x00ff
			low  =  real & 0x00ff
			self.SS_CMD[(i + 2) * 2 + 1] = high#5 7 9 11
			self.SS_CMD[(i + 3) * 2 ] = low
			value[2*i] = high
			value[2*i + 1] = low
		self.SS_CMD[13] = self.checkSum(value)
		self.serial.send(self.arr2chr(self.SS_CMD))
		
		
	def getEncoder(self):
		data = []
		bytes_waiting = 0
		ind = -2
		Motor_Encoder_Diff = [0 for i in range(4)]
		#获取编码器的数据
		self.serial.send(self.arr2chr(self.GE_CMD))
		
		#读取串口缓存中的全部数据
		bytes_waiting = self.serial.in_waiting()
		#print "in_waiting: %f"%bytes_waiting
		self.rec_buf = self.serial.read(bytes_waiting)
		
		GE_ECHO = "%c"*5%(0x45, 0x54, 0x0B, 0x47, 0x45)
		cmd_size = 14
		buf = self.rec_buf
		while True:
			ind = buf.find(GE_ECHO)
			if ind == -1:
				break
			#chr转换为int数据
			data = self.chr2int(buf)
			data_len = len(data)
			#print data[(ind+5):(ind+13)]
			if self.checkSum(data[(ind+5):(ind+13)]) == data[ind+13]:
				#print "checkSum"
				self.Motor_Encoder[0] = (data[ind + 5] << 8) | data[ind + 6]
				self.Motor_Encoder[1] = (data[ind + 7] << 8) | data[ind + 8]
				self.Motor_Encoder[2] = (data[ind + 9] << 8) | data[ind + 10]
				self.Motor_Encoder[3] = (data[ind + 11] << 8) | data[ind + 12]
				
			#将剩下的放入下次循环
			remain = ""
			for j in range(ind + cmd_size, data_len, 1):
				remain += buf[i]
			buf = remain
		
	def checkSum(self, data):
		Sum = 48 + (sum(data) & 0x3F)
		return Sum
		
	def arr2chr(self, arr):
		str1 = ''
		#print arr
		for num in arr:
			str1 += chr(num)
		return str1
	
	def chr2int(self, buf):
		data = []
		for i in range(len(buf)):
			data.append(ord(buf[i]))
		return data
		
	def nodeShutdown(self):
		for i in range(4):
			self.w[i] = 0
		self.setSpeed()
		print "Motor node shutdown"
		
if __name__ == "__main__":
	try:
		print "Motor"
		Motor()
	except:
		rospy.loginfo("Motor node terminated.")
	


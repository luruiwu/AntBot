#!/usr/bin/env python
#coding=utf-8
"""
2017-01-12 by Bob
2017-03-15
	给角速度添加了负号

"""
import rospy
import time
import logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s- %(message)s')
from yun_bringup.msg 			import Encoder
from geometry_msgs.msg 			import Twist
from mserial 					import MSerialPort 
from math 						import sin, cos, pi, radians


class Motor():
	def __init__(self):
		
		rospy.init_node("Motor", anonymous=True)
		
		rospy.on_shutdown(self.nodeShutdown)
		
		self.serialport    = rospy.get_param("~port", "/dev/ttyUSB0")
		self.baudrate      = rospy.get_param("~baudrate", 115200)
		self.timeout       = rospy.get_param("~timeout", 0.2)
		self.serial        = MSerialPort(self.serialport, self.baudrate, self.timeout)
		
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
		self.length = 0.325
		#左右轮距的一半
		self.width = 0.325
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
		self.dis = [0]*4
		self.flag = True
		
		r = rospy.Rate(50)
		print "1"
		while not rospy.is_shutdown():
			#start_time = time.time()
			print "2"
			r.sleep()
			self.setSpeed()
			self.getEncoder()
			
			encoder_msg.encoder[0] = self.Motor_Encoder[0]
			encoder_msg.encoder[1] = self.Motor_Encoder[1]
			encoder_msg.encoder[2] = self.Motor_Encoder[2]
			encoder_msg.encoder[3] = self.Motor_Encoder[3]
			
			self.pub.publish(encoder_msg)
			print "3"
			#end_time = time.time()
			#logging.debug("%d"%(end_time - start_time))
			
	def cmd_callback(self, msg):
		self.cmd_msg = msg 
		#接受到的速度命令
		vx = msg.linear.x
		vy = 0#msg.linear.y
		vth = (-1) * msg.angular.z###
		
		#这里轮子的编码和这不一致
		#轮子的速度(m/s)
		"""
		|w1|    |1 1 -(l1+l2)
		|w2|=1/R|1 -1  (l1+l2)|vx|
		|w3|    |1 -1 -(l1+l2)|vy|
		|w4|    |1 1  (l1+l2)|vth|
		
		w1=vx + vy - vth(l1+l2)
		w2=vx - vy + vth(l1+l2)
		w3=vx - vy - vth(l1+l2)
		w4=vx + vy + vth(l1+l2)
		
		vx = (v[0] + v[1] + v[2] + v[3])/4
		vy = (v[0] - v[1] + v[2] - v[3])/4
		vth = (v[0] + v[1] - v[2] - v[3])/4/(self.length + self.width)
		
		"""
		v = [0]*4
		v[0] = vx + vy + (self.length + self.width) * vth#w4
		v[1] = vx - vy + (self.length + self.width) * vth#w2
		v[2] = vx + vy - (self.length + self.width) * vth#w1
		v[3] = vx - vy - (self.length + self.width) * vth#w3
		
		#电机应该设置的速度单位rpm
		#w(rpm) = v(m/s) * 60/(2 * PI * R) * 25
		#w = v * (60 /(3.14159 * 6 * 25.4 * 10^-3) * 25)= v * 3132.9
		self.w[0] = int(v[0] * 3132.9)
		self.w[1] = int(v[1] * 3132.9)
		self.w[2] = int(v[2] * 3132.9)
		self.w[3] = int(v[3] * 3132.9)
		
		#print "wheel speed :", self.w
		
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
		print "4"
		data = []
		bytes_waiting = 0
		Motor_Encoder_Diff = [0 for i in range(4)]
		#获取编码器的数据
		self.serial.send(self.arr2chr(self.GE_CMD))
		
		#读取串口缓存中的数据
		#bytes_waiting = self.serial.in_waiting()
		#print "in_waiting: %f"%bytes_waiting
		
		#GE_ECHO = "%c"*5%(0x45, 0x54, 0x0B, 0x47, 0x45)
		print "5"
		while not rospy.is_shutdown():
			data = self.serial.read(1)	
			if ord(data) == 0x45:
				data = self.serial.read(1)
				print "7"
				if ord(data) == 0x54:
					data = self.serial.read(3)
					if ord(data[1]) == 0x47 and ord(data[2]) == 0x45:
						data = self.serial.read(9)
						#转换为int
						data = self.chr2int(data)
						if self.checkSum(data[0:8]) == data[8]:
							#print "checkSum succeed"
							self.Motor_Encoder[0] = (data[0] << 8) | data[1]
							self.Motor_Encoder[1] = (data[2] << 8) | data[3]
							self.Motor_Encoder[2] = (data[4] << 8) | data[5]
							self.Motor_Encoder[3] = (data[6] << 8) | data[7]
							print self.Motor_Encoder
							break
							"""
							if self.flag:
								self.Motor_Encoder_Last[0] = self.Motor_Encoder[0]
								self.Motor_Encoder_Last[1] = self.Motor_Encoder[1]
								self.Motor_Encoder_Last[2] = self.Motor_Encoder[2]
								self.Motor_Encoder_Last[3] = self.Motor_Encoder[3]
								self.flag = False
							diff = [0]*4
							diff[0] = (self.Motor_Encoder[0] - self.Motor_Encoder_Last[0])* 7.48 * 1e-5 
							diff[1] = (self.Motor_Encoder[1] - self.Motor_Encoder_Last[1])* 7.48 * 1e-5 
							diff[2] = (self.Motor_Encoder[2] - self.Motor_Encoder_Last[2])* 7.48 * 1e-5 
							diff[3] = (self.Motor_Encoder[3] - self.Motor_Encoder_Last[3])* 7.48 * 1e-5 
							
							for i in range(4):
								self.dis[i] = self.dis[i] + diff[i]
								print "dis[%d] : %f"%(i, self.dis[i])
							
							self.Motor_Encoder_Last[0] = self.Motor_Encoder[0]
							self.Motor_Encoder_Last[1] = self.Motor_Encoder[1]
							self.Motor_Encoder_Last[2] = self.Motor_Encoder[2]
							self.Motor_Encoder_Last[3] = self.Motor_Encoder[3]
							"""
							
		
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
		self.setSpeed()
		print "Motor node shutdown"
		
if __name__ == "__main__":
	try:
		print "Motor"
		Motor()
	except Exception,e:
		print "Motor node shutdown"
		print "Exception", ":", e


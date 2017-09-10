#!/usr/bin/env python
#coding=utf-8
"""
2017-07-1 by Bob
添加AGV的灯光，气阀，电池，充电等的通信。
"""
import rospy
import time
import logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s- %(message)s')
from yun_bringup.msg 			import Encoder, IO_Ctl
from geometry_msgs.msg 			import Twist
from mserial 				import MSerialPort
from math 				import sin, cos, pi, radians
from std_msgs.msg 			import UInt8

class Motor:
	def __init__(self):
		
		rospy.init_node("Motor", anonymous=True)
		
		rospy.on_shutdown(self.nodeShutdown)
		
		self.serialport    = rospy.get_param("~port", "/dev/ttyUSBMotor")
		self.baudrate      = rospy.get_param("~baudrate", 115200)
		self.timeout       = rospy.get_param("~timeout", 0.2)
		self.serial        = MSerialPort(self.serialport, self.baudrate, self.timeout)
		
		encoder_msg = Encoder()
		encoder_msg.encoder = [0 for i in range(4)]
		
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
		self.SS_CMD[3] = ord('S')
		self.SS_CMD[4] = ord('S')
		
		#电机的编码器数据
		self.Motor_Encoder = [0 for i in range(4)]
		
		#电量
		self.battery_msg = UInt8()
		
		self.state_msg = IO_Ctl()
		
		self.GE_CMD = [0x54, 0x45, 0x02, ord('G'), ord('E')]
		#LED
		self.LED_ON = [0x54, 0x45, 0x02, ord('L'), ord('N')]
		self.LED_OFF = [0x54, 0x45, 0x02, ord('L'), ord('F')]
		#Fa
		self.Fa_Up = [0x54, 0x45, 0x02, ord('F'), ord('U')]
		self.Fa_Down = [0x54, 0x45, 0x02, ord('F'), ord('D')]
		#Charge
		self.Start_Charge = [0x54, 0x45, 0x02, ord('S'), ord('C')]
		self.Finish_Charge = [0x54, 0x45, 0x02, ord('F'), ord('C')]
		#Battery
		self.Get_Battery = [0x54, 0x45, 0x02, ord('G'), ord('B')]
		
		#订阅速度话题
		rospy.Subscriber("/cmd_vel", Twist, self.cmd_callback)
		
		#接受Network的控制指令
		rospy.Subscriber("/state", IO_Ctl, self.state_callback)
		
		#发布编码器数据
		self.encoder_pub = rospy.Publisher("/encoder", Encoder, queue_size=1)
		
		#发布电量信息
		self.battery_pub = rospy.Publisher("/battery", UInt8, queue_size=1)
		
		r = rospy.Rate(50)
		
		i = 0
		print "1"
		while not rospy.is_shutdown():
			#start_time = time.time()
			#self.setSpeed()
			self.readSerial()
			
			encoder_msg.encoder[0] = self.Motor_Encoder[0]
			encoder_msg.encoder[1] = self.Motor_Encoder[1]
			encoder_msg.encoder[2] = self.Motor_Encoder[2]
			encoder_msg.encoder[3] = self.Motor_Encoder[3]
			
			#publish encoder
			self.encoder_pub.publish(encoder_msg)
			
			#publish battery
			self.battery_pub.publish(self.battery_msg)
			
			#get battery at 1HZ
			
			if i==50:
				i=0
				self.serial.send(self.arr2chr(self.Get_Battery))
			i = i + 1
			
			#LED ON: STATE：0x01-执行成功；0x02-执行失败
			#LED OFF: STATE：0x01-执行成功；0x02-执行失败
			if state_msg.led == 1:
				self.serial.send(self.arr2chr(self.LED_ON))
			elif state_msg.led == 2:
				self.serial.send(self.arr2chr(self.LED_OFF))
			
			#Fa Up：STATE：0x01-执行成功；0x02-执行失败
			#Fa Down：STATE：0x01-执行成功；0x02-执行失败
			if state_msg.fa == 1:
				self.serial.send(self.arr2chr(self.Fa_Down))
			elif state_msg.fa == 2:
				self.serial.send(self.arr2chr(self.Fa_Up))
			
			#Start Charge：STATE：0x01-连接中；0x02-开始充电；0x03-连接失败
			#Finish Charge: STATE：0x01-断开中；0x02-断开成功；0x03-断开失败
			if state_msg.charge == 1:
				self.serial.send(self.arr2chr(self.Start_Charge))
			elif state_msg.charge == 2:
				self.serial.send(self.arr2chr(self.Finish_Charge))
				
			r.sleep()
			#end_time = time.time()
			#logging.debug("%d"%(end_time - start_time))
		
	def readSerial(self):
		data = []
		bytes_waiting = 0
		Motor_Encoder_Diff = [0 for i in range(4)]
		#获取编码器的数据
		self.serial.send(self.arr2chr(self.GE_CMD))
		
		#读取串口缓存中的数据
		#bytes_waiting = self.serial.in_waiting()
		#print "in_waiting: %f"%bytes_waiting
		
		#GE_ECHO = "%c"*5%(0x45, 0x54, 0x0B, 0x47, 0x45)
		while not rospy.is_shutdown():
			data = self.serial.read(1)
			if len(data) != 0 and ord(data) == 0x45:
				data = self.serial.read(1)
				if len(data) !=0 and ord(data) == 0x54:
					data = self.serial.read(1)
					#read rest data
					if len(data) != 0:
						data = self.serial.read(ord(data))
					data = self.chr2int(data)
					#GE_CMD
					if data[0] == ord('G') and data[1] == ord('E'):
						#将读取的数据全部转换为int
						#print "data:", data
						if self.checkSum(data[2:10]) == data[10]:
							#print "checkSum succeed"
							self.Motor_Encoder[0] = (data[2] << 8) | data[3]
							self.Motor_Encoder[1] = (data[4] << 8) | data[5]
							self.Motor_Encoder[2] = (data[6] << 8) | data[7]
							self.Motor_Encoder[3] = (data[8] << 8) | data[9]
							print self.Motor_Encoder
							break
					
					#Led On
					elif data[0] == self.LED_ON[3] and data[1] == self.LED_ON[4]:
						print "LED ON"
						
					#Led Off
					elif data[0] == self.LED_OFF[3] and data[1] == self.LED_OFF[4]:
						print "LED OFF"
					
					#Fa Up
					elif data[0] == self.Fa_Up[3] and data[1] == self.Fa_Down[4]:
						print "Fa_Up"
						
					#Get_Battery 
					elif data[0] == self.Get_Battery[3] and data[1] == self.Get_Battery[4]:
					                #STATE：用0x0-0x64(即十进制的0-100)表示电池电量0% - 100%
						print "Get_Battery"
						self.battery_msg.data = data[2]
					
					#Start_Charge
					elif data[0] == self.Start_Charge[3] and data[1] == self.Start_Charge[4]:
						print "Start_Charge"
						
					#Finish_Charge
					elif data[0] == self.Finish_Charge[3] and data[1] == self.Finish_Charge[4]:
						print "Finish_Charge"
						
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
		print "self.SS_CMD", self.SS_CMD
		self.serial.send(self.arr2chr(self.SS_CMD))
		
	def state_callback(self, msg):
		print "state"
		
		self.state_msg = msg
	
	def cmd_callback(self, msg):
		self.cmd_msg = msg
		#接受到的速度命令
		vx = msg.linear.x
		vy = 0#msg.linear.y
		vth = (-1) * msg.angular.z###
		
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
		
		#print "wheel speed :", self.w
		
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
		self.setSpeed()
		print "Motor node shutdown"
		
if __name__ == "__main__":
	try:
		print "Motor"
		Motor()
	except Exception, e:
		print Exception, ":", e
	


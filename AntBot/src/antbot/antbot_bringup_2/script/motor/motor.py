#!/usr/bin/env python
#coding=utf-8
"""
2016-12-28
"""
import rospy
import time
import logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s- %(message)s')
from antbot_bringup_2.msg 		import Imu
from geometry_msgs.msg 			import Twist
from mserial 					import MSerialPort 
from std_msgs.msg 				import String
from math 						import sin, cos, pi, radians

global count
count = 0

class Motor():
	def __init__(self):
		
		rospy.init_node("Motor", anonymous=True)
		
		rospy.on_shutdown(self.nodeShutdown)
		
		r = rospy.Rate(50)
		
		self.serialport = rospy.get_param("~port", "/dev/ttyUSB0")
		self.baudrate   = rospy.get_param("~baudrate", 115200)
		self.timeout	= rospy.get_param("~timeout", 0.2)
		self.serial = MSerialPort(self.serialport, self.baudrate, self.timeout)
		
		#发布编码器数据
		self.encoder = rospy.Publisher("/encoder", String, queue_size=1)
		
		#订阅Imu消息
		rospy.Subscriber("/Imu_msg", Imu, self.imu_callback)
		
		#订阅cmd_vel速度
		rospy.Subscriber("/cmd_vel", Twist, self.cmd_callback)
		
		self.imu_msg  = Imu()
		self.current_angle_z = 0
		self.last_angle_z = 0
		#判断第一次
		self.flag = True
		
		self.cmd_msg = Twist()
		self.xPosition = 0
		self.yPosition = 0
		self.thPosition = 0
		
		#前后轮距的一半
		self.length = 0.65
		#左右轮距的一半
		self.width = 0.65
		#麦克纳姆轮的半径
		self.radius = 0.076#76.2mm
		#四个电机的速度
		self.w = [1000 for j in range(4)]
		#命令
		self.SS_CMD = [0 for i in range(14)]
		self.SS_CMD[0] = 0x54
		self.SS_CMD[1] = 0x45
		self.SS_CMD[2] = 0x0B
		self.SS_CMD[3] = 0x53
		self.SS_CMD[4] = 0x53
		
		self.GE_CMD = [0x54, 0x45, 0x02, 0x47, 0x45]
		
		#电机的编码器数据
		self.Motor_Encoder_Last = [0 for i in range(4)]
		self.Motor_Encoder      = [0 for i in range(4)]
		self.Motor_Encoder_Dis  = [0 for i in range(4)]
		
		while not rospy.is_shutdown():
			start_time = time.time()
			r.sleep()
			self.setSpeed()
			self.getEncoder()
			#self.calculateOdom()
			end_time = time.time()
			#logging.debug("%d"%(end_time - start_time))
			
	def cmd_callback(self, msg):
		
		self.cmd_msg = msg
		#打印
		"""
		rospy.loginfo("Received a /cmd_vel message!")
		rospy.loginfo("Linear Components: [%f, %f, %f]"%(msg.linear.x, msg.linear.y, msg.linear.z))
		rospy.loginfo("Angular Components: [%f, %f, %f]\n"%(msg.angular.x, msg.angular.y, msg.angular.z))
		"""      
		#接受到的速度命令
		vx = msg.linear.x
		vy = msg.linear.y
		vth = msg.angular.z
		
		#轮子的速度
		v1 = vx + vy + (self.length + self.width) * vth
		v2 = vx - vy - (self.length + self.width) * vth
		v3 = vx + vy - (self.length + self.width) * vth
		v4 = vx - vy + (self.length + self.width) * vth
		
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
		global count
		data = []
		Motor_Encoder_Diff = [0 for i in range(4)]
		#获取编码器的数据
		self.serial.send(self.arr2chr(self.GE_CMD))
		data = self.serial.read(2)
		#判断数据头
		if ord(data[0]) == 0x45 and ord(data[1]) == 0x54:
			data = self.serial.read(3)
			#'GE'
			if ord(data[1]) == 0x47 and ord(data[2]) == 0x45:
				data = self.serial.read(9)
				data = self.chr2int(data)
				print len(data), data
				#print self.checkSum(data[0:8]), data[-1]
				if self.checkSum(data[0:8]) == data[-1]:
					#print "checkSum succeed"
					self.Motor_Encoder[0] = (data[0] << 8) | data[1]
					self.Motor_Encoder[1] = (data[2] << 8) | data[3]
					self.Motor_Encoder[2] = (data[4] << 8) | data[5]
					self.Motor_Encoder[3] = (data[6] << 8)| data[7]
					
					if self.flag:
						self.flag = False
						print "ddddddddddddddd"
						for i in range(4):
							self.Motor_Encoder_Last[i] = self.Motor_Encoder[i]
						#print self.Motor_Encoder_Last
					count += 1
					#print "calculation[%d]"%count, self.Motor_Encoder
					#编码器脉冲数转换为轮子的移动距离
					#Motor_Distance = Motor_Encoder_Diff/(256*25) * (2*PI*R) = 
					#Motor_Encoder_Diff 1.0/(256*25) * (6*3.14159*25.4*1e-3) = 7.48*10^-5 m 
					for i in range(4):
						#脉冲差
						
						Motor_Encoder_Diff[i] = self.Motor_Encoder[i] - self.Motor_Encoder_Last[i]
						
						if Motor_Encoder_Diff[i] < -10000:
							Motor_Encoder_Diff[i] = Motor_Encoder_Diff[i] + 60000
						elif Motor_Encoder_Diff[i] > 10000:
							Motor_Encoder_Diff[i] = Motor_Encoder_Diff[i] - 60000
							
						#print "Diff %d:%f"%(i,Motor_Encoder_Diff[i])
						#累计移动距离
						self.Motor_Encoder_Dis[i] += Motor_Encoder_Diff[i] * 7.48 * 1e-5
						#print "Motor_Encoder_Dis[%d]: %f"%(i, self.Motor_Encoder_Dis[i])
						#迭代
						self.Motor_Encoder_Last[i] = self.Motor_Encoder[i]
					
	
	def calculateOdom(self):
		print "Odom"
		self.current_angle_z = self.imu_msg.angle_z
		delta_th = self.current_angle_z - self.last_angle_z
		s = [0] * 4
		for i in range(4):
			s[i] = self.Motor_Encoder_Dis[i] * cos(radians(45.0))
				
		#延x轴走
		if self.cmd_msg.linear.y == 0:
			#机器人坐标系统的变化
			s_m = (s[1] + s[2])/2.0
			i = s_m * cos((180-delta_th)/2.0)
			j = s_m * sin((180-delta_th)/2.0)
			#映射到世界坐标系的变化
			delta_x = i * cos(self.thPosition) - j * sin(self.thPosition)
			delta_y = i * sin(self.thPosition) - j * cos(self.thPosition)
			
		#延y轴走
		else:
			#机器人坐标系统的变化
			s_m = (s[1] + s[4])/2.0
			i = s_m * cos((180-delta_th)/2.0)
			j = s_m * sin((180-delta_th)/2.0)
			#映射到世界坐标系的变化
			delta_x = i * cos(self.thPosition) - j * sin(self.thPosition)
			delta_y = i * sin(self.thPosition) - j * cos(self.thPosition)
		
		self.xPosition += delta_x
		self.yPosition += delta_y
		self.thPosition += delta_th
		
		#迭代
		self.last_angle_z = self.current_angle_z
		
	def imu_callback(self, msg):
		#读取航向角度
		self.imu_msg.angle_z = msg.angle_z
		print "angle_z: %f"%(msg.angle_z / (pi) * 180)
		
	def checkSum(self, data):
		Sum = 48 + (sum(data) & 0x3F)
		return Sum
	"""
	数cmd一般都是ASCII码，需要经过chr或者“%c”命令的转化
	比如说串口发送: 03 03 00 00 04命令 cmd  = "%c" * 5 % (0x03, 0x03, 0, 0, 0x04)
	
	"""
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
	


#!/usr/bin/env python
# -*- coding: utf-8 -*-

#电机的直接驱动文件 接受cmdvel信息 发送给电机 并获得所有电机的速度 合成之后 发布到car_speed话题上
#


import roslib; roslib.load_manifest('antbot_bringup') 
import rospy
import tf.transformations
from geometry_msgs.msg import Twist
from MotorDriver.mserial import MSerialPort
import MotorDriver.MotorSpeedvHandler
import MotorDriver.Motor
import MotorDriver.mType


serialport=rospy.get_param("~serial_port",'/dev/ttyUSBMotor')
baundrate=rospy.get_param("~serial_baudrate",57600)
timeout=rospy.get_param("~timeout",0.2)
mSerial=MSerialPort(serialport,baundrate,timeout)

VX=0.0
VY=0.0
VZ=0.0

#监听'cmd_vel' 话题上的速度消息
def callback(msg):
	global VX
	global VY
	global VZ
	try:
		'''
		rospy.loginfo("Received a /cmd_vel message!")  
		rospy.loginfo("Linear Components: [%f, %f, %f]"%(msg.linear.x, msg.linear.y, msg.linear.z))  
		rospy.loginfo("Angular Components: [%f, %f, %f]"%(msg.angular.x, msg.angular.y, msg.angular.z))  
		'''
		if msg.linear.x!=VX or msg.linear.y!=VY or msg.angular.z!=VZ:
			VX=msg.linear.x
			VY=msg.linear.y
			VZ=msg.angular.z
			velocity=MotorDriver.MotorSpeedvHandler.odom_to_speed(msg.linear.x,msg.linear.y,msg.angular.z)
			MotorSetSpeed(0x01,velocity[0])
			MotorSetSpeed(0x02,velocity[1])
			MotorSetSpeed(0x03,velocity[2])
			MotorSetSpeed(0x04,velocity[3])
	except Exception:
		return
#
#motor fuction start -----------------------------------------
#motor fuction start -----------------------------------------
def MotorEnableAll():
	MotorEnable(0x01,0x01)
	MotorEnable(0x02,0x01)
	MotorEnable(0x03,0x01)
	MotorEnable(0x04,0x01)
def MotorDisableAll():	
	MotorEnable(0x01,0x00)
	MotorEnable(0x02,0x00)
	MotorEnable(0x03,0x00)
	MotorEnable(0x04,0x00)
	print('the robot driver is down!')	
def MotorEnable(ID,enable):
	running=True
	while not rospy.is_shutdown() and running:
		#获取使能电机速度的CRC校验后的字符串
		arr=MotorDriver.Motor.Enable(ID,enable)
		mSerial.send(arr)
		arr1=mSerial.read(8)
		if arr.find(arr1)>0:
			if enable==1:
				print(str(ID)+'motor enable sucessfully!')			
			else:
				print(str(ID)+'motor disable sucessfully!')				
			running=False
def MotorSetSpeed(ID,speed):
	running=True
	while not rospy.is_shutdown() and running:
		#获取设定电机速度的CRC校验后的字符串
		arr=MotorDriver.Motor.SetSpeed(ID,speed)
		#send the message
		mSerial.send(arr)
		#read the ack message
		arr1=mSerial.read(8)
		#print16(arr1)
		if arr.find(arr1)>0:
			#print(str(ID)+'motor speed set sucessfully!')
			running=False
def nodeShutdown():
	#disbale motor	
	MotorDisableAll()
#motor fuction end -----------------------------------------
#motor fuction end -----------------------------------------

#16 print 
def print16(arr):
	for i in arr:
		print('%#x'%ord(i))
#-----------------------------------------------------------
#-----------------------------------------------------------
		
if __name__ == '__main__':
	rospy.init_node('robot_driver',anonymous=True)
	rospy.Subscriber("/cmd_vel",Twist,callback)
	rospy.on_shutdown(nodeShutdown)
	#Enable motor
	MotorEnableAll()
	rospy.spin()

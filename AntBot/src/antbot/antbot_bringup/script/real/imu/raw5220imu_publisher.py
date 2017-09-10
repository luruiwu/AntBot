#!/usr/bin/env python 
# -*- coding: utf-8 -*-
#发布陀螺仪的数据
#角度格式 0xBD 0XDB 0X04 roll roll pitch pitch yaw yaw  debug0 debug0 debug1 debug1     
#				    0     1   2     3     4    5    6   7      8      9      10
# gx    gx    gy    gy    gz   gz   ax   ax    ay   ay     az     az 
# 11   12     13    14   15   16   17    18    19   20     21     22
# temp temp timestamp timestamp timestamp timestamp info info sum
# 23   24   25        26        27        28        29   30   31
import roslib; roslib.load_manifest('antbot_bringup') 
import rospy
import sys

spath=sys.path[0]
snum=spath.rfind('/')
ssyspath=spath[0:snum]
sys.path.append(ssyspath)

import tf.transformations
from geometry_msgs.msg import Twist 
from mserial import MSerialPort
from sensor_msgs.msg import Imu
import mType
import sys
from agv.msg import MImu
DBL_MAX=sys.float_info.max
ACC_X=0.0
ACC_Y=0.0
ANGULAR_Z=0.0
NUM=20
#加速度测量范围00-2g(m^2/s)
ARange=2*9.8
#角速度测量范围011-300deg/s(rad/s)
GRange=300.0/360.0*(2.0*3.14159)
#角度测量范围(rad)
#AngleRange=2*3.14159
AngleRange=360.0

serialport=rospy.get_param("~serial_port", '/dev/ttyUSB0')
baundrate=rospy.get_param("~serial_baudrate",115200)
timeout=rospy.get_param("~timeout",1)
mSerial=MSerialPort(serialport,baundrate,timeout)

	
def shutdown():
	print('the imudata_publisher is down!')
def setZero():
	global ACC_X
	global ACC_Y
	global NUM
	num1=0
	num2=0
	running=True
	acc_x=0.0
	acc_y=0.0
	while running:
		num=mSerial.read(1)
		#print('%#x'%ord(num))
		if ord(num)==0x55:
			str1=mSerial.read(10)
			#mType.print16(str1)
			if ord(str1[0])==0x51:
				if num1<NUM:
					acc_xc=float(mType.TwoBytes_ToInt(str1[2],str1[1]))/32768*16*9.8
					acc_yc=float(mType.TwoBytes_ToInt(str1[4],str1[3]))/32768*16*9.8
					acc_x+=acc_xc
					acc_y+=acc_yc
					#print(acc_x)
					#print(acc_y)
					num1+=1
			if num1==NUM:
				ACC_X=acc_x/NUM
				ACC_Y=acc_y/NUM
				running=False
def Publiser():
	global ACC_X
	global ACC_Y
	global DBL_MAX
	rospy.init_node('imudata_publisher',anonymous=True)
	rospy.on_shutdown(shutdown)
	count=0
	pub=rospy.Publisher('/rawImu_message',MImu,queue_size=20)
	real_acc_roll=0.0
	real_acc_pitch=0.0
	angle_z=0.0
	angular_z=0.0
	angle_yaw=0.0
	msg=MImu()
	while not rospy.is_shutdown():
		first=mSerial.read(1)
		if ord(first)==0xBD:
			second=mSerial.read(1)
			if ord(second)==0xDB:
				str1=mSerial.read(32)
				#mType.print16(str1)
				if ord(str1[0])==0x04:
					#angle_roll=float(mType.TwoBytes_ToInt(str1[2],str1[1]))*AngleRange/32768
					#angle_pitch=float(mType.TwoBytes_ToInt(str1[4],str1[3]))*AngleRange/32768
					angle_yaw=0.0-float(mType.TwoBytes_ToInt(str1[6],str1[5]))*AngleRange/32768.0
					#angluar_x=float(mType.TwoBytes_ToInt(str1[12],str1[11]))*GRange/32768
					#angluar_y=float(mType.TwoBytes_ToInt(str1[14],str1[13]))*GRange/32768
					angluar_z=0.0-float(mType.TwoBytes_ToInt(str1[16],str1[15]))*GRange/32768.0
					#acc_x=float(mType.TwoBytes_ToInt(str1[18],str1[17]))*ARange/32768
					#acc_y=float(mType.TwoBytes_ToInt(str1[20],str1[19]))*ARange/32768
					#acc_z=float(mType.TwoBytes_ToInt(str1[22],str1[21]))*ARange/32768
					#msg.acc_x=acc_x
					#msg.acc_y=acc_y
					msg.angular_z=angluar_z
					msg.angle_z=angle_yaw
					#print(msg.angle_z)
					pub.publish(msg)
if __name__ == '__main__':
	#setZero()
	print('being to publish imu message')
	Publiser()
				
					

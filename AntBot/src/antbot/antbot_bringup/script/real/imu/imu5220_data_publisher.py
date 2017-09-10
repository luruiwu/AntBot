#!/usr/bin/env python 
# -*- coding: utf-8 -*-
"""
imu5220数据格式 
0xBD 0XDB 0X04 roll roll pitch pitch yaw yaw  debug0 debug0 debug1 debug1     
		  0    1    2    3     4     5   6    7      8      9      10
 gx    gx    gy    gy    gz   gz   ax   ax    ay   ay     az     az 
 11    12    13    14    15   16   17   18    19   20     21     22
 temp temp timestamp timestamp timestamp timestamp info info sum
 23   24   25        26        27        28        29   30   31
 
 获取imu5220的角度数据，并发布到imu_data话题上
 
"""
import roslib; roslib.load_manifest('antbot_bringup') 
import rospy
import sys

spath=sys.path[0]
snum=spath.rfind('/')
ssyspath=spath[0:snum]
sys.path.append(ssyspath)
from geometry_msgs.msg import Twist 
from mserial import MSerialPort
from sensor_msgs.msg import Imu


class IMU_Publisher:
    
    def __init__(self):
    
        #初始化节点
        rospy.init_node("imu5220_publisher", anonymous = True)
        
        #节点结束的时候执行shutdown函数    
        rospy.on_shutdown(self.shutdown)
        
        #获取USB串口
        self.serialport = rospy.get_param("~serial_port", "/dev/ttyUSB1")
        
        #获取波特率
        self.baudrate   = rospy.get_param("~serial_baudrate", 115200)
        
        #获取等待时间
        self.timeout    = rospy.get_param("~timeout", 1)
        
        #获取串口对象
        self.mSerial    = MSerialPort(self.serialport,self.baudrate,self.timeout)
        
        #获取在/imu_data上发布数据的话题对象
        self.pub        = rospy.Publisher("/imu_data", Imu, queue_size=20)
        
        #加速度测量范围(m^2/s)
        ARange=8*9.8
        
        #角速度测量范围(rad/s)
        GRange=150.0/360.0*2.0*3.14159
        
        #角度测量范围(rad)
        AngleRange=2*3.14159
        
        #定义一个Imu消息
        msg = Imu()
        
        #航向角
        angluar_z = 0.0
        
        #航向角速度
        angle_yaw = 0.0
        
        while not rospy.is_shutdown():
        
            #读取一个数据
		    first=self.mSerial.read(1)
		    
		    #如果是数据头
		    if ord(first)==0xBD:
			    second=self.mSerial.read(1)
			    if ord(second)==0xDB:
				    str1=self.mSerial.read(32)
				    
				    #如果是陀螺仪数据
				    if ord(str1[0])==0x04:
				    
				        #提取角度（弧度）
					    angle_yaw=0.0-float(MotorDriver.mType.TwoBytes_ToInt(str1[6],str1[5]))*AngleRange/32768.0
					    #angluar_z=0.0-float(MotorDriver.mType.TwoBytes_ToInt(str1[16],str1[15]))*GRange/32768.
					    
					    #填充消息
					    msg.header.stamp = rospy.Time.now()
					    msg.header.frame_id = "imu_link"
					    
  					    #将偏航角转化为四元数，效率高
					    msg.orientation = tf.createQuaternionMsgFromYaw(0, 0, 0, angle_yaw);
					    
					    #发布消息
					    self.pub.publish(msg)
					    
    def shutdown(self):
        print "imu publisher game over"

if __name__=="__main__":
    print "being to publish imu message"
    IMU_Publisher()


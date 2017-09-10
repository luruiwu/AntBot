#!/usr/bin/env python
#coding=utf-8
"""
2017-04-23 by Bob

增加了支持各种方向移动的功能
"""
import rospy
import tf
import logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s- %(message)s')
from geometry_msgs.msg    import Twist, Quaternion, TransformStamped
from math                 import sin, cos ,pi, radians, sqrt, atan2, fabs, copysign
from sensor_msgs.msg      import JointState
from nav_msgs.msg         import Odometry
from tf.transformations   import euler_from_quaternion
from tf.transformations   import quaternion_from_euler
from yun_bringup.msg      import Encoder, Imu5220

class PublishOdom():
	def __init__(self):
		#初始化节点
		rospy.init_node("PublishOdom", anonymous = True)
		#结束节点时执行nodeshutdown函数
		rospy.on_shutdown(self.nodeShutdown)
		
		#订阅car_speed话题
		rospy.Subscriber("/encoder", Encoder, self.encoder_callback)
		#订阅imu数据
		rospy.Subscriber("/imu_data", Imu5220, self.imu_callback)
		
		#关节坐标发布者
		self.joint_pub = rospy.Publisher("/wheel_joint_states", JointState, queue_size = 20)
		#里程信息发布者
		self.odom_pub = rospy.Publisher("/odom", Odometry, queue_size = 20)
		#发布odom消息
		self.odom_broadcaster = tf.TransformBroadcaster()
		
		self.imu_msg		= Imu5220()
		self.encoder_msg	= Encoder()
		self.encoder_msg.encoder = [0]*4
		
		#机器人的位置姿态
		xPosition  = 0.0
		yPosition  = 0.0
		thPosition = 0.0
		
		#odom的变化速度
		vx = 0.0
		vy = 0.0
		vth = 0.0
		
		#相对与机器人坐标系统的位移
		s 	= 0.0
		s1 = 0.0
		s2 = 0.0
		s_x = 0.0
		s_y = 0.0
		r_x = 0.0
		r_y = 0.0
		a_x = 0.0
		a_y = 0.0
		
		L = 0.4
		W = 0.3
		R = 0.12
		
		angle_last = 0.0
		angle_curr = 0.0
		delta_th = 0
		time_last = 0.0
		time_curr = 0.0
		K = 1.0263
		encoder_diff = [0]*4
		encoder_last = [0]*4
		encoder_curr = [0]*4
		encoder_dis  = [0]*4
		temp = [0]*4
		
		#四个轮子的速度和位置
		v = [0]*4
		p = [0]*4
		FistTime = True
		self.encoder_flag = True
		self.imu_flag     = True
		
		r = rospy.Rate(50)
		"""
		call_back函数在r.sleep()时候执行
		"""
		while not rospy.is_shutdown():
			r.sleep()
			
			#保证接受到imu和encoder的初始值
			if (self.encoder_flag != False) or (self.imu_flag != False):
				continue
			
			if FistTime:
				#初始上一次的编码器数据
				for i in range(4):
					encoder_last[i] = self.encoder_msg.encoder[i]
					encoder_curr[i] = encoder_last[i]
				#初始上一次的角度
				angle_last = self.imu_msg.angle
				angle_curr = angle_last
				#初始化上一次的时间
				time_last = rospy.Time.now().to_sec()
				time_curr = time_last
			else:
				for i in range(4):
					encoder_curr[i] = self.encoder_msg.encoder[i]
				angle_curr = self.imu_msg.angle
				time_curr = rospy.Time.now().to_sec()
				
			#计算每个轮子转动的距离
			for i in range(4):
				#统计单位时间的脉冲数
				encoder_diff[i] = encoder_curr[i] - encoder_last[i]
				
				if encoder_diff[i] < -10000:
					encoder_diff[i] = encoder_diff[i] + 60000
				elif encoder_diff[i] > 10000:
					encoder_diff[i] = encoder_diff[i] - 60000
				
				#计算单位时间的距离
				encoder_dis[i] = encoder_diff[i] * 7.48 * 1e-5 * K
				#temp[i] += encoder_dis[i]
				#print "temp[%d]:%f"%(i, temp[i])
			
			#时间差
			delta_time = time_curr - time_last
			#航向角差
			delta_th   = angle_curr - angle_last
			
			"""
			航迹推演,角度逆时针为正
			"""
			#/*********************************************************************************/
			"""
			s = (encoder_dis[1] + encoder_dis[3])/2.0
			
			#如果是沿着Y轴运动，轮胎的运转方向相反
			if (encoder_dis[0]) * (encoder_dis[3]) < 0:
				r_x = 0.0
				r_y = s
				#print "x"
			#如果是沿着X轴运动
			else:
				#在机器人坐标系统上的投影
				r_x = s * cos(delta_th / 2.0)
				r_y = s * sin(delta_th / 2.0)

			
			#投射到世界坐标系的坐标变化
			delta_x = r_x * cos(thPosition) + r_y * sin(thPosition)
			delta_y = -1.0 * r_x * sin(thPosition) + r_y * cos(thPosition)
			#delta_y = r_x * cos(thPosition + pi/2) + r_y * cos(thPosition)
			"""
			#//////////////////////////////////////////////////////////////////////////////
			#<1>
			delta_s13 = (encoder_dis[0] + encoder_dis[2])/(2.0 * cos(pi/4)) #1，3轮子
			delta_s24 = (encoder_dis[1] + encoder_dis[3])/(2.0 * cos(pi/4)) #2，4轮子
			
			delta_x = (delta_s24 * cos(thPosition + pi/4 + delta_th/2) + delta_s13 * cos(thPosition + pi*3/4 + delta_th/2))/2.0
			delta_y = (delta_s24 * sin(thPosition + pi/4 + delta_th/2) + delta_s13 * sin(thPosition + pi*3/4 + delta_th/2))2.0
			
			#////////////////////////////////////////////////////////////////////////////
			"""
			#<2>
			#两组差速轮子沿着各自中心线移动的距离
			delta_s13 = (encoder_dis[0] + encoder_dis[2])/(2.0 * cos(pi/4)) #1，3轮子
			delta_s24 = (encoder_dis[1] + encoder_dis[3])/(2.0 * cos(pi/4)) #2，4轮子
			
			#两组轮子的位移在x，y轴上的投影
			delta_s13_x = delta_s13 * cos(delta_th/2)
			delta_s13_y = delta_s13 * sin(delta_th/2)
			
			delta_s24_x = delta_s24 * cos(delta_th/2)
			delta_s24_y = delta_s24 * sin(delta_th/2)
			
			#在世界坐标系上的投影
			delta_x = delta_s13_x * cos(thPosition) + delta_s13_y * cos(thPosition + pi/2) + \
						delta_s24_x * cos(thPosition + pi*3/4) + delta_s24_y * cos(thPosition + pi*3/4 + pi/2)
			delta_y = delta_s13_x * sin(thPosition + pi/4) + delta_s13_y * sin(thPosition + pi/4 + pi/2) + \
						delta_s24_x * sin(thPosition + pi*3/4) + delta_s24_y * sin(thPosition + pi*3/4 + pi/2)
			
			"""
			#////////////////////////////////////////////////////////////////////////////
			#/*********************************************************************************/
			#速度(第一次的delta_time为0)
			if  not FistTime:
				vx  = r_x / delta_time
				vy  = r_y / delta_time
				vth = delta_th / delta_time
			
			#累计的位置
			xPosition  += delta_x
			yPosition  += delta_y
			thPosition += delta_th
			
			print("xPosition: %f, yPosition: %f, thPosition: %f"%(xPosition, -yPosition, thPosition*180/pi))
			
			#迭代
			for i in range(4):
				encoder_last[i] = encoder_curr[i]
			angle_last = angle_curr
			time_last = time_curr
			if FistTime:
				FistTime = False
				
			##########################################################################################################
			#每个轮子的位置和速度
			v[0] = vx + vy + (L + W) * vth
			v[1] = vx - vy - (L + W) * vth
			v[2] = vx + vy - (L + W) * vth
			v[3] = vx - vy + (L + W) * vth
			
			for i in range(4):
				p[i] += (v[i] / R) * delta_time
			
			#发布关节的坐标关系
			wheel_joint_state = JointState()
			wheel_joint_state.header.stamp = rospy.Time.now()
			wheel_joint_state.name.append("front_right_wheel_to_base_link")
			wheel_joint_state.position.append(p[0])
			wheel_joint_state.name.append("front_left_wheel_to_base_link")
			wheel_joint_state.position.append(p[1])
			wheel_joint_state.name.append("rear_left_wheel_to_base_link")
			wheel_joint_state.position.append(p[2])
			wheel_joint_state.name.append("rear_right_wheel_to_base_link")
			wheel_joint_state.position.append(p[3])
			self.joint_pub.publish(wheel_joint_state)
			
			#发布TF关系
			odom_quat = Quaternion()
			odom_quat = quaternion_from_euler(0, 0, thPosition)
			self.odom_broadcaster.sendTransform((xPosition, -yPosition, 0),
																odom_quat,
															rospy.Time.now(),
																"base_link",
																"odom")
			#发布航迹推演消息
			#定义额一个Odometry消息
			odom = Odometry()
			odom.header.stamp = rospy.Time.now()
			odom.header.frame_id = "odom"

			#设置位姿
			odom.pose.pose.position.x = xPosition
			odom.pose.pose.position.y = -yPosition
			odom.pose.pose.position.z = 0.0
			odom.pose.pose.orientation.x = odom_quat[0]
			odom.pose.pose.orientation.y = odom_quat[1]
			odom.pose.pose.orientation.z = odom_quat[2]
			odom.pose.pose.orientation.w = odom_quat[3]

			#设置速度
			odom.child_frame_id = "base_link"
			odom.twist.twist.linear.x = vx
			odom.twist.twist.linear.y = vy
			odom.twist.twist.angular.z = vth#有可能是这个符号错了

			#协方差
			odom.pose.covariance[0]  = 0.1
			odom.pose.covariance[7]  = 0.1
			odom.pose.covariance[35] = 0.2
			odom.pose.covariance[14] = 10000000 #set a non-zero covariance on unused
			odom.pose.covariance[21] = 10000000 #dimensions (z, pitch and roll); this
			odom.pose.covariance[28] = 10000000 #is a requirement of robot_pose_ekf
			self.odom_pub.publish(odom)

			##########################################################################################
			
	def encoder_callback(self, msg):
		if self.encoder_flag:
			self.encoder_flag = False
		#获取编码器数据
		self.encoder_msg = msg
		#print "encoder_callback"
		
	def imu_callback(self, msg):
		if self.imu_flag:
			self.imu_flag = False
		#读取imu数据
		self.imu_msg = msg
		#print "imu_callback"
		
	def nodeShutdown(self):
		print "PublishOdom node is shutdown"
		
if __name__=="__main__":
	try:
		print "being PublishOdom..."
		PublishOdom()
	except:
		rospy.loginfo("PublishOdom node terminated !")
		

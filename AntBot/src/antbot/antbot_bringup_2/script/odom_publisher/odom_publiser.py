#!/usr/bin/env python
#coding=utf-8
"""
2017-01-11 by Bob

"""
import rospy
import tf
from geometry_msgs.msg    import Twist, Quaternion, TransformStamped
from math                 import sin, cos ,pi, radians
from sensor_msgs.msg      import JointState, Imu5220
from nav_msgs.msg         import Odometry
from tf.transformations   import euler_from_quaternion
from tf.transformations   import quaternion_from_euler
from antbot_bringup_2.msg import Encoder

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
		#订阅速度话题
		rospy.Subscriber("/cmd_vel", Twist, self.cmd_callback)
		
		#关节坐标发布者
		self.joint_pub = rospy.Publisher("/joint_state", JointState, queue_size = 20)
		#里程信息发布者
		self.odom_pub = rospy.Publisher("/odom", Odometry, queue_size = 20)
		#发布odom消息
		self.odom_broadcaster = tf.TransformBroadcaster()
		
		self.cmd_msg		= Twist()
		self.imu_msg		= Imu5220()
		self.encoder_msg	= Encoder()
		self.encoder_msg.encoder = [0]*4
		
		xPosition  = 0.0
		yPosition  = 0.0
		thPosition = 0.0
		
		vx = 0.0
		vy = 0.0
		vth = 0.0
		
		angle_last = 0.0
		angle_curr = 0.0
		delta_th = 0
		time_last = 0.0
		time_curr = 0.0
		k = 1.0263
		encoder_diff = [0]*4
		encoder_last = [0]*4
		encoder_curr = [0]*4
		encoder_dis  = [0]*4
		FistTime = True
		
		r = rospy.Rate(50)
		while not rospy.is_shutdown():
			r.sleep()
			
			#如果是第一次进入回调函数
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
				#统计脉冲数
				encoder_diff[i] = encoder_curr[i] - encoder_last[i]
				if encoder_diff[i] < -10000:
					encoder_diff[i] += 60000
				elif encoder_diff[i] > 10000:
					encoder_diff[i] -= 60000
				#计算距离
				encoder_dis[i] += encoder_diff[i] * 7.48 * 1e-5 * k
				
			#时间差
			delta_time = time_curr - time_last
			
			#航向角差
			delta_th   = angle_curr - angle_last
			
			"""
			航迹推演
			"""
			#只有Y轴的速度
			if self.cmd_msg.linear.y != 0:
				#机器人人坐标系的坐标变化
				s_y = cos(radians(45)) * encoder_dis[1]
				s_x = 0
				
				#投射到世界坐标系的坐标变化
				delta_x = s_x * cos(thPosition) - s_y * sin(thPosition)
				delta_y = s_x * sin(thPosition) + s_y * cos(thPosition)
			#没有Y轴的速度
			else:
				#机器人人坐标系的坐标变化
				s = cos(radians(45)) * sum(encoder_dis)/4
				s_x = cos(delta_th / 2.0) * s
				s_y = sin(delta_th / 2.0) * s
				
				#投射到世界坐标系的坐标变化
				delta_x = s_x * cos(thPosition) - s_y * sin(thPosition)
				delta_y = s_x * sin(thPosition) + s_y * cos(thPosition)
			
			#速度(第一次的delta_time为0)
			if  not FistTime:
				vx  = delta_x / delta_time
				vy  = delta_y / delta_time
				vth = delta_th / delta_time
			
			#累计的位置
			xPosition  += delta_x
			yPosition  += delta_y
			thPosition += delta_th
			
			#迭代
			for i in range(4):
				encoder_last[i] = encoder_curr[i]
			angle_last = angle_curr
			time_last = time_curr
			if FistTime:
				FistTime = False
			
			##########################################################################################################
			#发布关节的坐标关系
			joint_state = JointState()
			joint_state.header.stamp = rospy.Time.now()
			joint_state.name.append("base_to_wheel1")
			joint_state.position.append(0)
			joint_state.name.append("base_to_wheel2")
			joint_state.position.append(1)
			joint_state.name.append("base_to_wheel3")
			joint_state.position.append(2)
			joint_state.name.append("base_to_wheel4")
			joint_state.position.append(3)
			self.joint_pub.publish(joint_state)
			
			#发布TF关系
			odom_quat = Quaternion()
			odom_quat = quaternion_from_euler(0, 0, thPosition)
			self.odom_broadcaster.sendTransform((xPosition, yPosition, 0),
																odom_quat,
															rospy.Time.now(),
																	"odom",
																"base_link")
			#发布航迹推演消息
			#定义额一个Odometry消息
			odom = Odometry()
			odom.header.stamp = rospy.Time.now()
			odom.header.frame_id = "odom"

			#设置位姿
			odom.pose.pose.position.x = xPosition
			odom.pose.pose.position.y = yPosition
			odom.pose.pose.position.z = 0.0
			odom.pose.pose.orientation.x = odom_quat[0]
			odom.pose.pose.orientation.y = odom_quat[1]
			odom.pose.pose.orientation.z = odom_quat[2]
			odom.pose.pose.orientation.w = odom_quat[3]

			#设置速度
			odom.child_frame_id = "base_link"
			odom.twist.twist.linear.x = vx
			odom.twist.twist.linear.y = vy
			odom.twist.twist.angular.z = vth

			#协方差
			odom.pose.covariance[0]  = 0.1
			odom.pose.covariance[7]  = 0.1
			odom.pose.covariance[35] = 0.2
			odom.pose.covariance[14] = 10000000 #set a non-zero covariance on unused
			odom.pose.covariance[21] = 10000000 #dimensions (z, pitch and roll); this
			odom.pose.covariance[28] = 10000000 #is a requirement of robot_pose_ekf
			self.odom_pub.publish(odom)
			
			#####################################################################################################
			
	def encoder_callback(self, msg):
		#获取编码器数据
		self.encoder_msg = msg
		
	def cmd_callback(self, msg):
		#读取cmd_vel速度
		self.cmd_msg = msg
		
	def imu_callback(self, msg):
		#读取imu数据
		self.imu_msg = msg
		
	def nodeShutdown(self):
		print "PublishOdom node is shutdown"
		
if __name__=="__main__":
	try:
		print "being PublishOdom..."
		PublishOdom()
	except:
		rospy.loginfo("PublishOdom node terminated !")
		

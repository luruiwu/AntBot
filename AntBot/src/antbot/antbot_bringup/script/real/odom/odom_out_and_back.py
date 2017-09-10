#!/usr/bin/env python
#coding=utf-8
""" odom_out_and_back.py - Version 1.1 
    在/odom话题上发布速度信息，让机器人走指定的距离或者转给定的角度
"""

import rospy
from geometry_msgs.msg import Twist, Point, Quaternion
import tf
from transform_utils import quat_to_angle, normalize_angle
from math import radians, copysign, sqrt, pow, pi

class OutAndBack():
    def __init__(self):
        # 初始化node
        rospy.init_node('out_and_back', anonymous=False)

        # 当结束node时执行shutdown函数
        rospy.on_shutdown(self.shutdown)

        # 定义一个速度发布者
        self.cmd_vel = rospy.Publisher('/cmd_vel', Twist, queue_size=5)
        
        # 更新机器人运动频率
        rate = 10
        
        # Set the equivalent ROS rate variable
        r = rospy.Rate(rate)
        
        # 设置线性的速度
        linear_speed = 0.5
        
        # 设置测试距离
        goal_distance = 2.0

        # 设置旋转的速度
        angular_speed = 0.5
        
        # 设置旋转角度的允许误差
        angular_tolerance = radians(0.01)
        
        # 设置旋转的角度
        goal_angle = pi

        # 初始化
        self.tf_listener = tf.TransformListener()
        
        # 给一些时间用来填充buffer
        rospy.sleep(2)
        
        # 设置/odom框架
        self.odom_frame = '/odom'
        
        # 判断机器人是使用/base_link
        try:
            self.tf_listener.waitForTransform(self.odom_frame, '/base_link', rospy.Time(), rospy.Duration(1.0))
            self.base_frame = '/base_link'
        except (tf.Exception, tf.ConnectivityException, tf.LookupException):
            rospy.loginfo("Cannot find transform between /odom and /base_link")
            rospy.signal_shutdown("tf Exception")  
        
        # 初始化位置为点类型
        position = Point()
            
        # 循环
        for i in range(2):
            # 初始化速度命令
            move_cmd = Twist()
            
            # 设置速度
            move_cmd.linear.x = linear_speed
            
            # 获取初始化位姿态
            (position, rotation) = self.get_odom()
            
            x_start = position.x
            y_start = position.y
            
            # 跟踪
            distance = 0
            
            # 进入距离控制循环
            while distance < goal_distance and not rospy.is_shutdown():
                # 发布速度命令
                self.cmd_vel.publish(move_cmd)
                
                #控制发布速度频率
                r.sleep()
        
                # 获取当前位姿
                (position, rotation) = self.get_odom()
                
                # 计算相对于起点的欧几里德距离
                distance = sqrt(pow((position.x - x_start), 2) + 
                                pow((position.y - y_start), 2))
            
            # 在旋转之前停止机器人
            move_cmd = Twist()
            self.cmd_vel.publish(move_cmd)
            rospy.sleep(1)
            
            # 设置旋转角速度
            move_cmd.angular.z = angular_speed
            
            last_angle = rotation
            
            turn_angle = 0
            
            while abs(turn_angle + angular_tolerance) < abs(goal_angle) and not rospy.is_shutdown():
                # 发布速度命令
                self.cmd_vel.publish(move_cmd)
                r.sleep()
                
                # 获取当前旋转角度
                (position, rotation) = self.get_odom()
                
                # 计算相对于上一次旋转的角度
                delta_angle = normalize_angle(rotation - last_angle)
                
                # 添加到旋转角度中
                turn_angle += delta_angle
                last_angle = rotation
                
            # 下一次循环之前停止机器人
            move_cmd = Twist()
            self.cmd_vel.publish(move_cmd)
            rospy.sleep(1)
            
        # 停止机器人
        self.cmd_vel.publish(Twist())
        
    def get_odom(self):
        # 获取odom框架相对于基础框架之间的转换关系
        try:
            (trans, rot)  = self.tf_listener.lookupTransform(self.odom_frame, self.base_frame, rospy.Time(0))
        except (tf.Exception, tf.ConnectivityException, tf.LookupException):
            rospy.loginfo("TF Exception")
            return

        return (Point(*trans), quat_to_angle(Quaternion(*rot)))
        
    def shutdown(self):
        # 节点停止前停止机器人
        rospy.loginfo("Stopping the robot...")
        self.cmd_vel.publish(Twist())
        rospy.sleep(1)
 
if __name__ == '__main__':
    try:
        OutAndBack()
    except:
        rospy.loginfo("Out-and-Back node terminated.")

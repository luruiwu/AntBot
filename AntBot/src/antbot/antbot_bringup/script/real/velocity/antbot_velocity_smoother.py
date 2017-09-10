#!/usr/bin/env python
#coding=utf-8


"""
接收速度命令，对速度进行平滑处理，使其加速度不至于过大

"""
import rospy
from geometry_msgs.msg import Twist

class VelocitySmoother:

    def __init__(self):
        rospy.init_node("antbot_velocity_smoother")
        
        rospy.on_shutdown(self.nodeshutdown)
        
        #订阅原始的速度数据
        rospy.Subscriber("/raw_cmd_vel", Twist, self.callback)
        
        #发布平滑的速度发布者
        self.cmd_pub = rospy.Publisher("/cmd_vel", Twist, queue_size = 1)
        
        #发布频率
        rate = rospy.Rate(10)
        
        #目标速度
        self.cmd_target  = None
        
        #控制速度
        self.cmd_control = Twist()
        
        #速度增加
        delta_x = 0.01
        delta_y = 0.01
        delta_z = 0.05
            
        while not rospy.is_shutdown():
        
            rate.sleep()
            if self.cmd_target:
                # 速度缓冲
                #X
                if self.cmd_target.linear.x > self.cmd_control.linear.x:
                    self.cmd_control.linear.x = min(self.cmd_target.linear.x, self.cmd_control.linear.x + delta_x)
                elif self.cmd_target.linear.x < self.cmd_control.linear.x:
                    self.cmd_control.linear.x = max(self.cmd_target.linear.x, self.cmd_control.linear.x - delta_x)
                else:
                    self.cmd_control.linear.x = self.cmd_target.linear.x
                #Y
                if self.cmd_target.linear.y > self.cmd_control.linear.y:
                    self.cmd_control.linear.y = min(self.cmd_target.linear.y, self.cmd_control.linear.y + delta_y)
                elif self.cmd_target.linear.y < self.cmd_control.linear.y:
                    self.cmd_control.linear.y = max(self.cmd_target.linear.y, self.cmd_control.linear.y - delta_y)
                else:
                    self.cmd_control.linear.y = self.cmd_target.linear.y
                #Z
                if self.cmd_target.angular.z > self.cmd_control.angular.z:
                    self.cmd_control.angular.z = min(self.cmd_target.angular.z, self.cmd_control.angular.z + delta_z)
                elif self.cmd_target.angular.z < self.cmd_control.angular.z:
                    self.cmd_control.angular.z = max(self.cmd_target.angular.z, self.cmd_control.angular.z - delta_z)
                else:
                    self.cmd_control.angular.z = self.cmd_target.angular.z

                self.cmd_pub.publish(self.cmd_control)
                
                
    def callback(self, msg):
        #原始的获取速度数据
        self.cmd_target = msg
    
    def nodeshutdown(self):
        self.cmd_pub.publish(Twist())

if __name__=="__main__":
    try:
        VelocitySmoother()
    except:
        rospy.loginfo("antbot velocity smoother")
        
        

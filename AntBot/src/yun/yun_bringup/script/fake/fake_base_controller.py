#!/usr/bin/env python
#coding=utf-8
#
#这是一个模拟的base_controller节点
#1、订阅/cmd_vel上的速度，并转换为轮子的速度通过串口发送到arduino
#2、监听串口，接收arduino的发送的轮子的速度信息，并转化为odom信息，在yun_speed这个话题上发布出去
#由于这个是模拟的，没有底层硬件，所以这里的第二部分的轮子的速度信息是直接从/cmd_vel上得到的，
#实际操作中这两个速度是Arduino通过串口反馈上来的。
#
import roslib; roslib.load_manifest('yun_bringup')
import rospy
import tf.transformations
from geometry_msgs.msg import Twist
from yun_bringup.msg import Odom

global vx#x方向上的速度
global vy#y方向上的速度
global vth#围绕z轴的旋转速度
vx  = 0.0
vy  = 0.0
vth = 0.0

def callback(msg):
        global vx#x方向上的速度
        global vy#y方向上的速度
        global vth#围绕z轴的旋转速度
        rospy.loginfo("Received a /cmd_vel message!")
        rospy.loginfo("Linear Components: [%f, %f, %f]"%(msg.linear.x, msg.linear.y, msg.linear.z))
        rospy.loginfo("Angular Components: [%f, %f, %f]\n"%(msg.angular.x, msg.angular.y, msg.angular.z))

        #新收到的速度与之前的速度不一致，就重新设置速度
        if vx!=msg.linear.x or vy!=msg.linear.y or vth!=msg.angular.z:
                vx=msg.linear.x
                vy=msg.linear.y
                vth=msg.angular.z
        #将接收到的速度指令，映射为四个轮子的速度，并通过蓝牙发送到arduino

        #将接收到的速度指令，映射为四个轮子的速度，并通过蓝牙发送到arduino

#listen speed message on cmd_vel topic
def listener():
        global vx#x方向上的速度
        global vy#y方向上的速度
        global vth#围绕z轴的旋转速度
        rospy.Subscriber("/cmd_vel", Twist, callback)
        pub = rospy.Publisher("yun_speed",Odom, queue_size=20)

        rate = rospy.Rate(20)
        yunSpeed = Odom()
        #监听串口，并接收从arduino发送过来的速度信息，转换为odom信息，并发布出去
        #直接将得/cmd_vel上的信息发布出去
        while not rospy.is_shutdown():
                yunSpeed.vx  = vx
                yunSpeed.vy  = vy
                yunSpeed.vth = vth

                pub.publish(yunSpeed)
                rate.sleep()

def nodeShutdown():
        rospy.loginfo("The fake move_base is down!")

if __name__=='__main__':
        rospy.init_node('base_controller', anonymous=True)
        rospy.on_shutdown(nodeShutdown)
        listener()



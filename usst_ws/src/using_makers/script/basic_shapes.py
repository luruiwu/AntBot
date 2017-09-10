#!/usr/bin/env python
#coding=utf-8

"""
这个例子是简单的在Rviz中显示一个Marker
"""


import rospy

#导入显示消息类型，这里主要是Marker消息类型
from visualization_msgs.msg import *

#main
if __name__=="__main__":

	#初始化节点，anonymous为True，避免重复名字
	rospy.init_node("cube", anonymous=True)
	
	#发布频率
	rate = rospy.Rate(20)
	
	#定义发布者，发布话题：/cube，消息类型是：Marker,消息队列长度：10
	marker_pub = rospy.Publisher("/cube", Marker, queue_size=10)
	
	rospy.loginfo("Initializing...")

	#定义一个marker对象，并初始化各种Marker的特性
	marker = Marker()
	
	#指定Marker的参考框架
	marker.header.frame_id = "/map"
	
	#时间戳
	marker.header.stamp = rospy.Time.now()
	
	#ns代表namespace，命名空间可以避免重复名字引起的错误
	marker.ns = "basic_shapes"
	
	#Marker的id号
	marker.id = 0
	
	#Marker的类型，有ARROW，CUBE等
	marker.type = Marker.CYLINDER
	
	#Marker的尺寸，单位是m
	marker.scale.x = 0.2
	marker.scale.y = 0.2
	marker.scale.z = 0.4
	
	#Marker的动作类型有ADD，DELETE等
	marker.action = Marker.ADD
	
	#Marker的位置姿态
	marker.pose.position.x = 0.0
	marker.pose.position.y = 0.0
	marker.pose.position.z = 0.2
	marker.pose.orientation.x = 0.0
	marker.pose.orientation.y = 0.0
	marker.pose.orientation.z = 0.0
	marker.pose.orientation.w = 1.0
	
	#Marker的颜色和透明度
	marker.color.r = 0.0
	marker.color.g = 0.8
	marker.color.b = 0.0
	marker.color.a = 0.5
	
	#Marker被自动销毁之前的存活时间，rospy.Duration()意味着在程序结束之前一直存在
	marker.lifetime = rospy.Duration()
	
	#循环发布
	while not rospy.is_shutdown():
		
		#发布Marker
		marker_pub.publish(marker)
		
		#控制发布频率
		rate.sleep()
	
	

"""
下面是Marker.msg的消息类型的参数说明：

# See http://www.ros.org/wiki/rviz/DisplayTypes/Marker and http://www.ros.org/wiki/rviz/Tutorials/Markers%3A%20Basic%20Shapes for more information on using this message with rviz

uint8 ARROW=0
uint8 CUBE=1
uint8 SPHERE=2
uint8 CYLINDER=3
uint8 LINE_STRIP=4
uint8 LINE_LIST=5
uint8 CUBE_LIST=6
uint8 SPHERE_LIST=7
uint8 POINTS=8
uint8 TEXT_VIEW_FACING=9
uint8 MESH_RESOURCE=10
uint8 TRIANGLE_LIST=11

uint8 ADD=0
uint8 MODIFY=0
uint8 DELETE=2
#uint8 DELETEALL=3 # TODO: enable for ROS-J, disabled for now but functionality is still there. Allows one to clear all markers in plugin

Header header                        # header for time/frame information
string ns                            # Namespace to place this object in... used in conjunction with id to create a unique name for the object
int32 id                             # object ID useful in conjunction with the namespace for manipulating and deleting the object later
int32 type                           # Type of object
int32 action                         # 0 add/modify an object, 1 (deprecated), 2 deletes an object, 3 deletes all objects
geometry_msgs/Pose pose              # Pose of the object
geometry_msgs/Vector3 scale          # Scale of the object 1,1,1 means default (usually 1 meter square)
std_msgs/ColorRGBA color             # Color [0.0-1.0]
duration lifetime                    # How long the object should last before being automatically deleted.  0 means forever
bool frame_locked                    # If this marker should be frame-locked, i.e. retransformed into its frame every timestep

#Only used if the type specified has some use for them (eg. POINTS, LINE_STRIP, ...)
geometry_msgs/Point[] points
#Only used if the type specified has some use for them (eg. POINTS, LINE_STRIP, ...)
#number of colors must either be 0 or equal to the number of points
#NOTE: alpha is not yet used
std_msgs/ColorRGBA[] colors

# NOTE: only used for text markers
string text

# NOTE: only used for MESH_RESOURCE markers
string mesh_resource
bool mesh_use_embedded_materials


参考：
[1]:http://wiki.ros.org/rviz/DisplayTypes/Marker#Sphere_List_.28SPHERE_LIST.3D7.29
[2]:http://wiki.ros.org/rviz/Tutorials

"""


#!/usr/bin/env python
#coding=utf-8


import rospy
import copy

#导入交互marker服务端
from interactive_markers.interactive_marker_server import *

#导入视觉消息
from visualization_msgs.msg import *

#交互回调函数
def processFeedback(feedback):

	#打印当前的位置
	p = feedback.pose.position
	r = feedback.pose.orientation
	print feedback.marker_name + " is now at " + str(p.x) + ", " + str(p.y) 
	print "(" + str(r.x) + " " + str(r.y) + " " + str(r.z) + " " + str(r.w) + ")" + "\n"

if __name__=="__main__":

	#初始化节点
	rospy.init_node("InteractiveMarker", anonymous=True)
	
	#创建交互marker服务端对象
	server = InteractiveMarkerServer("simple_interactive_marker")
	
	#创建交互marker对象
	int_marker = InteractiveMarker()
	
	#交互Marker的参考框架
	int_marker.header.frame_id = "map"
	int_marker.name = "my_marker"
	int_marker.description = "Simple Interactive Marker"
	
	#创建Marker
	waypoint_marker = Marker()
	waypoint_marker.type = Marker.CYLINDER
	waypoint_marker.scale.x = 0.2
	waypoint_marker.scale.y = 0.2
	waypoint_marker.scale.z = 0.4
	waypoint_marker.color.r = 0.0
	waypoint_marker.color.g = 0.5
	waypoint_marker.color.b = 0.5
	waypoint_marker.color.a = 1.0
	waypoint_marker.pose.position.z = 0.2
	
	#创建非交互的控制，并添加waypoint_marker到控制，不用来交互，
	#只是用来在移动的时候可以看到waypoint_marker
	waypoint_control = InteractiveMarkerControl()
	
	#始终可见
	waypoint_control.always_visible = False
	
	#给交互控制添加具体的Marker
	waypoint_control.markers.append(waypoint_marker)
	
	#添加非交互控制
	int_marker.controls.append(waypoint_control)
	
	#创建交互控制，不包含marker
	move_control = InteractiveMarkerControl()
	move_control.name = "move_x"
	
	#设置交互控制的方向
	move_control.orientation.w = 1.0
	move_control.orientation.x = 0.0
	move_control.orientation.y = 1.0
	move_control.orientation.z = 0.0
	
	#设置交互控制模式
	move_control.interaction_mode = InteractiveMarkerControl.MOVE_PLANE
	int_marker.controls.append(copy.deepcopy(move_control))
	move_control.interaction_mode = InteractiveMarkerControl.ROTATE_AXIS
	int_marker.controls.append(move_control)
	
	
	#添加非交互控制
	int_marker.controls.append(move_control)
	
	#添加Marker和它的回调函数
	server.insert(int_marker, processFeedback)
	
	#应用变化，并发送给客户端
	server.applyChanges()
	
	rospy.spin()
	
	
"""
Marker，InteractiveMarker，InteractiveMarkerControl和InteractiveMarkerServer之间的关系

Maker：显示的Marker类型，定义Maker的位置姿态和颜色等
InteractiveMarker：交互的Marker类型，定义可以控制的Marker，可以指定具体的Marker。
InteractiveMarkerControl:交互Marker的控制类型，定义平移、旋转等控制等
InteractiveMarkerServer：交互Marker的服务器类型,用于管理InteractiveMarker的回调函数，添加和删除等

交互控制和非交互控制的区别是：
	交互控制是用于具体的交互，如果没有添加具体的Marker，那么会显示辅助交互的箭头或者圆弧；
	而非交互控制不是用于具体的交互，可能是用于在交互过程中显示Maker。


"""





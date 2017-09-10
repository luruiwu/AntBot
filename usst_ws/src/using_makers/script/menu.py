#!/usr/bin/env python
#coding=utf-8
"""
关于菜单的简单例子
"""

import rospy

#导入视觉显示消息
from visualization_msgs.msg import *

#导入交互Marker服务
from interactive_markers.interactive_marker_server import *

#导入菜单库函数
from interactive_markers.menu_handler import *

#菜单回调函数
def CreateCb(feedback):
	rospy.loginfo("CreateCb")

def SaveCb(feedback):
	rospy.loginfo("SaveCb")
	
def SavetoCb(feedback):
	rospy.loginfo("SavetoCb")
	
def LoadCb(feedback):
	rospy.loginfo("LoadCb")

def processFeedback(feedback):
	p = feedback.pose.position
	print feedback.marker_name + " is now at " + str(p.x) + ", " + str(p.y) + ", " + str(p.z)


if __name__=="__main__":
	
	rospy.init_node("menu_handler", anonymous=True)
	
	#InteractiveMarker服务器
	server = InteractiveMarkerServer("menu")
	
	#定义Marker对象box
	box = Marker()
	box.type = Marker.CYLINDER
	box.scale.x = 0.2
	box.scale.y = 0.2
	box.scale.z = 0.4
	box.color.r = 0.5
	box.color.g = 0.5
	box.color.b = 0.5
	box.color.a = 1.0
	box.pose.position.z = 0.2
	
	#定义marker control 
	control = InteractiveMarkerControl()
	
	#BUTTON模式
	control.interaction_mode = InteractiveMarkerControl.BUTTON
	control.always_visible = True
	
	#定义interactive marker
	int_marker = InteractiveMarker()
	int_marker.header.frame_id = "base_link"
	int_marker.name = "my_marker"
	int_marker.scale = 1
	
	#marker --> control
	control.markers.append(box)
	
	#control --> int_marker
	int_marker.controls.append(control)
	
	#int_marker --> server
	server.insert(int_marker, processFeedback)
	
	#创建一个菜单
	menu_handler = MenuHandler()
	
	#插入菜单
	menu_handler.insert("Create a waypoint", callback=CreateCb)
	
	entry = menu_handler.insert("Save waypoints", callback=SaveCb)
	
	#二级菜单
	menu_handler.insert("Save waypoints to", parent=entry, callback=SavetoCb)
	
	menu_handler.insert("Load waypoints", callback=LoadCb)
	
	#应用菜单到my_marker
	menu_handler.apply(server, "my_marker")
	
	#应用变化	
	server.applyChanges()
	
	rospy.spin()
	
	
	
	
	
	
	

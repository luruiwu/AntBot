#!/usr/bin/env python
#coding=utf-8

"""
2017-5-4 by Bob

"""
import rospy
from visualization_msgs.msg import *
from interactive_markers.interactive_marker_server import *
from geometry_msgs.msg import Point

#这个类用于管理创建基于InteractiveMarker的waypoint
class LinePath(InteractiveMarker):
	
	#构造函数，初始化参考框架，名字，描述，是否是管理waypoint
	def __init__(self, frame_id, name, description, is_manager=False, arrow_id=0, start_point=Marker()):
	
		rospy.loginfo("Initializing...")
		
		#调用InteractiveMarker的初始化函数
		InteractiveMarker.__init__(self)
		
		self.header.frame_id = frame_id
		self.name = name
		self.description = description
		self.start_point = Point()
		self.end_point = Point()
		
		self.pub = rospy.Publisher("/lines", Marker, queue_size=1)
		
		#箭头
		self.arrow = Marker()
		self.arrow.header.frame_id = self.header.frame_id
		self.arrow.header.stamp = rospy.Time.now()
		self.arrow.ns = "arrow"
		self.arrow.id = arrow_id
		self.arrow.action = Marker.ADD
		self.arrow.type = Marker.ARROW
		self.arrow.scale.x = 0.04 #shaft diameter
		self.arrow.scale.y = 0.12 #head diameter
		self.arrow.scale.z = 0.3  #head length
		self.arrow.color.r = 1.0
		self.arrow.color.a = 0.5
		self.arrow.points = [0 for i in range(2)]
		self.arrow.points[0] = self.start_point
		self.arrow.points[1] = self.end_point
		self.start_point.x = start_point.pose.position.x
		self.start_point.y = start_point.pose.position.y
		
		#定义一个Marker
		self.marker = Marker()
		self.marker.type = Marker.CYLINDER
		self.marker.scale.x = 0.2
		self.marker.scale.y = 0.2
		self.marker.scale.z = 0.4
		self.marker.pose.position.z = 0.2
		#self.marker.pose.position.x = self.end_point.pose.position.x
		#self.marker.pose.position.y = self.end_point.pose.position.y
		
		#管理waypoint的颜色不一样
		if is_manager:
			self.marker.color.r = 0.8
			self.marker.color.g = 0.0
			self.marker.color.b = 0.0
			self.marker.color.a = 0.5
		else:
			self.marker.color.r = 0.0
			self.marker.color.g = 0.8
			self.marker.color.b = 0.0
			self.marker.color.a = 0.5
			
		#创建一个InteractiveMarkerControl
		self.marker_control = InteractiveMarkerControl()
		self.marker_control.always_visible = True
		self.marker_control.orientation.w = 1.0
		self.marker_control.orientation.x = 0.0
		self.marker_control.orientation.y = 1.0
		self.marker_control.orientation.z = 0.0
		
		#给control指定具体的marker
		self.marker_control.markers.append(self.marker)
		self.marker_control.interaction_mode = InteractiveMarkerControl.MOVE_PLANE
		
		#给InteractiveMarker添加控制
		self.controls.append(self.marker_control)
		
	#每接收到一个interaction就调用一次回调函数
	def processFeedback(self, feedback):
		#回调函数得的当前的位置姿态
		self.pose = feedback.pose
		
		#填充arrow
		self.end_point.x = self.pose.position.x
		self.end_point.y = self.pose.position.y
		self.arrow.points[0] = self.start_point 
		self.arrow.points[1] = self.end_point
		
		#p = feedback.pose.position
		#print feedback.marker_name + " is now at " + str(p.x) + ", " + str(p.y) + ", " + str(p.z)
	
	#发布线路
	def line_publish(self, start_point=Marker()):
		self.start_point.x = start_point.pose.position.x
		self.start_point.y = start_point.pose.position.y
		self.pub.publish(self.arrow)


if __name__=="__main__":
	
	#初始化节点
	rospy.init_node("Waypoint_Test")
	
	rate = rospy.Rate(30)
	
	#定义一个Marker
	manager = LinePath("map", "manager", "manager", True, 0)
	
	#创建waypoint
	wayline = []
	wayline.append(LinePath("map", "P_0", "P_0", False, 1, manager))
	
	for i in range(5):
		wayline.append(LinePath("map", "P_%d"%(i+1), "P_%d"%(i+1), False, i+1, wayline[i-1]))
	
	#创建server
	server = InteractiveMarkerServer("path")
	
	#添加waypoint和回调函数
	server.insert(manager, manager.processFeedback)
	for i in range(5):
		server.insert(wayline[i], wayline[i].processFeedback)
	
	#应用改变
	server.applyChanges()
	
	rospy.loginfo("line_publish...")
	
	while not rospy.is_shutdown():
		
		rate.sleep()
		
		manager.line_publish()
		for i in range(5):
			wayline[i+1].line_publish(wayline[i])














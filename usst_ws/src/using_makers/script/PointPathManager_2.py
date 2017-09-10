#!/usr/bin/env python
#coding=utf-8

"""
2017-5-2 by Bob
用于管理所有的位点，包括创建删除，加载保存位点

"""
import roslib; 
roslib.load_manifest("interactive_markers")
import rospy
import rospkg
import copy
import os
from visualization_msgs.msg import *
from interactive_markers.interactive_marker_server import *
from interactive_markers.menu_handler import *
from pointpath import PointPath
from pointpath_2 import LinePath
from geometry_msgs.msg import Point


#用于管理所有的位点，基于InteractiveMarkerServer类
class PointPathManager(InteractiveMarkerServer):

	def __init__(self, name, frame_id):
	
		#调用InteractiveMarkerServer
		InteractiveMarkerServer.__init__(self, name)
		
		#参考框架
		self.frame_id = frame_id
		
		#点的数目，以list的形式组织起来
		self.list_of_points = []
		
		#箭头的数目
		self.arrow_points_pair = []
		
		#点的数目
		self.counter_points = 0
		
		self.pub = rospy.Publisher("/lines", Marker, queue_size=1)
		
		#设置频率
		rate = rospy.Rate(30)
		
		#创建菜单
		self.menu_handler = MenuHandler()
		self.menu_handler.insert("Create a Waypoint", callback=self.createWaypointCB)
		entry = self.menu_handler.insert("Delete Waypoint")
		self.menu_handler.insert("Delete last Waypoint", parent=entry, callback = self.deletelastWaypointCB)
		self.menu_handler.insert("Delete all Waypoints", parent=entry, callback = self.deleteallWaypointCB)
		self.menu_handler.insert("Load all Waypoints", callback=self.loadAllWaypointsCB)
		self.menu_handler.insert("Save all Waypoints", callback=self.saveAllWaypointsCB)
		
		#创建地一个管理Point,管理节点不算入list中
		self.initial_point = LinePath("map", "manager", "manager", True, 0)
		
		#添加到服务器
		self.insert(self.initial_point, self.initial_point.processFeedback)
		
		#给一个Point添加菜单
		self.menu_handler.apply(self, self.initial_point.name)
		
		#应用变化
		self.applyChanges()
		
		#waypoints.txt的路径
		rp = rospkg.RosPack() #rospath
		self.points_file_path = os.path.join(rp.get_path("using_makers"), "script", "waypoints.txt")
		
		while not rospy.is_shutdown():
			if len(self.arrow_points_pair) >=0 :
				for i in range(len(self.arrow_points_pair)/2):
					#关键是要知道当前操作的点和创建的点的坐标，才能创建arrow
					self.list_of_points[self.arrow_points_pair[i]].line_publish(self.list_of_points[self.arrow_points_pair[i+1]])
			
			rate.sleep()
			
	#回调函数
	def createWaypointCB(self, feedback):
		print "createWaypointCB clicked on %s"%feedback.marker_name
		
		#创建新的点
		if len(self.list_of_points) != 0:
			new_point = LinePath("map", "P_%d"%(self.counter_points), "P_%d"%(self.counter_points),\
								False, self.counter_points, self.list_of_points[self.arrow_points_pair[]])
		else:
			new_point = LinePath("map", "P_%d"%(self.counter_points), "P_%d"%(self.counter_points),\
													False, self.counter_points, self.initial_point)
		new_point.pose.position.x = feedback.pose.position.x 
		new_point.pose.position.y = feedback.pose.position.y
		
		#新的点比正在操作的点在X轴上坐标大一
		new_point.pose.position.x = new_point.pose.position.x + 1.0
		
		#保存好需要创建箭头的一对点的
		if feedback.marker_name == "manager":
			name = []
			name = feedback.marker_name
			marker_id = -1
			self.arrow_points_pair.append(marker_id)
			name = new_point.name
			marker_id = ord(name[2]) - ord('0')
			self.arrow_points_pair.append(marker_id)
		else:
			name = []
			name = feedback.marker_name
			marker_id = ord(name[2]) - ord('0')
			self.arrow_points_pair.append(marker_id)
			self.arrow_points_pair.append(new_point)
			
		
		#添加新的点到list
		self.list_of_points.append(new_point)
		
		#点的数目加1
		self.counter_points = self.counter_points + 1
		
		#添加点和回调函数到服务器
		self.insert(new_point, new_point.processFeedback)
		
		#应用菜单
		self.menu_handler.apply(self, new_point.name)
		
		#应用变化
		self.applyChanges()
		
	def deletelastWaypointCB(self, feedback):
		print "deletelastWaypointCB"
		
		#如果点数目不是0
		if self.counter_points > 0:
		
			#删除list中的点
			p = self.list_of_points.pop()
			self.counter_points = self.counter_points -1
			
			#擦除服务器中的点
			self.erase(p.name)
			
			#应用改变
			self.applyChanges()
			
			
	def deleteAllPoints(self):
		for i in range(len(self.list_of_points)):
			p = self.list_of_points.pop()
			self.counter_points = self.counter_points - 1
			self.erase(p.name)
			
		self.applyChanges()
			
		
	def deleteallWaypointCB(self, feedback):
		print "deleteallWaypointCB"
		
		self.deleteAllPoints()
		
	def loadAllWaypointsCB(self, feedback):
		print "loadAllWaypointsCB"
		#delete all loaded points
		if len(self.list_of_points) > 0:
			self.deleteAllPoints()
			
		try:
			file_points = open(self.points_file_path, "r")
		except IOError, e:
			rospy.logerr("Failed to Open %s : %s"%(self.points_file_path, e))
			return 
		
		num_of_loaded_points = 0
		
		#读取每一行的信息
		line = file_points.readline().replace("\n", "")
		
		#不是空行
		while line != "":
			a = line.split(";")
			
			if len(a) == 3:
				new_point = PointPath(self.frame_id, a[0], a[0])
				new_point.pose.position.x = float(a[1])
				new_point.pose.position.y = float(a[2])
				
				self.list_of_points.append(new_point)
				self.insert(new_point, new_point.processFeedback)
				self.menu_handler.apply(self, new_point.name)
				self.applyChanges()
				self.counter_points = self.counter_points + 1
				num_of_loaded_points = num_of_loaded_points + 1
			else:
				rospy.logerr(" Error processing line %s"%(line))
				
			#继续读取一行数据
			line = file_points.readline().replace('\n', '')
			
		#关闭文件
		file_points.close()
		
		rospy.loginfo("Loaded %d points"%(num_of_loaded_points))
		
		
	def saveAllWaypointsCB(self, feedback):
		print "saveAllWaypointsCB"
		try:
			#打开waypoints.txt
			file_points = open(self.points_file_path, "w")
		except IOError, e:
			rospy.logerr("Failed to Open %s : %s"%(self.points_file_path, e))
			return
			
		for i in self.list_of_points:
			#(name, x, y)
			line = "%s;%.3f;%.3f\n"%(i.name, i.pose.position.x, i.pose.position.y)
			file_points.write(line)
		
		#关闭文件
		file_points.close()
		
		rospy.loginfo("Save %d waypoints"%(len(self.list_of_points)))
		
if __name__=="__main__":
	
	rospy.init_node("PointPathManager")
	
	PointPathManager("agv", "map")
	
	rospy.spin()
	
	
"""
InteractiveMarkerFeedback.msg的消息类型，从回调消息中的marker_name知道的关于当前操作点的信息
要知道InteractiveMarker的实时的坐标，需要在回调函数中读取


# Time/frame info.
Header header

# Identifying string. Must be unique in the topic namespace.
string client_id

# Feedback message sent back from the GUI, e.g.
# when the status of an interactive marker was modified by the user.

# Specifies which interactive marker and control this message refers to
string marker_name
string control_name

# Type of the event
# KEEP_ALIVE: sent while dragging to keep up control of the marker
# MENU_SELECT: a menu entry has been selected
# BUTTON_CLICK: a button control has been clicked
# POSE_UPDATE: the pose has been changed using one of the controls
uint8 KEEP_ALIVE = 0
uint8 POSE_UPDATE = 1
uint8 MENU_SELECT = 2
uint8 BUTTON_CLICK = 3

uint8 MOUSE_DOWN = 4
uint8 MOUSE_UP = 5

uint8 event_type

# Current pose of the marker
# Note: Has to be valid for all feedback types.
geometry_msgs/Pose pose

# Contains the ID of the selected menu entry
# Only valid for MENU_SELECT events.
uint32 menu_entry_id

# If event_type is BUTTON_CLICK, MOUSE_DOWN, or MOUSE_UP, mouse_point
# may contain the 3 dimensional position of the event on the
# control.  If it does, mouse_point_valid will be true.  mouse_point
# will be relative to the frame listed in the header.
geometry_msgs/Point mouse_point
bool mouse_point_valid
"""

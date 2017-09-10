#!/usr/bin/env python
#coding=utf-8

"""
用于管理所有的位点，包括创建删除，加载保存位点

"""
import roslib; 
roslib.load_manifest("interactive_markers")
import rospy
import rospkg
import copy
import os
from interactive_markers.interactive_marker_server import *
from interactive_markers.menu_handler import *
from pointpath import PointPath


#用于管理所有的位点，基于InteractiveMarkerServer类
class PointPathManager(InteractiveMarkerServer):

	def __init__(self, name, frame_id):
	
		#调用InteractiveMarkerServer
		InteractiveMarkerServer.__init__(self, name)
		
		#参考框架
		self.frame_id = frame_id
		
		#点的数目，以list的形式组织起来
		self.list_of_points = []
		
		#点的数目
		self.counter_points = 0
		
		#创建菜单
		self.menu_handler = MenuHandler()
		self.menu_handler.insert("Create a Waypoint", callback=self.createWaypointCB)
		entry = self.menu_handler.insert("Delete Waypoint")
		self.menu_handler.insert("Delete last Waypoint", parent=entry, callback = self.deletelastWaypointCB)
		self.menu_handler.insert("Delete all Waypoints", parent=entry, callback = self.deleteallWaypointCB)
		self.menu_handler.insert("Load all Waypoints", callback=self.loadAllWaypointsCB)
		self.menu_handler.insert("Save all Waypoints", callback=self.saveAllWaypointsCB)
		
		#创建地一个管理Point,管理节点不算入list中
		self.initial_point = PointPath(self.frame_id, "PointManager", "PointManager", True)
		
		#添加到服务器
		self.insert(self.initial_point, self.initial_point.processFeedback)
		
		#给一个Point添加菜单
		self.menu_handler.apply(self, self.initial_point.name)
		
		#应用变化
		self.applyChanges()
		
		#waypoints.txt的路径
		rp = rospkg.RosPack() #rospath
		self.points_file_path = os.path.join(rp.get_path("using_makers"), "script", "waypoints.txt")
		
	
	#回调函数
	def createWaypointCB(self, feedback):
		print "createWaypointCB"
		
		#创建新的点
		new_point = PointPath(self.frame_id, "P_%d"%(self.counter_points), "P_%d"%(self.counter_points))
		
		if len(self.list_of_points) != 0:
			new_point.pose.position.x = self.list_of_points[self.counter_points - 1].pose.position.x
			new_point.pose.position.y = self.list_of_points[self.counter_points - 1].pose.position.y
		
		#新的点比之前的点在X轴上坐标大一
		new_point.pose.position.x = new_point.pose.position.x + 1.0
		
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
	
	
	
	
	
	
	


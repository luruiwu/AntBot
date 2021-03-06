#!/usr/bin/env python
#coding=utf-8
"""
在两个marker之间用箭头连接
"""
import rospy
from visualization_msgs.msg import *
from geometry_msgs.msg import Point
from interactive_markers.interactive_marker_server import *

global P_1
global P_0
global arrow

def processFeedback_2(feedback):
	p = feedback.pose.position
	
	P_0.x = p.x
	P_0.y = p.y
	P_0.z = p.z
	
	#print feedback.marker_name + " is now at " + str(p.x) + ", " + str(p.y) + ", " + str(p.z)
	
def processFeedback(feedback):
	p = feedback.pose.position
	
	P_1.x = p.x
	P_1.y = p.y
	P_1.z = p.z
	
	#print feedback.marker_name + " is now at " + str(p.x) + ", " + str(p.y) + ", " + str(p.z)


if __name__=="__main__":
	
	rospy.init_node("lines", anonymous=True)
	
	pub = rospy.Publisher("/lines", Marker, queue_size=10)
	
	server = InteractiveMarkerServer("interactive_marker_server")
	
	rospy.loginfo("Initializing...")
	
	P_1 = Point()
	P_0 = Point()
	
	rate = rospy.Rate(20)
	
	#arrow
	arrow = Marker()
	arrow.header.frame_id = "map"
	arrow.header.stamp = rospy.Time.now()
	arrow.ns = "arrow"
	arrow.action = Marker.ADD
	arrow.id = 0
	arrow.type = Marker.ARROW
	arrow.scale.x = 0.04 #shaft diameter
	arrow.scale.y = 0.12 #head diameter
	arrow.scale.z = 0.3  #head length
	arrow.color.r = 1.0
	arrow.color.a = 0.5
	arrow.points = [0 for i in range(2)]
	arrow.points[0] = P_0
	arrow.points[1] = P_1
	
	#waypoint
	waypoint = Marker()
	waypoint.type = Marker.CYLINDER
	waypoint.scale.x = 0.2
	waypoint.scale.y = 0.2
	waypoint.scale.z = 0.4
	waypoint.pose.position.z = 0.2
	waypoint.color.r = 0.0
	waypoint.color.g = 0.8
	waypoint.color.b = 0.0
	waypoint.color.a = 0.5
	
	#waypoint_control
	waypoint_control = InteractiveMarkerControl()
	waypoint_control.always_visible = True
	waypoint_control.name = "waypoint_control"
	waypoint_control.orientation.w = 1.0
	waypoint_control.orientation.x = 0.0
	waypoint_control.orientation.y = 1.0
	waypoint_control.orientation.z = 0.0
	waypoint_control.interaction_mode = InteractiveMarkerControl.MOVE_PLANE
	waypoint_control.markers.append(waypoint)
	
	#int_marker
	int_marker = InteractiveMarker()
	int_marker.header.frame_id = "map"
	int_marker.name = "int_marker"
	int_marker.description = "int_marker"
	int_marker.controls.append(waypoint_control)
	
	#int_marker_2
	int_marker_2 = InteractiveMarker()
	int_marker_2.header.frame_id = "map"
	int_marker_2.name = "int_marker_2"
	int_marker_2.description = "int_marker_2"
	int_marker_2.controls.append(waypoint_control) 
	
	server.insert(int_marker, processFeedback)
	
	server.insert(int_marker_2, processFeedback_2)
	
	server.applyChanges()
	
	while not rospy.is_shutdown():
		
		arrow.points[0] = P_0
		arrow.points[1] = P_1
		
		pub.publish(arrow)
		
		rate.sleep()
		
	
	
	
	
	
	
	
	

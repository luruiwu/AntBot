#!/usr/bin/env python
#coding=utf-8

""" odom_out_and_back.py 
    使用/odom话题让机器人移动给定的距离或者旋转给定的角度
      
"""
import rospy
from geometry_msgs.msg import Twist, Point, Quaternion
import tf
from transform_utils import quat_to_angle, normalize_angle
from math import radians, copysign, sqrt, pow, pi

class OutAndBack():
    def __init__(self):
        # Give the node a name
        rospy.init_node('out_and_back', anonymous=True)

        # Set rospy to execute a shutdown function when exiting
        rospy.on_shutdown(self.shutdown)

        # queue_size一定要等于1,否则会导致，速度命令进入队列，导致车子无法停止
        self.cmd_vel = rospy.Publisher('/cmd_vel', Twist, queue_size=1)
        
        # How fast will we update the robot's movement?
        rate = 20
        
        # Set the equivalent ROS rate variable
        r = rospy.Rate(rate)
        
        # 设置目标的前进速度
        goal_linear_speed = 0.3
        
        #当前的线性速度
        current_linear_speed = 0.3
        
        # Set the travel distance in meters
        goal_distance = 4.0

        # Initialize the tf listener
        self.tf_listener = tf.TransformListener()
        
        # Give tf some time to fill its buffer
        rospy.sleep(2)
        
        # Set the odom frame
        self.odom_frame = '/odom'
        
        # Find out if the robot uses /base_link or /base_footprint
        try:
            self.tf_listener.waitForTransform(self.odom_frame, '/base_link', rospy.Time(), rospy.Duration(1.0))
            self.base_frame = '/base_link'
        except (tf.Exception, tf.ConnectivityException, tf.LookupException):
            rospy.loginfo("Cannot find transform between /odom and /base_link or /base_footprint")
            rospy.signal_shutdown("tf Exception")
        
        # Initialize the position variable as a Point type
        position = Point()
            
        # Loop once for each leg of the trip
        for i in range(2):
            # Initialize the movement command
            move_cmd = Twist()
            
            # 走来回
            if (i == 0):
                move_cmd.linear.x = current_linear_speed
                move_cmd.linear.y = 0
            if (i == 1):
                move_cmd.linear.x = -current_linear_speed
                move_cmd.linear.y = 0
            #走矩形
            """
            if (i == 0):
                move_cmd.linear.x = linear_speed
                move_cmd.linear.y = 0
            if (i == 1):
                move_cmd.linear.x = 0
                move_cmd.linear.y = linear_speed
            elif (i == 2):
                move_cmd.linear.x = 0 - linear_speed
                move_cmd.linear.y = 0
            elif (i == 3):
                move_cmd.linear.x = 0
                move_cmd.linear.y = 0 - linear_speed
            """
            
            # Get the starting position values     
            (position, rotation) = self.get_odom()
            
            x_start = position.x
            y_start = position.y
            
            # Keep track of the distance traveled
            distance = 0
            
            # Enter the loop to move along a side
            while distance < goal_distance and not rospy.is_shutdown():
            
                # Publish the Twist message and sleep 1 cycle  
               # move_cmd.linear.x = current_linear_speed
                #move_cmd.linear.y = 0
                self.cmd_vel.publish(move_cmd)
                
                r.sleep()
        
                # Get the current position
                (position, rotation) = self.get_odom()
                
                # Compute the Euclidean distance from the start
                distance = sqrt(pow((position.x - x_start), 2) + pow((position.y - y_start), 2))
                
                #在开始走和快要到目的地的时候减小速度,设置加速和减速距离0.2m，0.2*5等于1
                #减速
                """
                if (abs(goal_distance - distance) < 0.2):
                    current_linear_speed = abs(goal_distance - distance) / 0.2 * goal_linear_speed
                    if current_linear_speed < 0.02:
                        current_linear_speed = 0.02
                    if current_linear_speed > goal_linear_speed:
                        current_linear_speed = goal_linear_speed
                #加速   
                if (distance < 0.2):
                    current_linear_speed = distance / 0.1 * goal_linear_speed
                    if current_linear_speed < 0.02:
                        current_linear_speed = 0.02
                    if current_linear_speed > goal_linear_speed:
                        current_linear_speed = goal_linear_speed
                """
                #print "distance: %f, current_linear_x: %f"%(distance, move_cmd.linear.x)

            # Stop the robot before next action
            self.cmd_vel.publish(Twist())
            rospy.sleep(3)
        # Stop the robot for good
        self.cmd_vel.publish(Twist())
        
    def get_odom(self):
        # Get the current transform between the odom and base frames
        try:
            (trans, rot)  = self.tf_listener.lookupTransform(self.odom_frame, self.base_frame, rospy.Time(0))
        except (tf.Exception, tf.ConnectivityException, tf.LookupException):
            rospy.loginfo("TF Exception")
            return

        return (Point(*trans), quat_to_angle(Quaternion(*rot)))
        
    def shutdown(self):
        # Always stop the robot when shutting down the node.
        rospy.loginfo("Stopping the robot...")
        self.cmd_vel.publish(Twist())
        rospy.sleep(1)
 
if __name__ == '__main__':
    try:
        print "OutAndBack"
        #while True:
        OutAndBack()
    except:
        rospy.loginfo("Out-and-Back node terminated.")


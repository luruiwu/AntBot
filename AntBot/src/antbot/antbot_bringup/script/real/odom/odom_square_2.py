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
        current_linear_speed = 0.02

        # Set the travel distance in meters
        goal_distance = list()
        direction_x   = list()
        goal_distance = [4.0, 2.0, 9.0, 5.0, 1.6]
        direction_x = [1, 0, 1, 0, -1]
        direction_y = [0, -1, 0, 1, 0]

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
        self.position = Point()

        # Initialize the movement command
        move_cmd = Twist()

        self.rotation = quat_to_angle(Quaternion())
        # Get the starting position values
        
        (self.position, self.rotation) = self.get_odom()
        
        self.x_start = self.position.x
        self.y_start = self.position.y

        # Keep track of the distance traveled
        distance = 0
        
        #1代表住正方向，0停止，-1负轴方向
        for i in range(5):
            self.move_to_goal(goal_distance[i], direction_x[i], direction_y[i])
            print "goal_distance_1 got"
            
        rospy.sleep(3)
        
        for i in range(5):
            self.move_to_goal(goal_distance[4 - i], -direction_x[4 - i], -direction_y[4 - i])
            print "goal_distance_1 got"

        # Stop the robot for good
        self.cmd_vel.publish(Twist())

    def move_to_goal(self, goal_distance, dir_x, dir_y):

        distance = 0.0
        
        r = rospy.Rate(20)
        
        move_cmd = Twist()
        
        current_linear_speed = 0.02
        
        goal_linear_speed  = 0.3
        
        (self.position, self.rotation) = self.get_odom()
        
        self.x_start = self.position.x
        self.y_start = self.position.y
        
        # Enter the loop to move along a side
        while distance < goal_distance and not rospy.is_shutdown():
            
            # Publish the Twist message and sleep 1 cycle
            move_cmd.linear.x = current_linear_speed * dir_x
            move_cmd.linear.y = current_linear_speed * dir_y
            self.cmd_vel.publish(move_cmd)

            r.sleep()

            # Get the current position
            (self.position, self.rotation) = self.get_odom()

            # Compute the Euclidean distance from the start
            distance = sqrt(pow((self.position.x - self.x_start), 2) + pow((self.position.y - self.y_start), 2))

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
                       
            print "distance: %f, current_linear_x: %f"%(distance, move_cmd.linear.x)
            
        self.cmd_vel.publish(Twist())
        rospy.sleep(0.1)
            
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
        OutAndBack()
    except:
        rospy.loginfo("Out-and-Back node terminated.")


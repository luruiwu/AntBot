#!/usr/bin/env python
#coding=utf-8

""" odom_out_and_back.py
    使用/odom话题让机器人移动给定的距离或者旋转给定的角度
    带速度缓冲

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
        
        goal_distance = [2.0, 2.0, 7.0]
        direction_x = [-1, 0, -1]
        direction_y = [0, -1, 0]

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
        
        #旋转180度
        #self.move_angle(pi)  
        #1代表住正方向，0停止，-1负轴方向
        
        for i in range(3):
            self.move_distance(goal_distance[i], direction_x[i], direction_y[i])
            print "goal_distance get"
        
        self.move_angle(pi)     
        rospy.sleep(0.5)
        
        #for i in range(2):
        self.move_distance(goal_distance[2], direction_x[2], direction_y[2])
        self.move_distance(goal_distance[1], direction_x[1], direction_y[1])
        self.move_distance(goal_distance[0], direction_x[0], direction_y[0])
        #print "goal_distance get"
        
        self.move_angle(pi)  
        rospy.sleep(0.5)
        
        # Stop the robot for good
        self.cmd_vel.publish(Twist())

    #转动指定的角度
    def move_angle(self, goal_angle):
    
        move_cmd = Twist()
        
        # 设置旋转角度的允许误差
        angular_tolerance = radians(0.01)
        
        #角速度
        goal_angular_speed = 1.2
        
        current_angular_speed = 0.02
        
        #最小的角速度值
        min_angular_speed = 0.02
        
        turn_angle = 0.0
        
        delta_angle = 0.0
        
        # 设置旋转角速度
        move_cmd.angular.z = current_angular_speed

        #获取初始角度值
        (self.position, self.rotation) = self.get_odom()
        
        last_angle = self.rotation
        
        r = rospy.Rate(20)

        while abs(turn_angle + angular_tolerance) < abs(goal_angle) and not rospy.is_shutdown():
        
            # 发布速度命令
            move_cmd.angular.z = current_angular_speed
            self.cmd_vel.publish(move_cmd)

            r.sleep()
            
            # 获取当前旋转角度
            (self.position, self.rotation) = self.get_odom()
            
            # 计算相对于上一次旋转的角度
            delta_angle = normalize_angle(self.rotation - last_angle)
            
            # 添加到旋转角度中
            turn_angle += delta_angle
            last_angle = self.rotation
            print "turn_angle: %f delta_angle: %f "%(turn_angle, delta_angle)
            
            if ((goal_angle - abs(turn_angle)) < radians(20)):
                
                current_angular_speed = (goal_angle - abs(turn_angle)) / radians(20) * goal_angular_speed
                if current_angular_speed < min_angular_speed:
                    current_angular_speed = min_angular_speed
                if current_angular_speed > goal_angular_speed:
                    current_angular_speed = goal_angular_speed
            #加速
            if (abs(turn_angle) < radians(5)):
                current_angular_speed = abs(turn_angle) / radians(5) * goal_angular_speed
                if current_angular_speed < min_angular_speed:
                    current_angular_speed = min_angular_speed
                if current_angular_speed > goal_angular_speed:
                    current_angular_speed = goal_angular_speed
            
            print current_angular_speed
            
        # 下一次循环之前停止机器人
        self.cmd_vel.publish(Twist())
        rospy.sleep(0.001)
        
    
    def move_distance(self, goal_distance, dir_x, dir_y):

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
        rospy.sleep(0.001)
            
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
        rospy.sleep(0.1)

if __name__ == '__main__':
    try:
        print "OutAndBack"
        OutAndBack()
    except:
        rospy.loginfo("Out-and-Back node terminated.")


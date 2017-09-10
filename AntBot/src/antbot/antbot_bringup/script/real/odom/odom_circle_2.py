#!/usr/bin/env python
#coding=utf-8

""" 
    2016-11-13
    走曲线
"""
import rospy
from geometry_msgs.msg import Twist, Point, Quaternion
import tf
from transform_utils import quat_to_angle, normalize_angle
from math import radians, copysign, sqrt, pow, pi

class OutAndBack():
    def __init__(self):
        # Give the node a name
        rospy.init_node('circle', anonymous=True)

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
        
        ###############################################################################

        #self.move_distance(1.5, 1, 0)
        
        ##走一个半径为1m，角度为90的圆弧
        self.circle(2, 270.0)
        

        ###############################################################################
        
        #根据线速度和角速度之间的比例走出指定半径的圆弧
        #r: 圆弧的半径(米)
        #goal_angle:圆弧的角度(度)
    def circle(self, r, goal_angle):        
        #线速度和角速度成一定的比例
        move_cmd = Twist()
        
        last_position = Point()
        
        #圆弧的角度(弧度)
        goal_angle = radians(goal_angle)
        
        #角度允许误差
        angular_tolerance = radians(0.01)
        
        #获取初始角度值
        (self.position, self.rotation) = self.get_odom()
        
        #初始值
        last_angle      = self.rotation
        last_position.x = self.position.x
        last_position.y = self.position.y
        total_s         = 0.0
        err_angle       = 0.0
        last_err_angle  = 0.0
        next_err_angle  = 0.0
        delta_x = 0.0
        delta_y = 0.0
        delta_angle = 0.0
        delta_s = 0.0
        actual_angle = 0.0

        #先确定一个线速度
        move_cmd.linear.x  = 0.2
        move_cmd.angular.z = move_cmd.linear.x / r + 0.1
        
        #pid的参数
        kp = 0.05
        ki = 0.15
        kd = 0.2
        
        rate = rospy.Rate(20)
        
        while abs(actual_angle + angular_tolerance) < abs(goal_angle) and not rospy.is_shutdown():
            
            rate.sleep()
            
            # 获取当前位置姿态
            (self.position, self.rotation) = self.get_odom()
            #与上一次的位置姿态差
            delta_x = self.position.x - last_position.x
            delta_y = self.position.y - last_position.y
            delta_angle = normalize_angle(self.rotation - last_angle)
            
            #用直线逼近圆弧
            delta_s = sqrt(delta_x * delta_x + delta_y * delta_y)
        
            #总的走过弧长
            total_s += delta_s

            #实际转动的角度
            actual_angle += delta_angle

            #应该转过的角度
            set_angle = total_s / r

            #角度误差
            err_angle = set_angle - actual_angle
            
            print "err_angle: %f"%(err_angle * 180.0 / pi)
            #根据角度误差设置角速度
            move_cmd.angular.z += kp * (err_angle - next_err_angle) + ki * err_angle + kd * (err_angle - 2 * next_err_angle + last_err_angle)
                                   
            
            #发布速度
            self.cmd_vel.publish(move_cmd)
        
            #迭代
            last_position.x = self.position.x
            last_position.y = self.position.y
            last_err_angle  = next_err_angle
            next_err_angle  = err_angle
            last_angle      = self.rotation
            
            ##############################PID程序#############################
            
            #print "turn_angle: %f"%(actual_angle * 180.0 / pi)
            
        #停止机器人
        self.cmd_vel.publish(Twist())
        rospy.sleep(0.001)
        
        
    #转动指定的角度
    def move_angle(self, goal_angle):
    
        move_cmd = Twist()
        
        # 设置旋转角度的允许误差
        angular_tolerance = radians(0.01)
        
        #角速度
        goal_angular_speed = 0.06
        
        current_angular_speed = 0.06
        
        #最小的角速度值
        min_angular_speed = 0.06
        
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
        #self.cmd_vel.publish(Twist())
        #rospy.sleep(0.001)
        
    #1代表住正方向，0停止，-1负轴方向
    def move_distance(self, goal_distance, dir_x, dir_y):

        distance = 0.0
        
        r = rospy.Rate(20)
        
        move_cmd = Twist()
        
        current_linear_speed = 0.5
        
        goal_linear_speed  = 0.5
        
        min_linear_speed = 0.5
        
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
                    current_linear_speed = min_linear_speed
                if current_linear_speed > goal_linear_speed:
                    current_linear_speed = goal_linear_speed
            #加速
            if (distance < 0.2):
                current_linear_speed = distance / 0.1 * goal_linear_speed
                if current_linear_speed < 0.02:
                    current_linear_speed = min_linear_speed
                if current_linear_speed > goal_linear_speed:
                    current_linear_speed = goal_linear_speed
                       
            print "distance: %f, current_linear_x: %f"%(distance, move_cmd.linear.x)
            
        #self.cmd_vel.publish(Twist())
        #rospy.sleep(0.001)
            
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


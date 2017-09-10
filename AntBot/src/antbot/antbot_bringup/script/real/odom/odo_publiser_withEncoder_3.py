#!/usr/bin/env python
#coding=utf-8

import roslib; roslib.load_manifest('agv')
#添加此句给pythonimport 时 寻找其他的文件用来
import sys

spath=sys.path[0]
snum=spath.rfind('/')
ssyspath=spath[0:snum]
sys.path.append(ssyspath)

import rospy
import tf
from agv.msg            import Num, carOdom, MImu
from geometry_msgs.msg  import Twist, Quaternion, TransformStamped
from math               import sin, cos ,pi
from sensor_msgs.msg    import JointState
from nav_msgs.msg       import Odometry
from tf.transformations import euler_from_quaternion
from tf.transformations import quaternion_from_euler

class PublishOdom():
    def __init__(self):
        #初始化节点
        rospy.init_node("PublishOdom", anonymous = True)
        
        #结束节点时执行nodeshutdown函数
        rospy.on_shutdown(self.nodeShutdown)
        
        #订阅car_speed话题
        rospy.Subscriber("/car_speed", carOdom, self.speed_callback)
        
        #订阅速度话题
        rospy.Subscriber("/cmd_vel", Twist, self.cmd_callback)
        
        #关节坐标发布者
        self.joint_pub = rospy.Publisher("/joint_state", JointState, queue_size = 20)

        #里程信息发布者
        self.odom_pub = rospy.Publisher("/odom", Odometry, queue_size = 20)
        
        self.odom_broadcaster = tf.TransformBroadcaster()
        self.cmd_msg          = Twist()
        
        self.xPosition  = 0.0
        self.yPosition  = 0.0
        self.thPosition = 0.0
        
        self.vx = 0.0
        self.vy = 0.0
        self.vth = 0.0
        
        self.last_time    = 0
        self.current_time = 0
        
        #包含编码器数据和陀螺仪数据
        self.current_odom_msg    = carOdom()
        self.last_odom_msg       = carOdom()
        
        self.FistTime = True
        
        #编码轮的系数
        k_x = 0.872
        k_y = 0.874

        #编码轮半径
        radius = 0.058

        #计算位置的系数
        self.pk_x = 2 * pi * radius * k_x / 2400.0
        self.pk_y = 2 * pi * radius * k_y / 2400.0
            
    #速度回调函数
    def speed_callback(self, msg):

        #读取msg
        #如果是第一次进入回调函数
        if  self.FistTime:
            #将last_odom_msg和current_odom_msg设置为相同的初始值，第一次的值
            self.current_odom_msg = msg
            self.last_odom_msg    = self.current_odom_msg
            
            #将时间设置为一样，第一次的rospy.Time.now().to_sec()
            self.current_time     = rospy.Time.now().to_sec()
            self.last_time        = self.current_time
            #self.FistTime = False
        else:
            self.current_odom_msg = msg
            self.current_time     = rospy.Time.now().to_sec()
                
        #单位时间的脉冲数
        diff_x = self.current_odom_msg.xOdom - self.last_odom_msg.xOdom
        diff_y = self.current_odom_msg.yOdom - self.last_odom_msg.yOdom
        
        #特殊情况
        if diff_x > 3000:
            diff_x = diff_x - 4800
        elif diff_x < -3000:
            diff_x = diff_x + 4800
    
        if diff_y > 3000:
            diff_y = diff_y - 4800
        elif diff_y < -3000:
            diff_y = diff_y + 4800
        
        #机器人坐标上单位时间内的位姿变化
        i = diff_x * self.pk_x
        j = diff_y * self.pk_y
        
        """
            判断是否有旋转运动如果有，这里只有两种运动形式，平移和旋转。
        """

        #平移
        if not self.cmd_msg.angular.z == 0:
            delta_x = 0.0
            delta_y = 0.0    
        #旋转
        else:
            delta_x = i * cos(self.thPosition) - j * sin(self.thPosition)
            delta_y = i * sin(self.thPosition) + j * cos(self.thPosition)
        
        #时间差
        delta_time = self.current_time - self.last_time

        #航向角差
        delta_th   = self.current_odom_msg.angle_z - self.last_odom_msg.angle_z
        
        #速度(第一次的delta_time为0)
        if  not self.FistTime:
            self.vx  = delta_x / delta_time
            self.vy  = delta_y / delta_time
            self.vth = delta_th / delta_time
        
        #累计的位置
        self.xPosition  += delta_x
        self.yPosition  += delta_y
        self.thPosition += delta_th
        
        
        #############################################################################
        #发布关节的坐标关系
        joint_state = JointState()
        joint_state.header.stamp = rospy.Time.now()
        joint_state.name.append("base_to_wheel1")
        joint_state.position.append(0)
        joint_state.name.append("base_to_wheel2")
        joint_state.position.append(1)
        joint_state.name.append("base_to_wheel3")
        joint_state.position.append(2)
        joint_state.name.append("base_to_wheel4")
        joint_state.position.append(3)
        self.joint_pub.publish(joint_state)
        
        #发布TF关系
        odom_quat = Quaternion()
        odom_quat = quaternion_from_euler(0, 0, self.thPosition)
        self.odom_broadcaster.sendTransform((self.xPosition, self.yPosition, 0),
                                            odom_quat,
                                            rospy.Time.now(),
                                            "odom",
                                            "base_link")
        #发布航迹推演消息
        #定义额一个Odometry消息
        odom = Odometry()
        odom.header.stamp = rospy.Time.now()
        odom.header.frame_id = "odom"

        #设置位姿
        odom.pose.pose.position.x = self.xPosition
        odom.pose.pose.position.y = self.yPosition
        odom.pose.pose.position.z = 0.0
        odom.pose.pose.orientation.x = odom_quat[0]
        odom.pose.pose.orientation.y = odom_quat[1]
        odom.pose.pose.orientation.z = odom_quat[2]
        odom.pose.pose.orientation.w = odom_quat[3]

        #设置速度
        odom.child_frame_id = "base_link"
        odom.twist.twist.linear.x = self.vx
        odom.twist.twist.linear.y = self.vy
        odom.twist.twist.angular.z = self.vth

        #协方差
        odom.pose.covariance[0]  = 0.1
        odom.pose.covariance[7]  = 0.1
        odom.pose.covariance[35] = 0.2
        odom.pose.covariance[14] = 10000000 #set a non-zero covariance on unused
        odom.pose.covariance[21] = 10000000 #dimensions (z, pitch and roll); this
        odom.pose.covariance[28] = 10000000 #is a requirement of robot_pose_ekf
        self.odom_pub.publish(odom)
        
        #迭代
        self.last_odom_msg = self.current_odom_msg
        self.last_time  = self.current_time
        if self.FistTime:
            self.FistTime = False
        
    #速度回调函数
    def cmd_callback(self, msg):
        #读取旋转的角速度
        self.cmd_msg.angular.z = msg.angular.z
           
    #节点结束时的执行函数
    def nodeShutdown(self):
        print "PublishOdom node is shutdown"

if __name__=="__main__":
    try:
        print "being PublishOdom..."
        PublishOdom()
        rospy.spin()
    except:
        rospy.loginfo("PublishOdom node terminated !")
        

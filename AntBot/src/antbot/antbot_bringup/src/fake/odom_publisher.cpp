/*
订阅机器人的速度，发布轮子的Joint信息
发布TF关系
*/
#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <antbot_bringup/antbotOdom.h>
#include <sensor_msgs/JointState.h>

class PublishOdom
{
private:
    ros::NodeHandle n;
    ros::Publisher odom_pub;
    ros::Subscriber antbotspeed_sub;
    ros::Time current_time,last_time;
    ros::Publisher joint_pub;
    tf::TransformBroadcaster odom_broadcaster;
    double x_sum;
    double y_sum;
    double th_sum;
    float L;//antbot半
    float W;//antbot半宽
    float R;//antbot轮子的半径
    float v[4];
    float p[4];
    
public:
    PublishOdom(){
        odom_pub = n.advertise<nav_msgs::Odometry>("odom", 1);
        joint_pub = n.advertise<sensor_msgs::JointState>("wheel_joint_states", 1);
        antbotspeed_sub = n.subscribe("antbot_speed", 1, &PublishOdom::sub_callrear, this);
        L = 0.4;
        W = 0.3;
        R = 0.12;
    }
    void sub_callrear(const antbot_bringup::antbotOdom::ConstPtr& input)
     {
         current_time=ros::Time::now();
         float vx = input->vx;//the x speed of the antbot
         float vy = input->vy;//the y speed of the antbot
         float vth = input->vth;//the thea speed of the antbot
         //compute odometry in a typical way given the velocities of the robot
         double dt = (current_time - last_time).toSec();
         double delta_x  = vx * dt; //(vx * cos(th_sum) - vy * sin(th_sum)) * dt;
         double delta_y  = vy * dt; //(vx * sin(th_sum) + vy * cos(th_sum)) * dt;
         double delta_th = vth * dt;
         th_sum += delta_th;
         printf("th_sum : %f\n", th_sum);
         x_sum  += delta_x * cos(th_sum) - delta_y * sin(th_sum);
         y_sum  += delta_x * sin(th_sum) + delta_y * cos(th_sum);
         
         
         v[0] = vx + vy + (L + W) * vth;
         v[1] = vx - vy - (L + W) * vth;
         v[2] = vx + vy - (L + W) * vth;
         v[3] = vx - vy + (L + W) * vth;


		 for(int i = 0; i < 4; i++)
	         p[i] += (v[i]/R)*dt;
	     

         sensor_msgs::JointState joint_state;
         //update joint_state
         joint_state.header.stamp = ros::Time::now();
         joint_state.name.resize(4);
         joint_state.position.resize(4);
         joint_state.name[0] ="front_right_wheel_to_base_link";
         joint_state.position[0] = p[0];
         joint_state.name[1] ="front_left_wheel_to_base_link";
         joint_state.position[1] = p[1];
         joint_state.name[2] ="rear_left_wheel_to_base_link";
         joint_state.position[2] = p[2];
         joint_state.name[3] ="rear_right_wheel_to_base_link";
         joint_state.position[3] = p[3];
         //send the joint state and transform
         joint_pub.publish(joint_state);


         //since all odometry is 6DOF we'll need a quaternion created from yaw
         /* 将里程计的偏航角转换成四元数，四元数效率高，这样使用二维和三维的功能包是一样的.*/
        geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th_sum);
        //first, we'll publish the transform over tf
        geometry_msgs::TransformStamped odom_trans;
        odom_trans.header.stamp = current_time;
        odom_trans.header.frame_id = "odom";
        odom_trans.child_frame_id = "base_link";
        odom_trans.transform.translation.x = x_sum;
        odom_trans.transform.translation.y = y_sum;
        odom_trans.transform.translation.z = 0.0;
        odom_trans.transform.rotation = odom_quat;
        //send the transform
        odom_broadcaster.sendTransform(odom_trans);


         //next, we'll publish the odometry message over ROS
        nav_msgs::Odometry odom;
        odom.header.stamp = current_time;
        odom.header.frame_id = "odom";
        //set the position
        odom.pose.pose.position.x = x_sum;
        odom.pose.pose.position.y = y_sum;
        odom.pose.pose.position.z = 0.0;
        odom.pose.pose.orientation = odom_quat;
        //set the velocity
        odom.child_frame_id = "base_link";
        odom.twist.twist.linear.x = vx;
        odom.twist.twist.linear.y = vy;
        odom.twist.twist.angular.z = vth;

        //publish the message
        odom.pose.covariance[0]  = 0.1;
        odom.pose.covariance[7]  = 0.1;
        odom.pose.covariance[35] = 0.2;

        odom.pose.covariance[14] = DBL_MAX; // set a non-zero covariance on unused
        odom.pose.covariance[21] = DBL_MAX; // dimensions (z, pitch and roll); this
        odom.pose.covariance[28] = DBL_MAX; // is a requirement of robot_pose_ekf
        odom_pub.publish(odom);

        last_time = current_time;
     }

};
int main(int argc,char** argv)
{
    ros::init(argc,argv,"odometry_publiser");
    // topic you want to publish
    PublishOdom PublishOdomObject;
    
    ros::spin();
    return 0;
}

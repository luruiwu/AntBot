#ifndef YUN_OBSTACLE_AVOID_H_
#define YUN_OBSTACLE_AVOID_H_

/***/

#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>
#include <stdio.h>
#include <std_srvs/Empty.h>
#include <std_msgs/UInt8.h> //for UInt8.msg
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <math.h>

#define LONG_CHECK_RANGE  1.0
#define SHORT_CHECK_RANGE  0.6
#define SIDE_CHECK_RANGE  0.6

namespace dynamic_navigation{
	
  typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

  enum AgvState{
    NORMAL=0,
    OBSTACLE=1
  };


  enum Direction{
    Forward=0x01,
    Backward=0x02,
    Both=0x03
  };


  class DynamicNavigation{

    public:
      DynamicNavigation();

      ~DynamicNavigation();

      void laserScanCallback(const sensor_msgs::LaserScanConstPtr& message);

      void cmdCallback(const geometry_msgs::TwistConstPtr& cmd_msg);

      void goalCB(const move_base_msgs::MoveBaseActionGoal::ConstPtr& goal);

      void move_base_statusCallback(const actionlib_msgs::GoalStatusArray::ConstPtr& msg);

      void aroundSafty();

    private:
      move_base_msgs::MoveBaseGoal current_goal;

      AgvState CurrentState, LastState;

      Direction agv_dir;

      sensor_msgs::LaserScan laser_msg;

      ros::Publisher action_goal_pub_;

      int move_base_status;

      int inc;

      std_msgs::UInt8 obstacle_msg;

      float robot_x;
      float robot_y;
      float dis_1;
      float dis_2;

      tf::TransformListener listener;
      tf::StampedTransform transform;

      //上一次到达的目标
      float goal_x;
      float goal_y;

      float  check_distance_1;
      float  check_distance_2;
  };

};


#endif



#ifndef YUN_OBSTACLE_AVOID_H_
#define YUN_OBSTACLE_AVOID_H_

#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>


namespace dynamic_navigation{
	
	typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

	enum AgvState{
		NORMAL,
		OBSTACLE
	};
	
	
	enum Direction{
		Forward,
		Backward
	};
	
	class DynamicNavigation{
		
		public:
			DynamicNavigation();
		
			~DynamicNavigation();
			
			void laserScanCallback(const sensor_msgs::LaserScanConstPtr& message);
			
			void cmdCallback(const geometry_msgs::TwistConstPtr& cmd_msg);
			
			void goalCB(const geometry_msgs::PoseStamped::ConstPtr& goal);
			
			void aroundSafty();
			
		private:
			move_base_msgs::MoveBaseGoal current_goal;
			
			AgvState CurrentState, LastState;
			
			Direction agv_dir;
			
			sensor_msgs::LaserScan laser_msg;
			
			ros::Publisher action_goal_pub_;
			
	};
	
};



#endif



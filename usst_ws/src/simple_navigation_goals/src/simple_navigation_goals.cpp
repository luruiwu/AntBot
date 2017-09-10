#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/PoseStamped.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

enum STATE{
	NormalState,
	ObstacleState,
	
};
//动态障碍物出现的时候如何进行导航
class DynamicNavigation{
	
private:
	
	ros::NodeHandle n;
	ros::Subscriber laserscan_sub;
	ros::Subscriber goal_sub;
	move_base_msgs::MoveBaseGoal current_goal;
	STATE CurrentState, LastState;
   
public:
	DynamicNavigation(){
		
		//订阅激光雷达的数据
		laserscan_sub = n.subscribe("/scan", 1, &DynamicNavigation::laserScanCallback, this);
		
		//订阅当前目标
		ros::NodeHandle simple_nh("move_base_simple");
		goal_sub = simple_nh.subscribe<geometry_msgs::PoseStamped>("goal", 1, boost::bind(&DynamicNavigation::goalCB, this, _1));
		
		//创建actionlib客户端
		MoveBaseClient ac("move_base", true); 
		
		CurrentState = NormalState;
		LastState    = NormalState;
		ros::Rate loop_rate(5);
		
		//等待actionlib库服务器出现
		while(!ac.waitForServer(ros::Duration(5.0))){
			ROS_INFO("Waiting for the move_base action server to come up");
		}
		
		while(ros::ok()){
			
			//如果出现障碍物
			if((LastState == NormalState) && (CurrentState == ObstacleState))
			{
				//取消导航
				ROS_INFO("Cancle All Goals");
				ac.cancelAllGoals();
			}
			
			if((CurrentState == ObstacleState)){
				//暂停10s
				ROS_INFO("Wait for obstacle dispear");
				ros::Duration(10).sleep();
				
			}
			
			if((LastState == ObstacleState) && (CurrentState == NormalState)){
				
				ROS_INFO("Resend the goal");
				//重新发送目标点
				ac.sendGoal(current_goal);
				
			}
				
			
			ros::spinOnce();
			
			loop_rate.sleep();
		}
	}//DynamicNavigation
	
	//激光雷达数据的回调函数
	void laserScanCallback(const sensor_msgs::LaserScanConstPtr& message)
	{
		//ROS_INFO("laserScanCallback");
		
		//储存上一个状态
		LastState = CurrentState;
		
		//假设为正常状态
		CurrentState = NormalState;
		
		sensor_msgs::LaserScan laser_msg = *message;
		//printf("Size %ld\n",laser_msg.ranges.size());
		int size = laser_msg.ranges.size();
		for(size_t i = size/2 - 30 ; i <  size/2 + 30 ; i+=5){
			float range = laser_msg.ranges[i];
			if((range < 1.2) && (range > 0)){
				
				//当前状态为有障碍的状态
				CurrentState = ObstacleState;
				//ROS_INFO("Stop Flag");
			}
		}

	}
	
	//导航目标的回调函数
	void goalCB(const geometry_msgs::PoseStamped::ConstPtr& goal)
	{
		ROS_INFO("goalCB");
		
		current_goal.target_pose.header = goal->header;
		current_goal.target_pose.header.stamp = ros::Time::now();
		current_goal.target_pose.pose  = goal->pose;
		
	}
};

int main(int argc, char** argv)
{
    ros::init(argc, argv, "dynamic_navigation");

	DynamicNavigation dynamicObject;
	
    return 0;
}








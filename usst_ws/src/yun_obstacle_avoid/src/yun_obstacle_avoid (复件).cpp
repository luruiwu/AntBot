#include <yun_obstacle_avoid/yun_obstacle_avoid.h>


namespace dynamic_navigation{
	
	//动态障碍物出现的时候如何进行导航
	DynamicNavigation::DynamicNavigation()
	{
		ros::NodeHandle n;
		ros::Subscriber laserscan_sub;
		ros::Subscriber goal_sub;
		ros::Subscriber cmd_sub;
		/*
		ros::NodeHandle action_nh("move_base");
		action_goal_pub_ = action_nh.advertise<move_base_msgs::MoveBaseActionGoal>("goal", 1);
		*/
		
		//订阅激光雷达的数据
		laserscan_sub = n.subscribe("/scan", 1, &DynamicNavigation::laserScanCallback, this);
		
		cmd_sub = n.subscribe("/raw_cmd_vel", 1, &DynamicNavigation::cmdCallback, this);
		
		//订阅当前目标
		ros::NodeHandle simple_nh("move_base_simple");
		goal_sub = simple_nh.subscribe<geometry_msgs::PoseStamped>("goal", 1, boost::bind(&DynamicNavigation::goalCB, this, _1));
		
		CurrentState = NORMAL;
		LastState    = NORMAL;
		
		agv_dir = Forward;
		
		ros::Rate loop_rate(10);
		
		
		while(ros::ok()){
			
			//如果出现障碍物
			if((LastState == NORMAL) && (CurrentState == OBSTACLE))
			{
				//取消导航
				ROS_INFO("Cancle All Goals");
				ac.cancelAllGoals();
			}
			
			if((CurrentState == OBSTACLE)){
				//暂停10s
				ROS_INFO("Wait for obstacle dispear");
				ros::Duration(5).sleep();
			}
			
			if((LastState == OBSTACLE) && (CurrentState == NORMAL)){
				
				ROS_INFO("Resend the goal");
				//重新发送目标点
				ac.sendGoal(current_goal);
				ac.sendGoal(current_goal);
				ac.sendGoal(current_goal);
			}

			ros::spinOnce();
			
			loop_rate.sleep();
		}
	}//DynamicNavigation
	
	void DynamicNavigation::aroundSafty(){

	}
	
	//激光雷达数据的回调函数
	void DynamicNavigation::laserScanCallback(const sensor_msgs::LaserScanConstPtr& message)
	{
		laser_msg = *message;
		int size = laser_msg.ranges.size();
		int check_range = 180;
		float check_distance_1 = 1.5;
		float check_distance_2 = 0.6;
		
		//储存上一个状态
		LastState = CurrentState;
		
		//假设为正常状态
		CurrentState = NORMAL;
		
		
		//AGV的前面障碍物判断
		if(agv_dir == Forward){
			for(size_t i = size/2- check_range/2; i < size/2 + check_range/2 ; i+=5){
				float range = laser_msg.ranges[i];
				if((range < check_distance_1) && (range > 0)){
					ROS_INFO("distance_1");
					//当前状态为有障碍的状态
					CurrentState = OBSTACLE;
				}
			}
		}else{
			for(size_t i = 0; i <  check_range/2 ; i+=5){
				float range = laser_msg.ranges[i];
				if((range < check_distance_1) && (range > 0)){
					ROS_INFO("distance_1");
					//当前状态为有障碍的状态
					CurrentState = OBSTACLE;
				}
			}
			for(size_t i = size - check_range/2; i < size ; i+=5){
				float range = laser_msg.ranges[i];
				if((range < check_distance_1) && (range > 0)){
					//当前状态为有障碍的状态
					CurrentState = OBSTACLE;
					ROS_INFO("distance_1");
				}
			}
		}
		
		//检查周边的障碍物
		for(size_t i = check_range/2; i <  size/2 - check_range/2 ; i+=5){
			float range = laser_msg.ranges[i];
			if((range < check_distance_2) && (range > 0)){
				//当前状态为有障碍的状态
				CurrentState = OBSTACLE;
				ROS_INFO("distance_2");
			}
		}
		
		for(size_t i = size/2 + check_range/2; i < size - check_range/2 ; i+=5){
			float range = laser_msg.ranges[i];
			if((range < check_distance_2) && (range > 0)){
				//当前状态为有障碍的状态
				CurrentState = OBSTACLE;
				ROS_INFO("distance_2");
			}
		}
		

		
	}//laserScanCallback
	
	//导航目标的回调函数
	void DynamicNavigation::goalCB(const geometry_msgs::PoseStamped::ConstPtr& goal)
	{
		ROS_INFO("goalCB");
		
		current_goal.target_pose.header = goal->header;
		current_goal.target_pose.header.stamp = ros::Time::now();
		current_goal.target_pose.pose  = goal->pose;
		
	}//goalCB
	
	void DynamicNavigation::cmdCallback(const geometry_msgs::Twist::ConstPtr& cmd_msg)
	{
		//ROS_INFO("cmdCallback");
		agv_dir = (cmd_msg->linear.x > 0) ? (Forward) : (Backward);
	}
	
	DynamicNavigation::~DynamicNavigation(){
		
	}
	
};//namespace

int main(int argc, char** argv)
{
    ros::init(argc, argv, "dynamic_navigation_node");

	dynamic_navigation::DynamicNavigation dynamicObject;
	
    return 0;
}








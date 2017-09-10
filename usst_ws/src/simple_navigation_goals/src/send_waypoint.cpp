/*
 * 通过命令行发送储存在硬盘中的目标点
 * ./waypoint [num]
 * 2017_8_2 by Bob
 */

#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <vector>
#include <string>
#include <fstream> //infstream
#include <sstream> //stringstream
#include <iostream> //std::cout
#include <stdlib.h> //for atoi
#include <tf/transform_datatypes.h> // for tf::createQuaternionFromYaw
#include <boost/algorithm/string.hpp>  //for split

#define DEBUG false

const char* pointPath = "/home/pepper/usst_ws/src/yun_global_planner/script/waypoints.txt";

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

using namespace std;

int readWaypoints();

/*将字符串转化为数字*/
template<typename Type> Type stringToNum(const string&);

vector<float> float_vector;

int main(int argc, char** argv)
{
  ros::init(argc, argv, "simple_navigation_goals");
  
  int vector_size = 0;
  int num_point = 0;
  int current_point = -1;
  
  if(argc < 2){
    ROS_INFO("Usage ./send_waypoint [num]");
    return EXIT_FAILURE;
  }
  
  current_point = atoi(argv[1]);
  
  //读取点
  if(readWaypoints() == EXIT_SUCCESS){
    num_point = float_vector.size() / 2;
    if(num_point < 0) return EXIT_FAILURE;
    ROS_INFO("Read %d points", num_point);
  }
#if DEBUG
  	for(int i = 0 ; i < float_vector.size(); i+= 2)
	{
		std::cout << i/2 << "(x: "<< float_vector[i] << ", y: "<< float_vector[i+1] << ")" << std::endl;
	}
#endif

  if((current_point > num_point - 1) || (current_point < 0)){
    ROS_INFO("The waypoint is not in range[0-vector_size]");
  }else{
    ROS_INFO("Ready to send waypoint %d", current_point);
  }
  
  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  //wait for the action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  move_base_msgs::MoveBaseGoal goal;

  //we'll send a goal to the robot to move 1 meter forward
  goal.target_pose.header.frame_id = "map";
  goal.target_pose.header.stamp = ros::Time::now();
  
  tf::Quaternion goal_quat = tf::createQuaternionFromYaw(0);
  goal.target_pose.pose.position.x = float_vector[current_point*2];
  goal.target_pose.pose.position.y = float_vector[current_point*2 + 1];
  
  //std::cout << "goal:" << float_vector[3] << "," << float_vector[4] <<  std::endl;
  
  goal.target_pose.pose.orientation.x = goal_quat.x();
  goal.target_pose.pose.orientation.y = goal_quat.y();
  goal.target_pose.pose.orientation.z = goal_quat.z();
  goal.target_pose.pose.orientation.w = goal_quat.w();

  ROS_INFO("Sending goal");
  ac.sendGoal(goal);

  ROS_INFO("WaitFor Result");
  ac.waitForResult();

  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    ROS_INFO("The base move to the goal");
  else
    ROS_INFO("The base failed move to the goal for some reason");

  return 0;
}


int readWaypoints()
{
	vector<string> string_vector;
	std::ifstream infile;
	std::string str;
	
	//cout << "Open the waypoint.txt......" << endl;
	infile.open(pointPath);
	if(!infile.is_open()){
		std::cout << "Error opening file" << std::endl;
		return EXIT_FAILURE;
	}
	
	//填入string_vector容器
	while(getline(infile, str))
	{
		//P_0,P_1,P_3...
		if(str == "") //排除多余空行的干扰
			break;
		boost::algorithm::split(string_vector, str, boost::algorithm::is_any_of(";"));
		float_vector.push_back(stringToNum<float>(string_vector[1]));//x
		float_vector.push_back(stringToNum<float>(string_vector[2]));//y
	}
	infile.close();
	
	
	return EXIT_SUCCESS;
}


template <typename Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}
















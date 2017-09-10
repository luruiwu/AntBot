#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include <antbot_navigation/GoToAction.h>
#include <boost/thread.hpp>

void spinThread(){
  ros::spin();
}

int main(int argc, char **argv)
{
  // Set up ROS.
  ros::init(argc, argv, "path_planner_client");

  //create the action client
  actionlib::SimpleActionClient<antbot_navigation::GoToAction> ac("GoToAction");
  boost::thread spin_thread(&spinThread);

  ROS_INFO("Waitting for action server to start.");

  ac.waitForServer();

  ROS_INFO("Action server started, sending goal.");

  antbot_navigation::GoToGoal goal;
  ac.sendGoal(goal);

  bool finished_before_timeout = ac.waitForResult(ros::Duration(30.0));

  if (finished_before_timeout){
    actionlib::SimpleClientGoalState state = ac.getState();
    ROS_INFO("Action finished: %s",state.toString().c_str());
  }else{
      ROS_INFO("Action did not finished before the time out.");
  }

  ros::shutdown();

  spin_thread.join();

  return 0;


  //ROS_INFO("Hello world!");
}

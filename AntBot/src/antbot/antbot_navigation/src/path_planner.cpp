#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <actionlib/server/simple_action_server.h>
#include <antbot_navigation/GoToAction.h>
#include <geometry_msgs/Twist.h>

class GoToAction{

private:
    ros::Publisher vel_pub_;
    antbot_navigation::GoToGoal goto_goal;
    

public:
  GoToAction(std::string name):
      as_(nh_, name, false),
      action_name_(name){
      as_.registerGoalCallback(boost::bind(&GoToAction::goalCB, this));
      as_.registerPreemptCallback(boost::bind(&GoToAction::preemptCB, this));
      as_.start();
  }
  ~GoToAction(void){

  }

  void goalCB(){
    ROS_INFO("%s: goal", action_name_.c_str());
  }

  void preemptCB(){
    ROS_INFO("%s: Preempted", action_name_.c_str());
    as_.setPreempted();
  }
  
  void SetRobotSpeed(){
    private_node_handle_.param("cmd_topic_vel_", cmd_topic_vel_, std::string("cmd_vel"));
    //发布速度在控制命令话题上
    vel_pub_ = private_node_handle_.advertise<geometry_msgs::Twist>(cmd_topic_vel_, 1);

    if(GoToAction.isNewGoalAvailable()){
        goto_goal.target = GoToAction.acceptNewGoal()->target;
    }
    geometry_msgs::Twist cmd_msg;

    cmd_msg.linear.x     = 0.2;
    cmd_msg.angular.z  = 0.2;

    vel_pub_.publish(cmd_msg);

  }

protected:
  ros::NodeHandle nh_;
  ros::NodeHandle private_node_handle_;
  actionlib::SimpleActionServer<antbot_navigation::GoToAction> as_;
  std::string action_name_;
  antbot_navigation::GoToFeedback  feedback_;
  antbot_navigation::GoToResult      result_;
  
};


int main(int argc, char ** argv){

  ros::init(argc, argv, "GoToAction");

  GoToAction GoTo(ros::this_node::getName());

  ros::spin();

  return 0;

}

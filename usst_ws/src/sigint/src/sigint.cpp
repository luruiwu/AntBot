#include <ros/ros.h>
#include <signal.h>
#include <ostream>

void mySigintHandler(int sig)
{
  //do something ,publish a stop message 
  std::cout << "SIGINT" << std::endl;

  std::cout << "OVER" << std::endl;
 
  ros::shutdown();

}

int main(int argc, char* argv[])
{
  ros::init(argc, argv, "my_node_name", ros::init_options::NoSigintHandler);
  ros::NodeHandle nh;

  /*
   * overried the default ros sigint handler.
   * This must be set after the first NodeHandle is created.
   */
  signal(SIGINT, mySigintHandler);

  ros::spin();
  
  return 0;
}

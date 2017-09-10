/*
 * The following pseudocode algorithm simulates the effect of a low-pass filter on a series of digital samples:

 // Return RC low-pass filter output samples, given input samples,
 // time interval dt, and time constant RC
 function lowpass(real[0..n] x, real dt, real RC)
   var real[0..n] y
   var real α := dt / (RC + dt)
   y[0] := α * x[0]
   for i from 1 to n
       y[i] := α * x[i] + (1-α) * y[i-1]
   return y

The loop that calculates each of the n outputs can be refactored into the equivalent:

   for i from 1 to n
       y[i] := y[i-1] + α * (x[i] - y[i-1])

That is, the change from one filter output to the next is proportional to the difference between the previous output and the next input. 
* This exponential smoothing property matches the exponential decay seen in the continuous-time system. As expected, as the time constant R C 
* {\displaystyle \scriptstyle RC} \scriptstyle RC increases, the discrete-time smoothing parameter α {\displaystyle \scriptstyle \alpha } 
* \scriptstyle \alpha decreases, and the output samples ( y 1 , y 2 , … , y n ) {\displaystyle \scriptstyle (y_{1},\,y_{2},\,\ldots 
* ,\,y_{n})} \scriptstyle (y_1,\, y_2,\, \ldots,\, y_n) respond more slowly to a change in the input samples ( x 1 , x 2 , … , x n ) 
* {\displaystyle \scriptstyle (x_{1},\,x_{2},\,\ldots ,\,x_{n})} \scriptstyle (x_1,\, x_2,\, \ldots,\, x_n); the system has more inertia. 
* This filter is an infinite-impulse-response (IIR) single-pole low-pass filter.
 * 
 * 
 */

#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "actionlib_msgs/GoalStatusArray.h"
#include <std_msgs/UInt8.h>

double T = 0.1; //sec
double dt = 0.4; //ms 0.1

double x_ = 0;
double y_ = 0;
double z_ = 0;
double epsilon = 0.0001;
double epsilon_2 = 0.0005;

int count = 0;

std_msgs::UInt8 obstacle_msg;

ros::Publisher twistPublisher;
ros::Subscriber movebaseSubscriber;
ros::Subscriber obstacleSubscriber;

double lowPassFilter(double x, double y0, double dt, double T)          // Taken from http://en.wikipedia.org/wiki/Low-pass_filter
{
  double res;
  //进入紧急刹车模式
  if(obstacle_msg.data == 1){
     res = y0 + (x - y0) * (19.0/20);
     if ((res*res) <= epsilon_2){
       res = 0;
     }
  }else{
       res = y0 + (x - y0) * (dt/(dt + T));
       if ((res*res) <= epsilon)
         res = 0;
  }
   return res;
}

void twistCallback(const geometry_msgs::Twist& twist){
   double x =  twist.linear.x;
   double y = twist.linear.y;
   double z = twist.angular.z;

	x_ = lowPassFilter(x, x_, dt, T);
	y_ = lowPassFilter(y, y_, dt, T);
	z_ = lowPassFilter(z, z_, dt, T);

	geometry_msgs::Twist t;
	t.linear.x = x_;
	t.linear.y = y_;
	t.angular.z = z_;

	twistPublisher.publish(t);

}
void statusCallback(const actionlib_msgs::GoalStatusArray::ConstPtr& msg){
	if(msg->status_list.size() == 0)
		return;
	if(msg->status_list[0].status == 3){
		count++;
	}else if(msg->status_list[0].status == 4){
		count++;
	}else{
		count = 0;
	}
}

void obstacleCallback(const std_msgs::UInt8::ConstPtr& msg){
  obstacle_msg.data = msg->data;
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "lowpass_filter");
	ros::NodeHandle n;
    /* setup input/output communication */

  n.param("T", T, 0.3);
	n.param("dt", dt, 0.1);

  obstacle_msg.data = 0;
	
	ros::Subscriber status_sub = n.subscribe<actionlib_msgs::GoalStatusArray>("/move_base/status", 1, &statusCallback);
	ros::Subscriber twistSubscriber = n.subscribe("/raw_cmd_vel", 1, &twistCallback);
  ros::Subscriber obstacleSubscriber = n.subscribe("/obstacle", 1, &obstacleCallback);
	twistPublisher = n.advertise<geometry_msgs::Twist>("smooth_cmd_vel",1);

	/* coordination */
	ros::Rate rate(50); //Input and output at the same time... (in Hz)
	while (n.ok()){
		
		//只发送一次的时候才发送停止速度。
		if(count == 1){
			geometry_msgs::Twist stop_speed;
			twistPublisher.publish(stop_speed);
		}
		
		ros::spinOnce();
		rate.sleep();
	}

  return 0;
}



#include <yun_obstacle_avoid/yun_obstacle_avoid.h>


namespace dynamic_navigation{
	
  //动态障碍物出现的时候如何进行导航
  DynamicNavigation::DynamicNavigation()
  {
    ros::NodeHandle n;
    ros::Subscriber laserscan_sub;
    ros::Subscriber goal_sub;
    ros::Subscriber cmd_sub;
    ros::Subscriber move_base_sub;
    ros::Publisher cmd_vel_pub;
    ros::Publisher obstacle_pub;

    move_base_status = 0;

    inc = 1;

    //有障碍物体
    obstacle_pub = n.advertise<std_msgs::UInt8>("/obstacle",1);

    //订阅激光雷达的数据
    laserscan_sub = n.subscribe("/scan", 1, &DynamicNavigation::laserScanCallback, this);

    cmd_sub = n.subscribe("/smooth_cmd_vel", 1, &DynamicNavigation::cmdCallback, this);

    goal_sub = n.subscribe<move_base_msgs::MoveBaseActionGoal>("/move_base/goal", 1, boost::bind(&DynamicNavigation::goalCB, this, _1));

    move_base_sub = n.subscribe<actionlib_msgs::GoalStatusArray>("/move_base/status", 1, boost::bind(&DynamicNavigation::move_base_statusCallback, this, _1));

    CurrentState = NORMAL;
    LastState    = NORMAL;

    agv_dir = Forward;

    //根据激光数据的频率
    ros::Rate loop_rate(10);

    //创建actionlib客户端
    MoveBaseClient ac("move_base", true);

    printf("wait server\n");

    //等待actionlib库服务器出现
    while(!ac.waitForServer(ros::Duration(5.0))){
      ROS_INFO("Waiting for the move_base action server to come up");
    }

    //调用service ，清除原来位置的动态障碍层
    ros::ServiceClient client = n.serviceClient<std_srvs::Empty>("/move_base/clear_costmaps");
    std_srvs::Empty srv;

    geometry_msgs::Twist twist;

    check_distance_1 = LONG_CHECK_RANGE;
    check_distance_2 = SIDE_CHECK_RANGE;

    printf("wait server over \n");
    while(ros::ok()){

      //printf("move_base_status:%d\n",move_base_status);
      //只有当机器人在运行的时候，才启动检测
      if(((move_base_status==1) || (move_base_status==2))){
          //check obstacle
          aroundSafty();

          //如果出现障碍物
          if((LastState == NORMAL) && (CurrentState == OBSTACLE))
          {
            //进入刹车模式
            obstacle_msg.data = 1;
            obstacle_pub.publish(obstacle_msg);

            //取消导航
            ROS_INFO("Cancle All Goals");
            ac.cancelAllGoals();
          }

          //障碍物状态
          if((CurrentState == OBSTACLE)){
            //暂停3s
            ROS_INFO("Wait for obstacle dispear");
            ros::Duration(1).sleep();
          }

          //恢复正常状态
          if((LastState == OBSTACLE) && (CurrentState == NORMAL)){
            //进入刹车模式
            obstacle_msg.data = 0;
            obstacle_pub.publish(obstacle_msg);

            if(client.call(srv)){
              ROS_INFO("\nClear costmap succeed!");
            }else{
              ROS_ERROR("\nFailed to call service /move_base/clear_costmaps");
              return;
            }

            //清理之后不能马上进行导航，规划路径会出错
            ros::Duration(2).sleep();

            ROS_INFO("Resend the goal");
            //重新发送目标点
            ac.sendGoal(current_goal);
          }
      }

      if(move_base_status == 3){
        //获取机器人的位置
        goal_x = transform.getOrigin().x();
        goal_y = transform.getOrigin().y();
      }

      /****************************Read TF*****************************/

      try{

        listener.waitForTransform("/map", "/base_link",
                           ros::Time(0), ros::Duration(3.0));
        listener.lookupTransform("/map",
                                                  "/base_link",
                                                  ros::Time(0),
                                                  transform);
      }catch (tf::TransformException &ex){
        ROS_ERROR("%s", ex.what());
        ros::Duration(1.0).sleep();
        continue;
      }

      //获取机器人的位置
      robot_x = transform.getOrigin().x();
      robot_y = transform.getOrigin().y();

      //计算机器人距离目标的位置的距离
      //靠近目标
      dis_1 = hypot((robot_x - current_goal.target_pose.pose.position.x),  (robot_y - current_goal.target_pose.pose.position.y));

      //离开之前的目标
      dis_2 = hypot((robot_x - goal_x),  (robot_y - goal_y));

      //当靠近目标的时候，因为货架离机器人太近，因此减少检测距离
      if((dis_1 < 1.0) || (dis_2 < 1.0)){
        check_distance_1 = SHORT_CHECK_RANGE;
      }else{
        check_distance_1 = LONG_CHECK_RANGE;
      }

      ros::spinOnce();

      loop_rate.sleep();
    }
  }//DynamicNavigation

  void DynamicNavigation::aroundSafty()
  {
    int size = laser_msg.ranges.size();
    int check_range = 180;
    if(size < 10) return;
    //printf("Laser range size :%d\n",size); //1440

    //储存上一个状态
    LastState = CurrentState;

    //假设为正常状态
    CurrentState = NORMAL;

     if((agv_dir & Forward) != 0){
        /***AGV的前面障碍物判断****/
        for(size_t i = size/2- check_range/2; i < size/2 + check_range/2 ; i+=inc)
        {
          if(( laser_msg.ranges[i] < check_distance_1) && ( laser_msg.ranges[i] > 0)){
            //当前状态为有障碍的状态
            CurrentState = OBSTACLE;
          }
        }
     }

      /***AGV的后面障碍物判断****/
     if((agv_dir & Backward) != 0){
        for(size_t i = 0; i <  check_range/2 ; i+=inc)
        {
          if(( laser_msg.ranges[i] < check_distance_1) && ( laser_msg.ranges[i] > 0))
          {
            //当前状态为有障碍的状态
            CurrentState = OBSTACLE;
          }
        }
        for(size_t i = size - check_range/2; i < size ; i+=inc)
        {
          if(( laser_msg.ranges[i] < check_distance_1) && ( laser_msg.ranges[i] > 0))
          {
            //当前状态为有障碍的状态
            CurrentState = OBSTACLE;
          }
        }
     }
      /****检查两边的障碍物***/
      for(size_t i = check_range/2; i <  size/2 - check_range/2 ; i+=inc)
      {
        if(( laser_msg.ranges[i] < check_distance_2) && ( laser_msg.ranges[i] > 0))
        {
          //当前状态为有障碍的状态
          CurrentState = OBSTACLE;
        }
      }

      for(size_t i = size/2 + check_range/2; i < size - check_range/2 ; i+=inc)
      {
        if(( laser_msg.ranges[i] < check_distance_2) && ( laser_msg.ranges[i] > 0))
        {
          //当前状态为有障碍的状态
          CurrentState = OBSTACLE;
        }
      }

      if(CurrentState == OBSTACLE)
    {
      ROS_INFO("distance_1");
    }
}

  //激光雷达数据的回调函数
  void DynamicNavigation::laserScanCallback(const sensor_msgs::LaserScanConstPtr& message)
  {
    laser_msg = *message;
  }//laserScanCallback

  //导航目标的回调函数
  void DynamicNavigation::goalCB(const move_base_msgs::MoveBaseActionGoalConstPtr& goal)
  {
    ROS_INFO("goalCB");

    current_goal.target_pose = goal->goal.target_pose;
    current_goal.target_pose.header.stamp = ros::Time::now();

  }//goalCB

  void DynamicNavigation::cmdCallback(const geometry_msgs::TwistConstPtr& cmd_msg)
  {
    //ROS_INFO("cmdCallback");
    if(cmd_msg->linear.x > 0)
        agv_dir = Forward;
    else if(cmd_msg->linear.x < 0)
      agv_dir = Backward;
    else
      agv_dir = Both;
  }

  DynamicNavigation::~DynamicNavigation(){

  }

  void dynamic_navigation::DynamicNavigation::move_base_statusCallback(const actionlib_msgs::GoalStatusArray::ConstPtr &msg)
  {
    //如果没有状态，则返回
    if(msg->status_list.size() == 0)
      return;

    //获取move base的状态
    move_base_status = msg->status_list[0].status;
  }


};//namespace

int main(int argc, char** argv)
{
    ros::init(argc, argv, "dynamic_navigation_node");

    dynamic_navigation::DynamicNavigation dynamicObject;
	
    return 0;
}








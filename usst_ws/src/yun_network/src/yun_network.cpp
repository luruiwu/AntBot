#include "yun_network/yun_network.h"


Network::Network(unsigned short port_num, string addr):sock_client(service, udp::endpoint(udp::v4(), 20000)),
                                                                                      ac("move_base", true)
{
  
  //设置ip和端口号
  receiver_endpoint.port(port_num);
  receiver_endpoint.address(ip::address::from_string(addr));

  //以ipv4协议打开套接字
 //sock_client.open(udp::endpoint(udp::v4(), SOCK_PORT));

  move_base_status = 0;
  server_state = Server_Ready_State;
  Current_Goal = -2;
  io_ctl_msg.led = LED_Off;
  io_ctl_msg.fa = Fa_Up;
  io_ctl_msg.charge = Charge_Default_State;
  agv_state = AGV_Ready_Navigation;
  last_agv_status = agv_state;
  battery_msg.data = 100;
  NEW_GOAL_FLAG = false;
  camera_locate_state = Camera_Default_State;
  REST_POSITION = false;
  NEED_PRECISE_LOCATE = true;
  STEP = 5;
  charge_state = Charge_Default_State;
  ReSent = false;

  /*服务器和导航点之间维护一个映射的表格*/
  memset(pose2goal, -1, sizeof(pose2goal));
  memset(goal2pose, -1, sizeof(goal2pose));//memset 以字节为单位
  for(int i = 0; i < MAX_GOAL_NUM; i++)
    cout << "pose2goal:" << pose2goal[i] << endl;

  //pose to goal map
  pose2goal[1] = REST_POS;
  pose2goal[2] = CHARGE_POS;
  pose2goal[3] = MACHINE_POS_3;
  pose2goal[4] = MACHINE_POS_4;
  pose2goal[5] = MATERIAL_POS;

  //goal to pose map
  for(int i = 0; i < MAX_GOAL_NUM; i++){
    if(pose2goal[i] != -1){
      goal2pose[pose2goal[i]] = i;
      //cout << "i:" << i << endl;
    }
  }

  //读取位点
  readWaypoints();

  for(int i = 0; i < float_vector.size()/2.0; i++){
    goal[i].x = float_vector[i * 2];
    goal[i].y = float_vector[i * 2 + 1];
    goal[i].angle = 0.0;
    goal[i].goal_state = Goal_Not_Send;
    goal[i].charge_pos = false;
    goal[i].rest_pos = false;
    goal[i].reverse = true;
    goal[i].need_precise_locate = true;/*set false for debug*/
    std::cout <<"["<< i << "]"<<  "x:" << goal[i].x << ", y:" <<  goal[i].y <<",angle:"  << goal[i].angle << std::endl;
  }
  //休息点不用精确定位
  goal[REST_POS].need_precise_locate = false;

  goal[CHARGE_POS].angle = M_PI_2;
  goal[CHARGE_POS].charge_pos = true;

  goal[MATERIAL_POS].angle = M_PI;
  goal[MATERIAL_POS].reverse = false;

  NUM_WAYPOINTS = float_vector.size()/2.0;

  std::cout << "#Read waypoints finished !" << std::endl;


  /*
   * 订阅者声明的几种形式ros::Subscriber status_sub = n.subscribe("/move_base/status", 1000, statusCallback);
   * 使用boost::bind的时候一定要用n.subscribe<消息类型>("", 1, boost:bind());
   * boost::bind()绑定成员函数，参数：(函数，对象，绑定函数的参数)
   */

  /*订阅电量的信息*/
  battery_sub = n.subscribe<std_msgs::UInt8>("/battery", 1, boost::bind(&Network::batteryCallback, this, _1));

  /*订阅/move_base的状态信息*/
  status_sub = n.subscribe<actionlib_msgs::GoalStatusArray>("/move_base/status", 1, boost::bind(&Network::move_base_statusCallback, this, _1));
  cout << "#subscribe to /move_base/status..." << endl;

  //发布state
  state_pub = n.advertise<yun_bringup::IO_Ctl>("/state", 1);
  cout << "#publish to /state" << endl;
  
  //发布初始位姿
  initialpose_pub = n.advertise<geometry_msgs::PoseWithCovarianceStamped>("/initialpose", 1);
  cout << "#publish to /initialpose" << endl;

  //等待建立发布接受连接
  sleep(3);
  
  //发布初始位置姿态
  setInitPose();
  cout << "#setInitPose" << endl;

}

std::vector<unsigned char> Network::hex2bytes(const std::string& hex)
{
  std::vector<unsigned char> bytes;

  for(unsigned int i = 0; i < hex.length(); i += 2){

    //substr(n, m)取从n开始的m位字符
    std::string byteString = hex.substr(i, 2);

    //strtol将字符串根据base转换为整形数据
    unsigned char byte = (unsigned char)strtol(byteString.c_str(), NULL, 16);

    bytes.push_back(byte);
  }
  return bytes;
}

void Network::wait(int seconds)
{
  boost::this_thread::sleep(boost::posix_time::seconds(seconds));
}

int Network::setInitPose()
{
	
	/*
	 * 初始位置由程序读取出来，AGV程序不停的储存当前的位置姿态到硬盘，直到导航程序关闭，
	 * 当导航程序重启之后就会读取导航程序储存的最后一次的位置姿态作为初始的位置姿态。
	 */
	geometry_msgs::PoseWithCovarianceStamped initpose_msg;
	initpose_msg.header.frame_id = "map";
	initpose_msg.header.stamp = ros::Time::now();
	
	//读取初始的位置姿态
	std::ifstream infile(json_path, std::ios::in);
	if(!infile.is_open()){
		std::cout << "\nError opening json_path file" << std::endl;
    exit(EXIT_FAILURE);
	}
	
	if(pose_reader.parse(infile, pose_read_root)){
		initpose_msg.pose.pose.position.x = pose_read_root["position_x"].asFloat();//1.6
		initpose_msg.pose.pose.position.y = pose_read_root["position_y"].asFloat();//-6.4
		initpose_msg.pose.pose.orientation.x = pose_read_root["orientation_x"].asFloat();
		initpose_msg.pose.pose.orientation.y = pose_read_root["orientation_y"].asFloat();
		initpose_msg.pose.pose.orientation.z = pose_read_root["orientation_z"].asFloat();
		initpose_msg.pose.pose.orientation.w = pose_read_root["orientation_w"].asFloat();
	}
	
	infile.close();
	
	/*
	 * 因为ros话题原理本身的问题，Setting pose 需要按照以下发送
	 */
	initialpose_pub.publish(initpose_msg);
	
	//调用service ，清除原来位置的动态障碍层
	ros::ServiceClient client = n.serviceClient<std_srvs::Empty>("/move_base/clear_costmaps");
	std_srvs::Empty srv;
	
	//等待创建连接
	ros::Duration(2).sleep();
	if(client.call(srv)){
		ROS_INFO("\nClear costmap succeed!");
	}else{
		ROS_ERROR("\nFailed to call service /move_base/clear_costmaps");
		return 1;
	}
	
	//清理之后不能马上进行导航，规划路径会出错
	ros::Duration(3).sleep();
	
	return 0;

}

void Network::readSocket()
{
  #if DEBUG
  cout << "#Create a thread read socket" << endl;
  #endif

  size_t len = 0;
  char temp[4];
  int ret_len;
  int head_len;
  int base_len;
  const char* pjson_str;

  //等待actionlib库服务器出现
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("\nWaiting for the move_base action server to come up");
  }

  /*数据解析参考：智能工厂系统-A2接口规范-AGV.docx*/
  while(ros::ok()){

    len = sock_client.receive_from(buffer(recv_buf), sender_point);
    //cout << "Read data len: "<< (int)len << endl;
    if(len == 2)
    {
      //收到心跳应答
      if((recv_buf[0] == 0xD0) && (recv_buf[1] == 0x00)){
        #if DEBUG
        cout << "[HeartbeatEcho]" << endl;
        #endif
        IsHeartbeatEcho = true;
      }
    }else if(len == 4) {
      //收到设备状态应答
      if((recv_buf[0] == 0x40) && (recv_buf[1] == 0x02)&& (recv_buf[2] == 0x00)&& (recv_buf[3] == 0x01))
      {
        #if DEBUG
        cout << "[DeviceStateEcho]" << endl;
        #endif
        DeviceStateEcho = true;
      }
      //收到CCS连接应答
      else if((recv_buf[0] == 0x20) && (recv_buf[1] == 0x02) && (recv_buf[2] == 0x00)&& (recv_buf[3] == 0x00)){
        #if DEBUG
        cout << "[CCSEcho]" << endl;
        #endif
        IsCCSEcho = true;
      }
      //收到设备命令(至少10个字节)
    }else if(len > 10){
      if(recv_buf[0] == 0x32){
        //设备命令
        #if DEBUG
        cout << "[DeviceCmd]" << endl;
        #endif
        IsDeviceCmd = true;

        //将包含剩余的字节数的编码结果的数据拷贝出来
        memcpy(temp, recv_buf+1, 4);

        //解码
        ret_len = decoder(temp);

        //报文头部数据长度
        head_len = len - ret_len;

        //报文基础数据部分长度在AGV1-9之内都是8个字节
        base_len = 19;

        //指针指向json字符流
        pjson_str = (char*)(recv_buf + head_len + base_len + 2);

        //用数组指针构造字符流
        string json_str(pjson_str);

        #if DEBUG
        //打印json数据流
        cout << "json_str: "<< json_str << endl;
        #endif

        if(reader.parse(json_str, read_root))
        {
          //动作非空
          string action_str = read_root["action"].asString();
          if(action_str == "go"){
            if(!read_root["position"].isNull()){

              //获取AGV的目标位置
              int g =  read_root["position"].asInt();

              //将位置装换为对应的导航点
              Current_Goal = pose2goal[g];/********************************************************************/

              //如果位点是否合理
              if(Current_Goal < -1 || (Current_Goal >= NUM_WAYPOINTS)){
                continue;
              }

              //是否为最新的目标
              NEW_GOAL_FLAG = true;
              goal[Current_Goal].goal_state = Goal_Not_Send;
              cout << "------>Go to position: " << Current_Goal << endl;
            }
          //充电指令
          }else if(action_str == "charge"){
            charge_state = Charge_Start;
            NEW_GOAL_FLAG = true;
          //结束充电指令
          }else if(action_str == "finish_charge"){
            charge_state = Charge_Finish;
          }
        }//if
      }//if
    }//else if
  }//while
}

void Network::loop()
{
  cout << "#loop" << endl;
  int i = 0;

  IsCCSEcho = false;
  IsHeartbeatEcho = false;
  IsDeviceCmd = false;

  ros::Rate loop_rate(5);

  //负责向socket发送数据
  while(ros::ok()){

    /************************Send CCS *************************/
    if(!IsCCSEcho){
      //发送CCS
      send_buf = hex2bytes(CCS);
      sock_client.send_to(buffer(send_buf), receiver_endpoint);
    }

    /***********************Send Heart Beat 5HZ********************/
    if(i == 5){
      i = 0;
      //发送Heartbeat
      send_buf = hex2bytes(Heartbeat);
      sock_client.send_to(buffer(send_buf), receiver_endpoint);
    }

    /************************Send Device Cmd Echo***************/
    if(IsDeviceCmd){
      //发送DeviceCmdEcho
      send_buf = hex2bytes(DeviceCmdEcho);
      sock_client.send_to(buffer(send_buf), receiver_endpoint);
    }

    /***************发送设备状态信息*************************/
    //服务器信息
    if(STEP == 5)
      server_state = Server_Ready_State;
    else if(STEP == 7)
      server_state == Server_AGV_Error_State;
    else if(STEP == 6)
      server_state = Server_Charge_State;
    else
      server_state == Server_Moving_State;

    write_root["AGVState"]["AGVID"] = "AGV1";
    write_root["AGVState"]["state"] = server_state;
    write_root["AGVState"]["power"] = battery_msg.data;
    write_root["AGVState"]["x"] =  transform.getOrigin().x();
    write_root["AGVState"]["y"] =  transform.getOrigin().y();
    if(goal[Current_Goal].goal_state == Goal_Reached)
      write_root["AGVState"]["position"] = goal2pose[Current_Goal];
    else
      write_root["AGVState"]["position"] = -1;
    string json_str = write_root.toStyledString();
    //cout << "json_str: " << endl << json_str << endl;

    int json_size = json_str.length();
    char json_chr[json_size];
    strcpy(json_chr, json_str.data());

    //clear vector
    send_buf.clear();
    //cout  << "send_buf size: " << send_buf.size() << endl;
    //cout  << "send_buf capacity: " << send_buf.capacity() << endl;

    //coder
    int base_len = 8;
    int extra_len = json_size + 2;
    int coder_len = base_len + extra_len;
    char coder_chr[MAX_CODER_STR_LENGTH] = {0};
    coder(coder_len, coder_chr);

    //head data
    send_buf.push_back(0x32);
    int j = 0;
    while((coder_chr[j] != '\0') && (j < sizeof(coder_chr))){
      send_buf.push_back(coder_chr[j++]);
    }
    //base data
    send_buf.push_back(0x00);
    send_buf.push_back(0x04);
    send_buf.push_back(0x41);
    send_buf.push_back(0x47);
    send_buf.push_back(0x56);
    send_buf.push_back(0x31);
    send_buf.push_back(0x00);
    send_buf.push_back(0x01);

    //extra data
    send_buf.push_back((char)((json_size >> 8) & 0xff));
    send_buf.push_back((char)((json_size) & 0xff));
    for(int j = 0; j < json_size; j++)
      send_buf.push_back(json_chr[j]);

    //发送设备状态信息1次/s
    sock_client.send_to(buffer(send_buf), receiver_endpoint);

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

    /*******************************Send Goal*******************************************/
     if((NEW_GOAL_FLAG)  && (STEP == 5)){

       if( charge_state == Charge_Start){
         Current_Goal = CHARGE_POS;
       }
       /****************发送目标位置*******************/
       tf::Quaternion goal_quat = tf::createQuaternionFromYaw(goal[Current_Goal].angle);
       current_goal.target_pose.header.stamp = ros::Time::now();
       current_goal.target_pose.header.frame_id = "/map";
       current_goal.target_pose.pose.position.x = goal[Current_Goal].x;
       current_goal.target_pose.pose.position.y = goal[Current_Goal].y;
       current_goal.target_pose.pose.orientation.x = goal_quat.x();
       current_goal.target_pose.pose.orientation.y = goal_quat.y();
       current_goal.target_pose.pose.orientation.z = goal_quat.z();
       current_goal.target_pose.pose.orientation.w = goal_quat.w();
       ac.sendGoal(current_goal);

       NEW_GOAL_FLAG = false;
       goal[Current_Goal].goal_state = Goal_Sent;
       cout << "Goal_Sent" << endl;
    }

    ros::spinOnce();

    //5HZ
    loop_rate.sleep();

    ++i;
  }

}

void Network::coder(int len, char buf[])
{
  int x = 0, d = 0, i = 0;
  x = len;

  do{
    //求余数
    d = x % 0x80;

    //求倍数
    x = x / 0x80;

    //如果有进位，那么将每个字节的最高位设置为1
    if(x > 0){
       d = d | 0x80;
    }

    //储存
    buf[i] = d;
    i++;
  }while(x > 0);
}

int Network::decoder(char buf[])
{
  int m = 1;
  int len = 0;
  int d = 0;
  int i = 0;

  do{
    //读取字符
    d = buf[i];

    //计算长度
    len += (d &0x7F) *m;

    m *= 0x80;
    i++;
  }while((d & 0x80) != 0);

  return len;
}

void Network::shutDown()
{
  /*
   * 保存机器人最后的位置姿态
   */
  pose_write_root["position_x"] = transform.getOrigin().x();
  pose_write_root["position_y"] = transform.getOrigin().y();
  pose_write_root["orientation_x"] = transform.getRotation().x();
  pose_write_root["orientation_y"] = transform.getRotation().y();
  pose_write_root["orientation_z"] = transform.getRotation().z();
  pose_write_root["orientation_w"] = transform.getRotation().w();

  string json_str = pose_write_root.toStyledString();
  cout << json_str << endl;

  std::ofstream outfile(json_path, std::ios::out);

  if(!outfile.is_open()){
    std::cerr << "open file failed !"  << std::endl;
    exit(EXIT_FAILURE);
  }

  //输出到文件
  outfile << json_str;

  outfile.close();

  cout << "#Save Current Position" << endl;

  //关闭socket
  sock_client.close();

  //关闭ROS
  ros::shutdown();
}

int Network::readWaypoints()
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

/*
 * 用于发布消息摄像头定位节点
 * 开始摄像头定位：1
 * 开始返航：2
 * 充电：3
 * ERROR：4
 * 充电结束返回：5
 */
void Network::cameraLocate()
{
  ros::Publisher  camera_pub = n.advertise<std_msgs::UInt8>("/RD_state", 1);
  cout << "#publish /RD_state..." << endl;

  //用于接收摄像头定位程序通信
  ros::Subscriber camera_sub = n.subscribe<std_msgs::UInt8>("/camera_state", 1, boost::bind(&Network::cameraCallback, this, _1));
  cout << "#subscribe to /camera_state..." << endl;

  std_msgs::UInt8 camera_msg;
  camera_msg.data = 0;

  //等待订阅创建好连接/*move_base status
  sleep(3);

  ros::Rate loop_rate(10);

  while(ros::ok()){

    cout << "STEP : "<< STEP << endl;

    /*********************************状态切换*******************************/
    switch(STEP){
      case 0:
        //正在导航
        if((move_base_status == Move_Base_Goal_Accepted))
        {
          goal[Current_Goal].goal_state = Goal_Pursuit;
          cout << " Goal_Pursuit " << endl;
          STEP = 1;
        }
        break;
      case 1:
        //初步定位目标到达
        if((move_base_status == Move_Base_Goal_Reached))
        {
          if(goal[Current_Goal].need_precise_locate){
            goal[Current_Goal].goal_state = Goal_Roughly_Reached;
            cout << "Goal_Roughly_Locate " << endl;

            io_ctl_msg.fa = Fa_Default_State;
            io_ctl_msg.led = LED_On;
            io_ctl_msg.charge = Charge_Default_State;
            state_pub.publish(io_ctl_msg);

            if(charge_state == Charge_Start){
              //充电定位
              camera_msg.data = Precise_Locate_Charge_Pos;
            }else{
              if(goal[Current_Goal].reverse)
                camera_msg.data = Precise_Start_Locate_Reverse;
              else
                camera_msg.data = Precise_Start_Locate;
            }

            camera_pub.publish(camera_msg);

            STEP = 2;
          }else{
            goal[Current_Goal].goal_state = Goal_Reached;
            cout << "Goal_Reached " << endl;
            STEP = 5;
          }
        }else if(move_base_status == Move_Base_Goal_ABORTED){
          //如果震荡，重新发送当前目标
          //ReSent = true;
          STEP = 2;
        }
        break;
      case 2:
        //精确定位结束
        if((camera_locate_state == Camera_Loate_Finished))
        {
          goal[Current_Goal].goal_state = Goal_Reached;
          cout << "Goal_Reached " << endl;

          io_ctl_msg.fa = Fa_Down;
          io_ctl_msg.led = LED_Off;
          io_ctl_msg.charge = Charge_Default_State;
          state_pub.publish(io_ctl_msg);

          STEP = 3;
        }
        //充电定位结束
        else if((camera_locate_state == Camera_Locate_Charge_Pos_Finished)){
          charge_state = Charge_Default_State;
          goal[Current_Goal].goal_state = Goal_Reached;

          io_ctl_msg.fa = Fa_Down;
          io_ctl_msg.led = LED_Off;
          io_ctl_msg.charge = Charge_Start;
          state_pub.publish(io_ctl_msg);

          STEP = 6;
        }
        break;
      case 3:
        //新目标，开始精确定位返回
        if((NEW_GOAL_FLAG))
        {
          io_ctl_msg.fa = Fa_Up;
          io_ctl_msg.led = LED_Default_State;
          io_ctl_msg.charge = Charge_Default_State;
          state_pub.publish(io_ctl_msg);

          //延时一秒，等待阀上去
          sleep(1);
          if(goal[Current_Goal].reverse)
            camera_msg.data = Precise_Start_Return_Reverse;
          else
            camera_msg.data = Precise_Start_Return;
          camera_pub.publish(camera_msg);

          cout << "NEW_GOAL_FLAG " << endl;

          STEP = 4;
        }
        break;
      case 4:
        //精确定位返回结束
        if(camera_locate_state == Camera_Return_Finished)
        {
          goal[Current_Goal].goal_state = Goal_Precise_Locate_Return;
          cout << "Goal_Presice_Locate_Return " << endl;
          STEP = 5;
        }
        //充电返回结束
        else if(camera_locate_state == Camera_Charge_Return_Finished){
          charge_state = Charge_Default_State;
          STEP = 5;
        }
        break;
      case 5:
        //发送了一个目标
        if(goal[Current_Goal].goal_state == Goal_Sent){
            STEP = 0;
        }
        break;
      case 6:
        //充电结束
        if(charge_state == Charge_Finish){
          io_ctl_msg.fa = Fa_Up;
          io_ctl_msg.led = LED_Default_State;
          io_ctl_msg.charge = Charge_Finish;
          state_pub.publish(io_ctl_msg);

          //延时一秒，等待阀上去
          sleep(1);
          camera_msg.data = Precise_Charge_Return;
          camera_pub.publish(camera_msg);

          STEP = 4;
        }
        break;
      case 7:
        //出现错误
        cout << "AGV ERROR" << endl;
        if(0){
          break;
        }
    }//switch

    ros::spinOnce();

    loop_rate.sleep();
  }//while
}

//将string转换为数字类型
template <typename Type>
Type Network::stringToNum(const string &str)
{
  istringstream iss(str);
  Type num;
  iss >> num;
  return num;
}

/*
 * move_base_status
 */
void Network::move_base_statusCallback(const actionlib_msgs::GoalStatusArray::ConstPtr& msg)
{
  //如果没有状态，则返回
  if(msg->status_list.size() == 0)
    return;

  //获取move base的状态
  move_base_status = msg->status_list[0].status;
}

/*
 *判断摄像头定位回调函数的状态
 * 1: camera locate finish
 * 2: return finished
 * else : error
 */
void Network::cameraCallback(const std_msgs::UInt8::ConstPtr& msg)
{
   ROS_INFO("camera state: [%d]", msg->data);
    camera_locate_state = msg->data;
}

void Network::batteryCallback(const std_msgs::UInt8::ConstPtr& msg)
{
  //ROS_INFO("batteryCallback");
  battery_msg.data = msg->data;

}

int main(int argc, char* argv[])
{
  ros::init(argc, argv, "yun_netwotk");
  
  Network agv_net(8090, server_ip);

  //创建线程,负责读取socket，并处理数据
  boost::thread t1(boost::bind(&Network::readSocket, &agv_net));//
  boost::thread t2(boost::bind(&Network::cameraLocate, &agv_net));//绑定指针,绑定实例对象不行

  //分离线程
  t1.detach();
  t2.detach();

  agv_net.loop();

  agv_net.shutDown();

  return 0;
}


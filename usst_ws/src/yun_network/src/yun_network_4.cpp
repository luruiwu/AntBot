#include "yun_network/yun_network.h"


Network::Network(unsigned short port_num, string addr):sock_client(service),
                                                                                             ac("move_base", true)
{
  
  //设置ip和端口号
  receiver_endpoint.port(port_num);
  receiver_endpoint.address(ip::address::from_string(addr));

  //以ipv4协议打开套接字
  sock_client.open(udp::v4());

  move_base_status = 0;
  server_state = Server_Ready_State;
  AGV_Position = -2;
  io_ctl_msg.led = LED_Off;
  io_ctl_msg.fa = Fa_Up;
  io_ctl_msg.charge = Finish_Charge;
  agv_state = AGV_Ready_Navigation;
  last_agv_status = agv_state;
  battery_msg.data = 100;
  NEW_GOAL_FLAG = false;
  camera_locate_state = Camera_Default_State;

  //读取位点
  readWaypoints();

  for(int i = 0; i < float_vector.size()/2.0; i++){
    p[i].x = float_vector[i * 2];
    p[i].y = float_vector[i * 2 + 1];
    p[i].angle = 0.0;

    std::cout <<"["<< i << "]"<<  "x:" << p[i].x << ", y:" <<  p[i].y <<",angle:"  << p[i].angle << std::endl;
  }

  NUM_WAYPOINTS = float_vector.size()/2.0;

  std::cout << "#Read waypoints finished !" << std::endl;


  /*
   * 订阅者声明的几种形式ros::Subscriber status_sub = n.subscribe("/move_base/status", 1000, statusCallback);
   *使用boost::bind的时候一定要用n.subscribe<消息类型>("", 1, boosT:bind());
   */

  //电量
  /*
   * battery_sub = n.subscribe<std_msgs::UInt8>("/battery", 1, boost::bind(&Network::batteryCallback, this, _1));
   */

  /*订阅/move_base的状态信息*/
   status_sub = n.subscribe<actionlib_msgs::GoalStatusArray>("/move_base/status", 1, boost::bind(&Network::move_base_statusCallback, this, _1));
   cout << "#subscribe to /move_base/status..." << endl;

  //发布state
  state_pub = n.advertise<yun_bringup::IO_Ctl>("io_ctl", 1);
  cout << "#publish to /io_ctl" << endl;
  
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
		std::cout << "\nError opening file" << std::endl;
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
  int Last_Goal = AGV_Position;

  //等待actionlib库服务器出现
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("\nWaiting for the move_base action server to come up");
  }

  while(ros::ok()){

    len = sock_client.receive_from(buffer(recv_buf), sender_point);
    cout << endl;
    //cout << "Read data len: "<< (int)len << endl;
    if(len == 2)
    {
      //收到心跳应答
      if((recv_buf[0] == 0xD0) && (recv_buf[1] == 0x00)){
        #if DEBUG
        cout << "[HeartbeatEcho]" << endl;
        #endif
        mx.lock();
        IsHeartbeatEcho = true;
        mx.unlock();
      }
    }else if(len == 4) {
      //收到设备状态应答
      if((recv_buf[0] == 0x40) && (recv_buf[1] == 0x02)&& (recv_buf[2] == 0x00)&& (recv_buf[3] == 0x01))
      {
        #if DEBUG
        cout << "[DeviceStateEcho]" << endl;
        #endif
        mx.lock();
        DeviceStateEcho = true;
        mx.unlock();
      }
      //收到CCS连接应答
      else if((recv_buf[0] == 0x20) && (recv_buf[1] == 0x02) && (recv_buf[2] == 0x00)&& (recv_buf[3] == 0x00)){
        #if DEBUG
        cout << "[CCSEcho]" << endl;
        #endif
        mx.lock();
        IsCCSEcho = true;
        mx.unlock();
      }
      //收到设备命令(至少10个字节)
    }else if(len > 10){
      if(recv_buf[0] == 0x32){
        #if DEBUG
        cout << "[DeviceCmd]" << endl;
        #endif

        //设备命令
        mx.lock();
        IsDeviceCmd = true;
        mx.unlock();

        //将包含剩余的字节数的编码结果的数据拷贝出来
        memcpy(temp, recv_buf+1, 4);

        //解码
        ret_len = decoder(temp);

        //报文头部数据长度
        head_len = len - ret_len;

        //报文基础数据部分长度在AGV1-9之内都是8个字节
        base_len = 8;

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
              AGV_Position =  read_root["position"].asInt();

              //如果发送的不是预定义的位点
              if(AGV_Position < -1 || (AGV_Position >= NUM_WAYPOINTS)){
                continue;
              }
              cout << "------>Go to position: " << read_root["position"].asString() << endl;

              if(Last_Goal != AGV_Position){
                Last_Goal = AGV_Position;
                NEW_GOAL_FLAG = true;
                cout << "NEW_GOAL_FLAG==true" << endl;
              }
              //如果电量少于百分之30，并且当前没有其他任务，处于准备导航状态，那么发送充电装的坐标。
             /*if(battery_msg.data < 10){
               AGV_Position = 10; //预定义充电位置
               server_state = Server_Charge_State;
             }*/

            }
          }
        }//if
      }//if
    }//else if

    if((NEW_GOAL_FLAG) && (agv_state == AGV_Ready_Navigation)){
       /****************发送目标位置*******************/
       tf::Quaternion goal_quat = tf::createQuaternionFromYaw(p[AGV_Position].angle);
       current_goal.target_pose.header.stamp = ros::Time::now();
       current_goal.target_pose.header.frame_id = "/map";
       current_goal.target_pose.pose.position.x = p[AGV_Position].x;
       current_goal.target_pose.pose.position.y = p[AGV_Position].y;
       current_goal.target_pose.pose.orientation.x = goal_quat.x();
       current_goal.target_pose.pose.orientation.y = goal_quat.y();
       current_goal.target_pose.pose.orientation.z = goal_quat.z();
       current_goal.target_pose.pose.orientation.w = goal_quat.w();
       ac.sendGoal(current_goal);

       NEW_GOAL_FLAG = false;
       cout << "NEW_GOAL_FLAG==false" << endl;
     }
  }//while
}

void Network::loop()
{
  cout << "#loop" << endl;
  int i = 0;

  mx.lock();
  IsCCSEcho = false;
  IsHeartbeatEcho = false;
  IsDeviceCmd = false;
  mx.unlock();

  ros::Rate loop_rate(1);

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

    /***************发送设备状态信息*********************/
    if(agv_state == AGV_Ready_Navigation)
      server_state = Server_Ready_State;
    else if(agv_state == AGV_Navigation_Error)
      server_state == Server_AGV_Error_State;
    else if(agv_state == AGV_In_Charge)
      server_state = Server_Charge_State;
    else
      server_state == Server_Moving_State;

    write_root["AGVState"]["AGVID"] = "AGV1";
    write_root["AGVState"]["state"] = server_state;
    write_root["AGVState"]["power"] = battery_msg.data;
    write_root["AGVState"]["x"] =  transform.getOrigin().x();
    write_root["AGVState"]["y"] =  transform.getOrigin().y();
    write_root["AGVState"]["position"] = AGV_Position;
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

  /**随时打印AGV的状态信息，测试AGV在各种状态之间的切换，如果切换正确，那么程序就没有问题***/
    switch(agv_state){
      case AGV_Ready_Navigation:
        cout << "***********AGV_STATE: AGV_Ready_Navigation**********" << endl;
        break;
    case AGV_In_Navigation:
      cout << "**********AGV_STATE: AGV_In_Navigation**********" << endl;
      break;
    case AGV_Navigation_Finished:
      cout << "**********AGV_STATE: AGV_Navigation_Finished**********" << endl;
      break;
    case AGV_In_Precise_Locate:
      cout << "**********AGV_STATE: AGV_In_Precise_Locate**********" << endl;
      break;
    case AGV_Precise_Loate_Finished:
      cout << "**********AGV_STATE: AGV_Precise_Loate_Finished**********" << endl;
      break;
    case AGV_In_Precise_Locate_Return:
      cout << "**********AGV_STATE: AGV_In_Precise_Locate_Return**********" << endl;
      break;
    case AGV_In_Charge:
      cout << "**********AGV_STATE: AGV_In_Charge**********" << endl;
      break;
    case AGV_Navigation_Error:
      cout << "**********AGV_STATE: AGV_Navigation_Error**********" << endl;
      break;
    default:
      break;
    }

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

    ros::spinOnce();
    //1HZ
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

    if((move_base_status == Goal_Reached && agv_state == AGV_In_Navigation)){
      agv_state = AGV_Navigation_Finished;
      cout << "AGV_Navigation_Finished" << endl;
    }

    if(move_base_status == Goal_Accepted && agv_state == AGV_Ready_Navigation){
      agv_state = AGV_In_Navigation;
      cout << "AGV_In_Navigation" << endl;
    }

    /*开始摄像头定位*/
    if(agv_state == AGV_Navigation_Finished){
      agv_state = AGV_In_Precise_Locate;
      camera_msg.data = Start_Locate;//Locate_Charge_Pos
      io_ctl_msg.led = LED_On;
      camera_pub.publish(camera_msg);
      cout << "AGV_In_Precise_Locate" << endl;
    }

    /*摄像头定位结束*/
    if(camera_locate_state == Camera_Loate_Finished && agv_state == AGV_In_Precise_Locate){
     //io_ctl_msg.fa = Fa_Down;
     agv_state = AGV_Precise_Loate_Finished;
     cout << "AGV_Precise_Loate_Finished" << endl;
    }

    /*开始摄像头定位返回*/
    if((NEW_GOAL_FLAG) && (agv_state == AGV_Precise_Loate_Finished)){
     io_ctl_msg.led = LED_Off;
     camera_msg.data = Start_Return;
     //io_ctl_msg.fa = Fa_Up;
     agv_state = AGV_In_Precise_Locate_Return;
     camera_pub.publish(camera_msg);
     cout << "AGV_In_Precise_Locate_Return" << endl;
    }

    /*摄像头定位返航结束*/
    if((camera_locate_state == Return_Finished ) && (agv_state == AGV_In_Precise_Locate_Return)){
      agv_state = AGV_Ready_Navigation;
      cout << "AGV_Ready_Navigation" << endl;
    }

    /*摄像头定位错误*/
    if(camera_locate_state == Camera_Error){
    agv_state = AGV_Navigation_Error;
    cout << "AGV_Navigation_Error" << endl;
    }
    /********************Send Io_Ctl.msg(LED, Fa, Charge, Battery,,etc)*****************************/
    state_pub.publish(io_ctl_msg);

    ros::spinOnce();

    loop_rate.sleep();
  }
}

void Network::batteryCallback(const std_msgs::UInt8::ConstPtr& msg)
{
  //ROS_INFO("batteryCallback");
  battery_msg.data = msg->data;

}

void Network::savePose()
{
  /*
   * 保存机器人最后的位置姿态
   */
  pose_write_root["position_x"] = transform.getOrigin().x();
  pose_write_root["position_y"] = transform.getOrigin().y();
  pose_write_root["orientation_x"] = transform.getRotation().x();
  pose_write_root["orientation_y"] = transform.getRotation().y();
  pose_write_root["orientation_z"]  = transform.getRotation().z();
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

//将string转换为数字类型
template <typename Type>
Type Network::stringToNum(const string &str)
{
  istringstream iss(str);
  Type num;
  iss >> num;
  return num;
}


int main(int argc, char* argv[])
{
  ros::init(argc, argv, "yun_netwotk");
  
  Network agv_net(8090, server_ip);

  //创建线程,负责读取socket，并处理数据
  boost::thread t1(boost::bind(&Network::readSocket, &agv_net));
  boost::thread t2(boost::bind(&Network::cameraLocate, &agv_net));

  //分离线程
  t1.detach();
  t2.detach();

  agv_net.loop();

  agv_net.savePose();

  return 0;
}


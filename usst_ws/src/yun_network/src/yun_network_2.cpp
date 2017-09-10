/*
 * 创建两个线程，一个用于AGV读取Socket，一个用于摄像头定位，另外的主线程用于发送Socket
 * 
 */

#include "yun_network/yun_network.h"


Network::Network(unsigned short port_num, string addr):sock_client(service),ac("move_base", true)
{
	//设置ip和端口号
	receiver_endpoint.port(port_num);
	receiver_endpoint.address(ip::address::from_string(addr));
	
	//以ipv4协议打开套接字
	sock_client.open(udp::v4());
	
	move_base_status = 0;
	camera_status = 0;
	
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

void Network::readSocket()
{
	#if DEBUG
	cout << "Create a thread..." << endl;
	#endif
	
	size_t len = 0;
	char temp[4];
	int ret_len;
	int head_len;
	int base_len;
	const char* pjson_str;
	
	//等待actionlib库服务器出现
	while(!ac.waitForServer(ros::Duration(5.0))){
		ROS_INFO("Waiting for the move_base action server to come up");
	}
	
	while(ros::ok()){
		
		len = sock_client.receive_from(buffer(recv_buf), sender_point);
		cout << endl;
		cout << "Read data len: "<< (int)len << endl;
		if(len == 2)
		{
			//收到心跳应答
			if((recv_buf[0] == 0xD0) && (recv_buf[1] == 0x00)){
				#if DEBUG
				cout << "HeartbeatEcho" << endl;
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
				cout << "DeviceStateEcho" << endl;
				#endif
				mx.lock();
				DeviceStateEcho = true;
				mx.unlock();
			}
			//收到CCS连接应答
			else if((recv_buf[0] == 0x20) && (recv_buf[1] == 0x02) && (recv_buf[2] == 0x00)&& (recv_buf[3] == 0x00)){
				#if DEBUG
				cout << "CCSEcho" << endl;
				#endif
				mx.lock();
				IsCCSEcho = true;
				mx.unlock();
			}
		
		}else if(len > 10){//设备命令至少10个字节
			if(recv_buf[0] == 0x32){
				#if DEBUG
				cout << "DeviceCmd" << endl;
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
							int p = read_root["position"].asInt();
							cout << read_root["position"].asString() << endl;
							
							tf::Quaternion goal_quat = tf::createQuaternionFromYaw(0);
							
							current_goal.target_pose.header.stamp = ros::Time::now();
							current_goal.target_pose.header.frame_id = "/map";
							if(p == 1){
								current_goal.target_pose.pose.position.x = 1;
								current_goal.target_pose.pose.position.y = 1;
							}else if(p == 2){
								current_goal.target_pose.pose.position.x = -15;
								current_goal.target_pose.pose.position.y = 3;
							}
							current_goal.target_pose.pose.orientation.x = goal_quat.x();
							current_goal.target_pose.pose.orientation.y = goal_quat.y();
							current_goal.target_pose.pose.orientation.z = goal_quat.z();
							current_goal.target_pose.pose.orientation.w = goal_quat.w();
							
							ac.sendGoal(current_goal);
						}
					}
				
				}//if
				
				 
			}//if
		}//else if
		
	}//while
	
	
}

void Network::loop()
{
	int i = 0;
	
	mx.lock();
	IsCCSEcho = false;
	IsHeartbeatEcho = false;
	IsDeviceCmd = false;
	mx.unlock();
	
	ros::Rate loop_rate(1);
	
	//负责向socket发送数据
	while(ros::ok()){
		
		//如果么有接受到CCS反馈loop
		if(!IsCCSEcho){
			//发送CCS
			send_buf = hex2bytes(CCS);
			sock_client.send_to(buffer(send_buf), receiver_endpoint);
		}
		
		//任何情况下都要发送心跳包10秒钟/次
		if(i == 5){
			i = 0;
			//发送Heartbeat
			send_buf = hex2bytes(Heartbeat);
			sock_client.send_to(buffer(send_buf), receiver_endpoint);
		}
		
		//如果接收到设备命令，发送接收到设备命令的应答
		if(IsDeviceCmd){
			//发送DeviceCmdEcho
			send_buf = hex2bytes(DeviceCmdEcho);
			sock_client.send_to(buffer(send_buf), receiver_endpoint);
		}
		
		/*
		write_root["state"] = 0;
		write_root["command"] = 1;
		write_root["power"] = 80;
		write_root["direction"] = 120;
		write_root["x"] = 100;
		write_root["y"] = 200;
		write_root["position"] = 10;
		
		//转换为json格式字符串
		DeviceState = write_root.toStyledString();
		cout << DeviceState << endl;
		*/
		//发送设备状态信息1次/s
		send_buf = hex2bytes(DeviceState);
		sock_client.send_to(buffer(send_buf), receiver_endpoint);
		
		//频率1HZ
		//wait(1);
		ros::spinOnce();
		
		loop_rate.sleep();
		
		i++;
	}
	
}

void Network::coder(int len, char buf[])
{
	int x, d, i;
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
	int d;
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


void statusCallback(const actionlib_msgs::GoalStatusArray::ConstPtr& msg)
{
	/*move_base status
	 * 1 : this goal accepted by simple action
	 * 2 : 
	 * 3 : Goal reached 
	 * 4 : Robot is oscillating
	 * 
	 */
	//从发布到回调很慢，比用rostopic echo 慢，不知道为什么
	//不是开了线程的原因
	ROS_INFO("move_base status: [%d]", msg->status_list[0].status);
	
	//获取move base的状态
	move_base_status = msg->status_list[0].status;
	
}

void cameraCallback(const std_msgs::UInt8::ConstPtr& msg)
{
	ROS_INFO("camera state: [%d]", msg->data);
	if(msg->data == 1)
		cout << "camera locate finish" << endl;
	else if(msg->data == 2)
		cout << "return finish" << endl;
	else
		cout << "error" << endl;
	
}
//用于发布消息摄像头定位节点
/*
 * 开始摄像头定位：1
 * 开始返航：2
 * ERROR：4
 */
void Network::cameraLocate()
{
	ros::Rate loop_rate(10);
	
	ros::Publisher  camera_pub = n.advertise<std_msgs::UInt8>("/RD_state", 1);
	cout << "publish /RD_state..." << endl;
	
	//用于接收摄像头定位程序通信
	ros::Subscriber camera_sub = n.subscribe("/camera_state", 1, cameraCallback);
	cout << "subscribe to /camera_state..." << endl;

	//同于订阅/move_base的状态信息
	ros::Subscriber status_sub = n.subscribe("/move_base/status", 1, statusCallback);
	cout << "subscribe to /move_base/status..." << endl;
	
	std_msgs::UInt8 msg;
	msg.data = 0;
	
	//等待订阅创建好连接
	sleep(3);
	while(ros::ok()){
		
		//如果结束导航结束，那么开始定位
		if((move_base_status == 3) || (move_base_status == 4)){
			msg.data = 1;
			camera_pub.publish(msg);
		}
		
		/*如果机械手臂的动作完成
		if(camera_status = ){
			msg.data = 2;
			camera_pub,publish(msg);
		}
		*/
		ros::spinOnce();
		
		loop_rate.sleep();
	}
	
	
	
}


int main(int argc, char* argv[])
{
	ros::init(argc, argv, "yun_netwotk");
	
	Network agv_net(8090, "192.168.1.8");
	

	//创建线程,负责读取socket，并处理数据
	boost::thread t1(boost::bind(&Network::readSocket, &agv_net));
	//boost::thread t2(boost::bind(&Network::cameraLocate, &agv_net));
	
	//分离线程
	t1.detach();
	//t2.detach();
	//t2.join();
	
	agv_net.loop();
	
	
	return 0;
}






























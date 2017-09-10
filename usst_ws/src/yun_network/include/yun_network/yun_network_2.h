/*
 * 这个程序用于接收服务器的命令，并对命令进行解析后发送给AGV的各执行部分；
 * 同时也负责及爱那个AGV的状态信息（位置姿态，电量等）发送给服务端。
 * 该通信程序基于UDP网络协议，通信协议接口规范见：智能工厂系统-A2接口规范-AGV.docx文档
 * 2017-6-5 By Bob
 * 单独编译命令：g++ yun_network.cpp -lboost_system -lboost_thread -lpthread -ljson_linux-gcc-4.8_libmt -o yun_network
 * 
*/
#include "ros/ros.h"
#include <std_msgs/UInt8.h> //for UInt8.msg
#include <actionlib_msgs/GoalStatusArray.h> // for /move_base/status
#include <iostream>
#include <sstream> //for stringstream
#include <string>
#include <cstring> //for memcpy
#include <vector>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <json/json.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/tf.h>


#define DEBUG true

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

boost::mutex mx;

string CCS = "1332000653444950413203C4025800044147563"
			 "100044155544F000C646973636F6E6E65637465"
			 "64000473687264000430303030";

string CCSEcho = "20020000";

string BreakCCS = "E000";

string DeviceCmdEcho = "40020001";

string DeviceCmd = "322B000441475631000100217B2261637"
					"4696F6E223A226D6F7665222C2278223"
					"A3132332C2279223A3435367D";

string DeviceState = "32150004414756310001000B7B2276616C7565223A317D";

string DeviceStateEcho = "40020001";

string Heartbeat = "C000";

string HeartbeatEcho = "D000";

uint8_t move_base_status;
uint8_t camera_status;

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

class Network{

public:
	
	//构造函数,[端口，地址]
	Network(unsigned short port_num, string addr);
	
	//析构函数
	~Network(){};
	
	//将hex字符串转换为Byte矩阵
	std::vector<unsigned char> hex2bytes(const std::string& hex);
	
	//循环
	void loop();
	
	//读取socket
	void readSocket();
	
	//等待n秒
	void wait(int seconds);
	
	/*可变长度的数据部分字节长度编解算法*/
	//编码
	void coder(int len, char buf[]);
	
	//解码
	int decoder(char buf[]);
	
	//摄像头定位
	void cameraLocate();
	

private:
	//io服务
	io_service service;
	
	//发送缓冲区ASCII码表 0-127
	vector<unsigned char> send_buf;
	
	//接收缓冲区
	unsigned char recv_buf[1024];
	//string recv_buf;
	
	//服务端点
	udp::endpoint receiver_endpoint;
	
	//客户端点
	udp::endpoint sender_point;
	
	//创建套接字客户端
	udp::socket sock_client;
	
	/*以下变量是两个县城之间的共享数据*/
	bool IsCCSEcho;
	bool IsDeviceCmd;
	bool IsHeartbeatEcho;
	
	Json::Reader reader;
	Json::Value read_root;
	Json::Value write_root;
	
	ros::NodeHandle n;
	
	//创建消息
	move_base_msgs::MoveBaseGoal current_goal;
	
	//创建actionlib客户端，定义带参数的类
	MoveBaseClient ac;
	

};

/*
 * “发送设备控制指令”报文扩展数据部分参数JSON定义示例：
 * {
 *  "action":"move", //位置移动指令move, go, arm
 *  "x":200,         //move 目标位置X坐标
 *  "y":300,         //move 目标位置Y坐标
 *  "position":"1",  //go
 * }
 */

/*
 * “发送设备状态信息”报文扩展数据部分参数JSON定义示例：
 * {
 * "state":0, //运行状态   0:静止就绪  1：移动中 98：设备控制指令无法执行  99：故障  100：正在充电
 * "command":{...}//设备控制指令（源指令）
 * "power":100, //电量: 0-100%
 * "direction":0, //方向 0-360度
 * "x":100, //X坐标
 * "y":200, //Y坐标
 * "position":,""//预定义位置1-10
 * }
 */






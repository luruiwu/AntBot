/*
 * 这个程序用于接收服务器的命令，并对命令进行解析后发送给AGV的各执行部分；
 * 同时也负责及爱那个AGV的状态信息（位置姿态，电量等）发送给服务端。
 * 该通信程序基于UDP网络协议，通信协议接口规范见：智能工厂系统-A2接口规范-AGV.docx文档
 * 2017-7-5 By Bob
 * 单独编译命令：g++ yun_network.cpp -lboost_system -lboost_thread -lpthread -ljson_linux-gcc-4.8_libmt -o yun_network
 */

/*To Do List
 * 动态配置一些参数：
 * 1、机器人震动的时候么处理；
 * 2、机器人运动方向会莫名的改变；
 *4、如何停止机器人发送导航点-1
 * 5、机器人设置一个待机点，进入这个点不需要进行精确定位等操作
 * 6、到达目标点之后向机器人反馈
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
#include <nav_msgs/Odometry.h>
#include <boost/bind.hpp>
#include <stdio.h>
#include <yun_bringup/IO_Ctl.h> //for State.msg LED, Fa,Charge
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <std_srvs/Empty.h>
#include <fstream>
#include <signal.h>
#include <tf/transform_listener.h>
#include <boost/algorithm/string.hpp>  //for split


#define DEBUG true

#define json_path  "/home/pepper/usst_ws/src/yun_global_planner/pose.json"

#define  pointPath  "/home/pepper/usst_ws/src/yun_global_planner/script/waypoints.txt"

#define server_ip "192.168.1.11"

#define MAX_CODER_STR_LENGTH 5

//最大的位点的个数
#define MAX_POS_NUM 50

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

boost::mutex mx;

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;


/*
 * 预定义的位置结构体
 */
struct Position{
	float x;
	float y;
	float angle; //弧度
};


/*
 * 开始摄像头定位的时候打开LED；摄像头定位返航后关闭LED
 */
enum LED{
  LED_On=1,
  LED_Off=2
};

/*
 *摄像头定位完成后Fa下降，机械手臂完成工作后Fa上升
 */
enum Fa{
  Fa_Up=1,
  Fa_Down=2
};

/*
 *服务器发送充电指令，开始充电，服务发送结束充电指令，停止充电
 */
enum Charge{
  Start_Charge=1,
  Finish_Charge=2
};

enum Camera_State{
  Camera_Default_State=0,
  Camera_Loate_Finished=1,
  Return_Finished=2,
  Camera_Error=4
};

enum Camera_Locate{
  Start_Locate=1,
  Start_Return=2,
  Locate_Charge_Pos=3
};

/*
 * pepper@X:/opt/ros/indigo/share/actionlib_msgs/msg$ cat GoalStatus.msg
GoalID goal_id
uint8 status
uint8 PENDING         = 0      # The goal has yet to be processed by the action server
uint8 ACTIVE          = 1         # The goal is currently being processed by the action server
uint8 PREEMPTED       = 2   # The goal received a cancel request after it started executing
                                                #   and has since completed its execution (Terminal State)
uint8 SUCCEEDED       = 3   # The goal was achieved successfully by the action server (Terminal State)
uint8 ABORTED         = 4   # The goal was aborted during execution by the action server due
                              #    to some failure (Terminal State)
uint8 REJECTED        = 5   # The goal was rejected by the action server without being processed,
                            #    because the goal was unattainable or invalid (Terminal State)
uint8 PREEMPTING      = 6   # The goal received a cancel request after it started executing
                            #    and has not yet completed execution
uint8 RECALLING       = 7   # The goal received a cancel request before it started executing,
                            #    but the action server has not yet confirmed that the goal is canceled
uint8 RECALLED        = 8   # The goal received a cancel request before it started executing
                            #    and was successfully cancelled (Terminal State)
uint8 LOST            = 9   # An action client can determine that a goal is LOST. This should not be
                            #    sent over the wire by an action server
*/

enum Move_Base_Status{
  Goal_Accepted = 1,
  Goal_Reached = 3,
  Goal_ABORTED = 4
};


//AGV返回给服务器的状态
enum Server_State{
  Server_Ready_State=0, //静止就绪
  Server_Moving_State=1, //移动中
  Server_Commad_Failed_State=98, //执行命令失败
  Server_AGV_Error_State=99, //AGV故障
  Server_Charge_State=100 //AGV在充电状态
};

/*
 *AGV的运动状态分为几个：
 * 0、 准备导航；
 * 1、正在导航；
 * 2、导航结束；
 * 3、正在精确定位；
 * 4、精确定位结束；
 * 5、正在精确定位返回；
 * 6、充电状态；
 * 7、导航出错。
 *  AGV的所有的运动都在这几种运动状态之间进行切换，当导航出错的时候，应该马上向服务器报错，并且停止所有任务和运动。
 * AGV的状态切换都在状态的callback函数中完成。
 */
enum AGV_State{
  AGV_Ready_Navigation, //准备导航
  AGV_In_Navigation, //正在导航
  AGV_Navigation_Finished, //导航结束
  AGV_In_Precise_Locate, //正在摄像头定位
  AGV_Precise_Loate_Finished, //摄像头定位结束
  AGV_In_Precise_Locate_Return, //正在精确定位返回
  AGV_In_Charge, //充电状态
  AGV_Navigation_Error //导航出错
};

string CCS = "1332000653444950413203C4001E00044147563"
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

class Network{

public:
	
	//构造函数,[端口，地址]
	Network(unsigned short port_num, string addr);
	
	//析构函数
	~Network(){}
	
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
	
	//odomCallback
	void odomCallback(const nav_msgs::Odometry::ConstPtr& msg);
	
	void batteryCallback(const std_msgs::UInt8::ConstPtr& msg);
	
	void cameraCallback(const std_msgs::UInt8::ConstPtr& msg);
	
  void move_base_statusCallback(const actionlib_msgs::GoalStatusArray::ConstPtr& msg);
	
	int setInitPose();

  //保存机器人最后的位置姿态
  void savePose();

  //读取位点
  int readWaypoints();

  /*将字符串转化为数字*/
  template<typename Type> Type stringToNum(const string&);

private:
	//io服务
	io_service service;
	
	//发送缓冲区ASCII码表 0-127
	vector<unsigned char> send_buf;
	
	//接收缓冲区
	unsigned char recv_buf[1024];
	
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
	
	std_msgs::UInt8 battery_msg;
	
	yun_bringup::IO_Ctl io_ctl_msg;
	
	ros::Publisher state_pub;
	
	ros::Publisher initialpose_pub;
	
	ros::Subscriber battery_sub;

  ros::Subscriber status_sub;
	
	uint8_t move_base_status;
	
	int8_t AGV_Position;
	
	Server_State server_state;
	
	AGV_State agv_state;

  /*记录上一个AGV的状态*/
  AGV_State last_agv_status;

  //预定义的位置
  Position p[MAX_POS_NUM];

  /*新的导航目标点的标志*/
  bool NEW_GOAL_FLAG;

  uint8_t camera_locate_state;

  tf::TransformListener listener;

  tf::StampedTransform transform;

  //读取pose.json
  Json::Reader pose_reader;
  Json::Value pose_read_root;
  Json::Value pose_write_root;

  //位点容器
  vector<float> float_vector;

  int NUM_WAYPOINTS;
};

/*
 * “发送设备控制指令”报文扩展数据部分参数JSON定义示例：
 * {
 *  "action":"move", //位置移动指令move, go, arm
 *  "x":200,         //move 目标位置X坐标
 *  "y":300,         //move 目标位置Y坐标
 *  "position":1,  //go
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


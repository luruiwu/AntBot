//2017-5-27 By Bob
//g++ client_4.cpp -lboost_system -lboost_thread -lpthread -ljson_linux-gcc-4.8_libmt -o client_4
//此报文适合AGV编号1-9
//该程序完成了对socket的读写和json字符串的解析

#include <iostream>
#include <sstream>
#include <string>
#include <cstring> //for memcpy
#include <vector>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <json/json.h>

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
	
};

Network::Network(unsigned short port_num, string addr):sock_client(service)
{
	//设置ip和端口号
	receiver_endpoint.port(port_num);
	receiver_endpoint.address(ip::address::from_string(addr));
	
	//以ipv4协议打开套接字
	sock_client.open(udp::v4());

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
	
	while(1){
		
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
				//cout << "The ret buf len is : " << ret_len << endl;
				
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
					if(!read_root["action"].isNull())
						cout << read_root["action"] << endl;
					if(!read_root["move"].isNull())
						cout << read_root["move"] << endl;
					if(!read_root["position"].isNull())
						cout << read_root["position"] << endl;
					if(!read_root["x"].isNull())
						cout << read_root["x"] << endl;
					if(!read_root["y"].isNull())
						cout << read_root["y"] << endl;
				}
				 
			}
		}
		
	}
	
	
}

void Network::loop()
{
	int i = 0;
	
	mx.lock();
	IsCCSEcho = false;
	IsHeartbeatEcho = false;
	IsDeviceCmd = false;
	mx.unlock();
	
	
	//负责向socket发送数据
	while(1){
		
		//如果么有接受到CCS反馈
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
		wait(1);
		
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


int main(int argc, char* argv[])
{

	Network agv_net(8090, "192.168.1.9");
	
	//创建线程,负责读取socket，并处理数据
	boost::thread t(boost::bind(&Network::readSocket, &agv_net));
	
	//分离线程
	t.detach();

	agv_net.loop();
	
	
	return 0;
}


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

































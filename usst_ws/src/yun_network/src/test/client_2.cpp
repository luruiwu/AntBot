/*
 * AGV发送给CCS的设备状态信息，没收到应答信息之前要一直的发送
 */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;


class Network{

public:
	
	//构造函数,[端口，地址]
	Network(unsigned short port_num, string addr);
	
	//析构函数
	~Network(){};
	
	//将hex字符串转换为Byte矩阵
	std::vector<unsigned char> hex2bytes(const std::string& hex);
	
	//连接CCS,返回0代表连接CCS成功
	int CCS();
	
	//断开CCS
	void BreakCCS();
	
	//发送心跳包
	int Heartbeat();
	
	//发送设备状态信息
	int DeviceState();
	
	//发送设备控制指令
	void DeviceCmd();
	
	//循环
	void Loop();

private:
	//io服务
	io_service service;
	
	//发送缓冲区ASCII码表 0-127
	vector<unsigned char> send_buf;
	
	//接收缓冲区
	array<unsigned char, 100>recv_buf;
	//string recv_buf;
	
	//服务端点
	udp::endpoint receiver_endpoint;
	
	//客户端点
	udp::endpoint sender_point;
	
	//创建套接字客户端
	udp::socket sock_client;
	
	
	
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

int Network::CCS()
{
	//hex字符串
	string str = "1332000653444950413203C4025800044147563"
				 "100044155544F000C646973636F6E6E65637465"
				 "64000473687264000430303030";
				 
	 //填充发送缓冲区
	send_buf = hex2bytes(str);
	
	//发送CCS连接请求到服务器
	sock_client.send_to(buffer(send_buf), receiver_endpoint);
	
	//从端点读取数据
	size_t len = sock_client.receive_from(buffer(recv_buf), sender_point);
	
	#if 1
	cout << "Recv " << len << " Bytes from server :" << endl;
	for(size_t i = 0; i < len; i++){
		cout << "[" << i <<"]: 0x"<< hex <<(int)recv_buf[i];
		cout << endl;
	}
	#endif
	
	//判断连接CCS是否成功
	if((len == 4) 
		&& (recv_buf[0] == 0x20)
		&& (recv_buf[1] == 0x02)
		&& (recv_buf[2] == 0x00)
		&& (recv_buf[3] == 0x00))
	{
		return 0;
	}else
		return -1;
		
}

void Network::BreakCCS()
{
	//hex字符串
	string str = "E000";
				 
	 //填充发送缓冲区
	send_buf = hex2bytes(str);
	
	//发送CCS连接请求到服务器
	sock_client.send_to(buffer(send_buf), receiver_endpoint);
	
}

int Network::Heartbeat()
{
	//hex字符串
	string str = "C000";
	
	send_buf = hex2bytes(str);
	
	sock_client.send_to(buffer(send_buf), receiver_endpoint);
	
	size_t len = sock_client.receive_from(buffer(recv_buf), sender_point);
	
	if((len == 2) && (recv_buf[0] == 0xD0) && (recv_buf[1] == 0x00))
	{
		return 0;
	}else
		return -1;
	
}


int Network::DeviceState()
{
	//hex字符串
	string str = "32150004414756310001000B7B2276616C7565223A317D";
	
	send_buf = hex2bytes(str);
	
	sock_client.send_to(buffer(send_buf), receiver_endpoint);
	
	size_t len = sock_client.receive_from(buffer(recv_buf), sender_point);
	
	if((len == 4) 
		&& (recv_buf[0] == 0x40) 
		&& (recv_buf[1] == 0x02)
		&& (recv_buf[2] == 0x00)
		&& (recv_buf[3] == 0x01))
	{
		return 0;
	}else
		return -1;
}

void Network::DeviceCmd()
{
	//接收设备控制指令
	size_t len = sock_client.receive_from(buffer(recv_buf), sender_point);
	
	//确认收到设备控制指令
	if(len > 0){
		//hex字符串
		string str = "40020001";
		
		send_buf = hex2bytes(str);
		
		sock_client.send_to(buffer(send_buf), receiver_endpoint);
	
	}
	
}

//循环
void Network::Loop()
{
	//连接CCS
	CCS();
	
	
	
}

int main(int argc, char* argv[])
{

	Network agv_net(8090, "192.168.1.9");
	
	agv_net.Loop();
	
	return 0;
}






/**
 * 
 * 
 * 
AGV --> CCS
string CCS = "1332000653444950413203C4025800044147563"
			 "100044155544F000C646973636F6E6E65637465"
			 "64000473687264000430303030";
string BreakCCS = "E000";
string DeviceCmdEcho = "40020001";
string DeviceState = "32150004414756310001000B7B2276616C7565223A317D";
string Heartbeat = "C000";


CCS --> AGV
string CCSEcho = "20020000";
string DeviceCmd = "";
string DeviceStateEcho = "40020001";
string HeartbeatEcho = "D000";
 * 
 * /




































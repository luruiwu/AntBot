/*
 * AGV发送给CCS的设备状态信息，没收到应答信息之前要一直的发送
 * 
 * 这个程序要写成异步的，因为机器人在不断接收数据的同时要不断的发送
 * 心跳数据包，因此，如果接收程序阻塞了，那么就不能按时发送数据包了。
 * 除非写成多线程的，主线程不断发送心跳数据包和设备状态信息，
 * 等有数据来的时候创建新的线程去读取socket。
 */

#include <iostream>
#include <sstream>
#include <string>
#include <cstring> //for memcpy
#include <vector>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

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

void Sum()
{
	cout <<"Sum" << endl;
	
}

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
		
		}else if(len > 10){//设备命令至少15个字节
//32 2d 0 4 41 47 56 31 0 1 0 23 7b 22 61 63 74 69 6f 6e 22 3a 22 6d 6f 76 65 22 2c 22 78 22 3a 31 32 33 2c 22 79 22 3a 34 35 36 7d d a
						  //0 23 7b 22 61 63 74 69 6f 6e 22 3a 22 6d 6f 76 65 22 2c 22 78 22 3a 31 32 33 2c 22 79 22 3a 34 35 36 7d
			if(recv_buf[0] == 0x32){
				#if DEBUG
				cout << "DeviceCmd" << endl;
				#endif
				
				//设备命令
				mx.lock();
				IsDeviceCmd = true;
				mx.unlock();
				
				//将包含剩余的字节数的编码结果的数据拷贝出来
				char temp[4];
				memcpy(temp, recv_buf+1, 4);
				//解码
				int ret_len = decoder(temp);
				cout << "The ret buf len is : " << ret_len << endl;
				
				//报文头部数据长度
				int head_len = len - ret_len;
				
				//报文基础数据部分长度在AGV1-9之内都是8个字节
				int base_len = 8;
				
				//扩展数据开始字节
				int expand_start = head_len + base_len;
				
				//扩展数据部分长度
				int expand_len = recv_buf[expand_start] * 16 + recv_buf[expand_start + 1];
				
				char expand_buf[expand_len];
				//复制recv_buf中的扩展数据到expand_buf数组中
				memcpy(expand_buf, recv_buf+expand_start + 2, expand_len);
				
				for(int i = 0; i < expand_len; i++)
					cout << std::hex << (int)expand_buf[i] << " ";
				cout << endl;
				 //处理后面的命令数据
				 
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
	//读bool变量的时候可以不加锁，因为写的时候已经加锁了????????????????????????????????????????????
	while(1){
		
		//如果么有接受到CCS反馈
		if(!IsCCSEcho){
			//发送CCS
			send_buf = hex2bytes(CCS);
			//mx.lock();
			sock_client.send_to(buffer(send_buf), receiver_endpoint);
			//mx.unlock();
		}
		
		//任何情况下都要发送心跳包10秒钟/次
		if(i == 5){
			i = 0;
			//发送Heartbeat
			send_buf = hex2bytes(Heartbeat);
			//mx.lock();
			sock_client.send_to(buffer(send_buf), receiver_endpoint);
			//mx.unlock();
		}
		
		//如果接收到设备命令，发送接收到设备命令的应答
		if(IsDeviceCmd){
			//发送DeviceCmdEcho
			send_buf = hex2bytes(DeviceCmdEcho);
			//mx.lock();
			sock_client.send_to(buffer(send_buf), receiver_endpoint);
			//mx.unlock();
		}
		
		//发送设备状态信息1次/s
		send_buf = hex2bytes(DeviceState);
		//mx.lock();
		sock_client.send_to(buffer(send_buf), receiver_endpoint);
		//mx.unlock();
		
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




































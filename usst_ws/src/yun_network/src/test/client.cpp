#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

//将hex字符串转换为Byte矩阵
std::vector<char> hex2bytes(const std::string& hex);

//hex字符串
string str = "1332000653444950413203C4025800044147563"
			 "100044155544F000C646973636F6E6E65637465"
			 "64000473687264000430303030";

int main(int argc, char* argv[])
{
	try
	{
		std::vector<char> send_buf;
		char recv_buf[100];
		
		send_buf = hex2bytes(str);
		
		//创建io服务
		io_service io_service;
		
		//创建端点
		udp::endpoint receiver_endpoint(ip::address::from_string("192.168.1.9"), 8090);
		
		//创建套接字
		udp::socket sock_client(io_service);
		
		//以ipv4协议打开socket
		sock_client.open(udp::v4());
		
		//发送数据
		sock_client.send_to(buffer(send_buf), receiver_endpoint);
		
		//发送端点
		udp::endpoint sender_point;
		
		//接收数据
		size_t len = sock_client.receive_from(buffer(recv_buf), sender_point);
		
		cout << "Recv " << len << " Bytes from server :" << endl;
		
		for(size_t i = 0; i < len; i++){
			cout << "[" << i <<"]: 0x"<< hex <<(int)recv_buf[i];
			cout << endl;
		}
		
	}catch (exception& e){
		cerr << "Error: "<<e.what() << endl;
		
	}
	
	return 0;
}

//将hex字符串转换为Byte矩阵
std::vector<char> hex2bytes(const std::string& hex)
{
	std::vector<char> bytes;
	
	for(unsigned int i = 0; i < hex.length(); i += 2){
		
		//substr(n, m)取从n开始的m位字符
		std::string byteString = hex.substr(i, 2);
		
		//strtol将字符串根据base转换为整形数据
		char byte = (char)strtol(byteString.c_str(), NULL, 16);
		
		bytes.push_back(byte);
	}
	return bytes;

}














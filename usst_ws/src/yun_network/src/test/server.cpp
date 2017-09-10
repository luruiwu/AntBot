#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost;
using namespace std;

int main(int argc, char*argv[])
{
	try{
		//创建io服务
		io_service io_service;
		
		//创建socket对象，可以连接任何ipv4协议的端点
		udp::socket sock_server(io_service, udp::endpoint(udp::v4(), 5000));
		
		//循环等待
		while(true){
			
			//创建缓冲区
			//array<char, 100> recv_buf;
			char recv_buf[100];
			
			//创建远程断点
			udp::endpoint remote_endpoint;
			
			//boost系统错误
			boost::system::error_code error;
			
			//阻塞接受数据
			sock_server.receive_from(buffer(recv_buf), remote_endpoint, 0, error);
			
			//打印远程端点的地址
			cout << recv_buf << endl;
			cout << remote_endpoint.address().to_string() << endl;
			
			//判断是否出错
			if(error && (error != error::message_size))
				throw boost::system::system_error(error);
				
			//消息
			string message = "[Server] : Hello\n";
			
			boost::system::error_code ignored_error;
			sock_server.send_to(buffer(message), remote_endpoint, 0, ignored_error);
		}
		
	}catch(std::exception& e){
		cerr << e.what() << endl;
	}
	return 0;
}


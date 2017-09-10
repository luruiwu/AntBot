#include "ros/ros.h"
#include <iostream>
#include <sstream> //for stringstream
#include <string>
#include <cstring> //for memcpy
#include <vector>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <json/json.h>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

class BoTele{
	
public:
	BoTele(unsigned short port_num, string addr);
	
	~BoTele(){};
	
private:
	io_servcie sevice;
	
	udp::endpoint sender_point;
	
	udp::endpoint receiver_point;
	
	udp::socket sock_client;
	
};

BoTele::BoTele(unsigned short prot_num, string addr):sock_client(service)
{
	receiver_point.port(port_num);
	receiver_point.address(ip::address::from_string(addr));
	
	sock_client.open(udp::v4());
}


int main(in argc, char*argv[])
{
	BoTele bo_tele(8090, "");
	
	
	
	return 0;
}






















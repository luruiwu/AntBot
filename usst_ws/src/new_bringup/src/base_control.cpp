/*
g++ base_control.cpp -lboost_system

通过ROS参数设置：
设备名称：/dev/USB0
波特率：115200



*/
#include "base_control.h"


using namespace std;
using namespace boost::asio;

void handle_read(char* buf, boost::system::error_code ec, std::size_t bytes_transferred)
{
	cout.write(buf, bytes_transferred);
}

int main(int argc ,char* argv[])
{
	
	io_service iosev;
	boost::system::error_code ec;
	string str = "Hello Async Serial\n";
	char buf[100];
	
	try{
		serial_port sp(iosev);
		sp.open("/dev/ttyUSB0");
		sp.set_option(serial_port::baud_rate(115200));
		sp.set_option(serial_port::flow_control(serial_port::flow_control::none));
		sp.set_option(serial_port::parity(serial_port::parity::none));
		sp.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
		sp.set_option(serial_port::character_size(8));
		
		//写数据
		write(sp, buffer(str, str.length()));

		//读数据
		async_read(sp, buffer(buf), boost::bind(handle_read, buf, _1, _2));

		//100ms后超时
		deadline_timer timer(iosev);
		timer.expires_from_now(boost::posix_time::millisec(100));

		//超时后调用 sp 的 cancel()方法放弃读取更多字符
		timer.async_wait(boost::bind(&serial_port::cancel, boost::ref(sp)));

		iosev.run();

		sp.close();

	}catch(std::exception& e){
		cout << e.what() << endl;
	}

	return 0;
}

#include <algorithm>
#include <iostream>

using namespace std;

int main(int argc, char*argv[])
{

	//cout << "argv[1]:" << atoi(argv[1]) << endl;
	char a[5] = {0x32, 0x00, 0x2B, 0x00, 0x04};
	char* i = find(a, a+4, 0x32);
	
	if(i != a+4)
		cout <<"i = " << std::hex << (int)(*i) << endl;
	else
		cout << "No result !" << endl;


	return 0;
}

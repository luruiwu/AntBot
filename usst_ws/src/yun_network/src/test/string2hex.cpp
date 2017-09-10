#include <iostream>
#include <sstream>

using namespace std;

int main()
{
	std::stringstream str;
	string s1 = "5f0066";
	str << s1;
	char value[10];
	
	str >> std::hex >> value;
	
	cout << value[2] << endl;

	return 0;
}

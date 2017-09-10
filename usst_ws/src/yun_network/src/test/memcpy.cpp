#include <iostream>
#include <cstring>

int main()
{
	char a1[] = "Hello";
	char a2[10];
	
	memcpy(a2, a1+1, sizeof(a1));
	
	std::cout << "a1: "<< a1 << std::endl;
	std::cout << "a2: "<< a2 << std::endl;

	return 0;
}

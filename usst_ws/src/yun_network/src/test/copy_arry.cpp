#include <iostream>
#include <string>

using namespace std;

int main()
{
	char a[] = {'a', 'b', 'c', 'd', 'e', 'f'};
	
	for(int i = 0; i < sizeof(a); i++)
		cout << a[i]  << " " << endl;	
	char* pa = a + 2;
	
	string str(pa);
	
	cout << "pa[2] : " << pa[2] << endl;
	
	cout << "str: " << str << endl;
	
	return 0;

}

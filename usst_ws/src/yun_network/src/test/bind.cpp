#include <iostream>
#include <boost/bind.hpp>
#include <stdio.h>

void test(int a, int b, int c)
{
  int sum = a + b +c;
  printf("The sum : %d\n", sum);

}
int main(int argc, char* argv[])
{
  //绑定函数
  boost::bind(test, 1, _1, _2)(2, 3);


  return 0;
}



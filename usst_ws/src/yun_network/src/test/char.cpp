#include <iostream>

int main(int argc, char* argv[])
{

  char s[20] = "123456";
  int i = 0;

 std:: cout << sizeof(s) << std::endl;
  while((s[i] != '\0') && (i < 20))
  {
    std::cout << s[i++] << std::endl;
  }







  return 0;
}

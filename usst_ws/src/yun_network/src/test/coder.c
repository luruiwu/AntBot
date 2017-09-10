#include <stdio.h>

//0x80 = 128

//解码
int decoder(char buf[])
{
  int m = 1;
  int len = 0;
  int d;
  int i = 0;
  
  do{
      //读取字符
       d = buf[i];
       
       //计算长度
       len += (d &0x7F) *m;
       
       //
       m *= 0x80;
       i++;
  }while((d & 0x80) != 0);

  return len;
}

//编码
void coder(int len, char buf[])
{
  int x, d, i;
  x = len;
  
  do{
       //求余数
       d = x % 0x80;

       //求倍数
       x = x / 0x80;

       //如果有进位，那么将每个字节的最高位设置为1
       if(x > 0){
           d = d | 0x80;
       }
       
       //储存
       buf[i] = d;
       i++;
  }while(x > 0);
}


int main()
{
  long int len = 1000;
  
  char str[5] = {0x2B, 0x00, 0x04};
/*
  coder(len, str);

  int i = 0;
  for(i = 0; i < 5; i++)
    printf("BUF[%d] :%d\n",i,str[i]);
*/
  len = decoder(str);
  
  printf("The length of str is %ld\n", len);
  
  return 0;
}





/*
g++ json_test.cpp -ljson_linux-gcc-4.8_libmt -o json_test
*/
//json的key字符流按照地一个字母的大小排序

#include <json/json.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>

//#define READ 
using namespace std;

int main()
{
#ifdef READ
    ifstream is;
     is.open ("JsonText", std::ios::binary );
      Json::Reader reader;
      Json::Value root;
      if(reader.parse(is,root))   ///root保存整个Json对象的value
      {
           if(!root["name"].isNull())
           {
               cout<<root["name"].asString()<<endl;    ///读取元素
             Json::Value arrayObj = root["array"];
             for(int i=0 ; i< arrayObj.size() ;i++)
             {
                 cout<<arrayObj[i].asString()<<endl;
             }
           }
      }
#else 
      Json::Value root;
      Json::Value arrayObj;
      Json::Value item;
 
      for (int i = 0; i < 2; i ++)
      {
       arrayObj.append(i);    ///给arrayObj中添加元素(arrayObj变为数组)
      }
 
      root["key1"] = "value1";   ///给root中添加属性(arrayObj变为map)
      root["key2"] = "value2";
      root["array"] = arrayObj;
      root["b"]  ="bob";
      std::string out = root.toStyledString();   ///转换为json格式字符串
      std::cout << out << std::endl;
      std::string json = "{\"AGVState\":{ \"AGVID\":\"AGV1\", \"state\":0, \"power\":100, \"x\":0, \"y\":0, \"position\":1} }";
      cout << json << endl;

#endif
      return 0;
}
/*

{
    "name": "json",
    "array": [
           "123",
        "456",
        "789"
     ]
}

*/













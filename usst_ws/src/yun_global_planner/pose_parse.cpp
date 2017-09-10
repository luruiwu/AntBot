/**g++ pose_parse.cpp -ljson_linux-gcc-4.8_libmt -o pose_parse**/
#include <string>
#include <json/json.h>
#include <iostream>
#include <fstream>

int main(int argc ,char* argv[])
{
  //std::string pose_str;
  Json::Reader reader;
  Json::Value read_root;
  
  std::ifstream infile("pose.json", std::ios::binary);
  if(!infile.is_open()){
  	std::cout << "Error opening file\n";
  	return -1;
  }


  if(reader.parse(infile, read_root)){
    float position_x = read_root["position_x"].asFloat();
    float position_y = read_root["position_y"].asFloat();
    float orientation_x = read_root["orientation_x"].asFloat();
    float orientation_y = read_root["orientation_y"].asFloat();
    float orientation_z = read_root["orientation_z"].asFloat();
    float orientation_w = read_root["orientation_w"].asFloat();
  std::cout << "position_x: " << position_x << std::endl;
  std::cout << "position_y: " << position_y << std::endl;
  std::cout << "orientation_x: " << orientation_x << std::endl;
  std::cout << "orientation_y: " << orientation_y << std::endl;
  std::cout << "orientation_z: " << orientation_z << std::endl;
  std::cout << "orientation_w: " << orientation_w << std::endl;
  }
  
  return 0;
}


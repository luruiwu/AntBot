// Generated by gencpp from file yun_bringup/Imu5220.msg
// DO NOT EDIT!


#ifndef YUN_BRINGUP_MESSAGE_IMU5220_H
#define YUN_BRINGUP_MESSAGE_IMU5220_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>


namespace yun_bringup
{
template <class ContainerAllocator>
struct Imu5220_
{
  typedef Imu5220_<ContainerAllocator> Type;

  Imu5220_()
    : angle(0.0)  {
    }
  Imu5220_(const ContainerAllocator& _alloc)
    : angle(0.0)  {
  (void)_alloc;
    }



   typedef float _angle_type;
  _angle_type angle;




  typedef boost::shared_ptr< ::yun_bringup::Imu5220_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::yun_bringup::Imu5220_<ContainerAllocator> const> ConstPtr;

}; // struct Imu5220_

typedef ::yun_bringup::Imu5220_<std::allocator<void> > Imu5220;

typedef boost::shared_ptr< ::yun_bringup::Imu5220 > Imu5220Ptr;
typedef boost::shared_ptr< ::yun_bringup::Imu5220 const> Imu5220ConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::yun_bringup::Imu5220_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::yun_bringup::Imu5220_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace yun_bringup

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': True, 'IsMessage': True, 'HasHeader': False}
// {'std_msgs': ['/opt/ros/indigo/share/std_msgs/cmake/../msg'], 'yun_bringup': ['/home/pepper/AntBot/src/yun/yun_bringup/msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::yun_bringup::Imu5220_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::yun_bringup::Imu5220_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::yun_bringup::Imu5220_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::yun_bringup::Imu5220_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::yun_bringup::Imu5220_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::yun_bringup::Imu5220_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::yun_bringup::Imu5220_<ContainerAllocator> >
{
  static const char* value()
  {
    return "2d11dcdbe5a6f73dd324353dc52315ab";
  }

  static const char* value(const ::yun_bringup::Imu5220_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x2d11dcdbe5a6f73dULL;
  static const uint64_t static_value2 = 0xd324353dc52315abULL;
};

template<class ContainerAllocator>
struct DataType< ::yun_bringup::Imu5220_<ContainerAllocator> >
{
  static const char* value()
  {
    return "yun_bringup/Imu5220";
  }

  static const char* value(const ::yun_bringup::Imu5220_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::yun_bringup::Imu5220_<ContainerAllocator> >
{
  static const char* value()
  {
    return "#角度Z\n\
float32 angle\n\
";
  }

  static const char* value(const ::yun_bringup::Imu5220_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::yun_bringup::Imu5220_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.angle);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct Imu5220_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::yun_bringup::Imu5220_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::yun_bringup::Imu5220_<ContainerAllocator>& v)
  {
    s << indent << "angle: ";
    Printer<float>::stream(s, indent + "  ", v.angle);
  }
};

} // namespace message_operations
} // namespace ros

#endif // YUN_BRINGUP_MESSAGE_IMU5220_H
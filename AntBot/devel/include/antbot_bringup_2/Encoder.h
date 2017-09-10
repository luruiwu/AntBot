// Generated by gencpp from file antbot_bringup_2/Encoder.msg
// DO NOT EDIT!


#ifndef ANTBOT_BRINGUP_2_MESSAGE_ENCODER_H
#define ANTBOT_BRINGUP_2_MESSAGE_ENCODER_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>


namespace antbot_bringup_2
{
template <class ContainerAllocator>
struct Encoder_
{
  typedef Encoder_<ContainerAllocator> Type;

  Encoder_()
    : encoder()  {
    }
  Encoder_(const ContainerAllocator& _alloc)
    : encoder(_alloc)  {
  (void)_alloc;
    }



   typedef std::vector<float, typename ContainerAllocator::template rebind<float>::other >  _encoder_type;
  _encoder_type encoder;




  typedef boost::shared_ptr< ::antbot_bringup_2::Encoder_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::antbot_bringup_2::Encoder_<ContainerAllocator> const> ConstPtr;

}; // struct Encoder_

typedef ::antbot_bringup_2::Encoder_<std::allocator<void> > Encoder;

typedef boost::shared_ptr< ::antbot_bringup_2::Encoder > EncoderPtr;
typedef boost::shared_ptr< ::antbot_bringup_2::Encoder const> EncoderConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::antbot_bringup_2::Encoder_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::antbot_bringup_2::Encoder_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace antbot_bringup_2

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': False, 'IsMessage': True, 'HasHeader': False}
// {'antbot_bringup_2': ['/home/pepper/AntBot/src/antbot/antbot_bringup_2/msg'], 'std_msgs': ['/opt/ros/indigo/share/std_msgs/cmake/../msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::antbot_bringup_2::Encoder_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::antbot_bringup_2::Encoder_<ContainerAllocator> const>
  : FalseType
  { };

template <class ContainerAllocator>
struct IsMessage< ::antbot_bringup_2::Encoder_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::antbot_bringup_2::Encoder_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::antbot_bringup_2::Encoder_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::antbot_bringup_2::Encoder_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::antbot_bringup_2::Encoder_<ContainerAllocator> >
{
  static const char* value()
  {
    return "f55857e2c2d7397d957a61ceadb6892a";
  }

  static const char* value(const ::antbot_bringup_2::Encoder_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0xf55857e2c2d7397dULL;
  static const uint64_t static_value2 = 0x957a61ceadb6892aULL;
};

template<class ContainerAllocator>
struct DataType< ::antbot_bringup_2::Encoder_<ContainerAllocator> >
{
  static const char* value()
  {
    return "antbot_bringup_2/Encoder";
  }

  static const char* value(const ::antbot_bringup_2::Encoder_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::antbot_bringup_2::Encoder_<ContainerAllocator> >
{
  static const char* value()
  {
    return "#编码器的数据\n\
float32[] encoder\n\
";
  }

  static const char* value(const ::antbot_bringup_2::Encoder_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::antbot_bringup_2::Encoder_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.encoder);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct Encoder_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::antbot_bringup_2::Encoder_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::antbot_bringup_2::Encoder_<ContainerAllocator>& v)
  {
    s << indent << "encoder[]" << std::endl;
    for (size_t i = 0; i < v.encoder.size(); ++i)
    {
      s << indent << "  encoder[" << i << "]: ";
      Printer<float>::stream(s, indent + "  ", v.encoder[i]);
    }
  }
};

} // namespace message_operations
} // namespace ros

#endif // ANTBOT_BRINGUP_2_MESSAGE_ENCODER_H

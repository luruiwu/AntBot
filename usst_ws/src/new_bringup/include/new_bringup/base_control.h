#ifndef BASE_CONTROL_H
#define BASE_CONTROL_H

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

class BaseControl{

private:
    int i;


public:

    void InitSerial();

    void SetSpeed();

    void ReadSerial();

    void PublishOdom();


};
#endif 
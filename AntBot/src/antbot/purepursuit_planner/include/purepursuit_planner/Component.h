/*! \class Component
 *  \file Component.h
 *	\author Robotnik Automation S.L.L
 *	\version 1.1.1
 *	\date 2014
 *  \brief Clase que definirá la estructura básica una serie de componentes. Adaptado para ROS
 * 	Define e implementa una serie de métodos y estructuras básicas que seguirán todos los componentes desarrollados por Robotnik
 * (C) 2013 Robotnik Automation, SLL
 *  All rights reserved.
 */

#ifndef __COMPONENT_H
#define __COMPONENT_H

#include <ros/ros.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//! Size of string for logging
#define DEFAULT_THREAD_DESIRED_HZ	40.0
#define DEFAULT_THREAD_PRIORITY     25

#define USEC_PER_SEC			1000000//微秒10^-6s
#define NSEC_PER_SEC			1000000000//纳秒10^-9s
#define Pi                      3.141592654

//! Defines return values for methods and functions
//定义返回值
enum ReturnValue{
        OK = 0,
        INITIALIZED,
        THREAD_RUNNING,
        ERROR = -1,
        NOT_INITIALIZED = -2,
        THREAD_NOT_RUNNING = -3,
        COM_ERROR = -4,
        NOT_ERROR = -5
};
//! Defines standard states of a component
//定义状态的标准组件
enum States{
        INIT_STATE,
        STANDBY_STATE,
        READY_STATE,
        EMERGENCY_STATE,
        FAILURE_STATE,
        SHUTDOWN_STATE,
        UNKNOWN_STATE
};

//! Global functions
//! Use of real time nanosleep
//全局函数，纳秒时间的延时
extern int clock_nanosleep(clockid_t __clock_id, int __flags,
			__const struct timespec *__req,
			struct timespec *__rem);

//! Common functions to all components
//! Normalize in sec.ns
//所有组件的公共函数
static inline void tsnorm(struct timespec *ts)
{
	while (ts->tv_nsec >= NSEC_PER_SEC) {
		ts->tv_nsec -= NSEC_PER_SEC;
		ts->tv_sec++;
	}
}
//! Normalize in rad 
//规范弧度
static inline void radnorm(double* radians)
{
	while (*radians >= (Pi)) {
		*radians -= 2.0 * Pi;
		}
	while (*radians <= (-Pi)) {
		*radians += 2.0 * Pi;
		}
}
//! Normalize in rad
//! Normalize between -PI and PI
static inline void radnorm2(double* radians)
{
	while (*radians >= (Pi)) {
		*radians -= 2.0 * Pi;
		}
	while (*radians <= (-Pi)) {
		*radians += 2.0 * Pi;
		}
}
//! Returns time difference in uS
//返回以us为单位的时间差
static inline long calcdiff(struct timespec t1, struct timespec t2)
{
	long diff;
	diff = USEC_PER_SEC * ((int) t1.tv_sec - (int) t2.tv_sec);
	diff += ((int) t1.tv_nsec - (int) t2.tv_nsec) / 1000;
	return diff;
}
//! Struct used for real time thread
//用于实时线程
struct thread_param{
	int prio;			// Thread priority level 0[min]-80[max]//线程优先级水平
	int clock;  		// CLOCK_MONOTONIC or CLOCK_REALTIME//时间
};
//! struct to store main data for a thread
//储存一个线程的主要数据
typedef struct thread_data{
    //! Priority and clock
    //优先级和时间
    struct thread_param pthreadPar;
    
    //! Contains the desired frequency of the thread
    //期望的线程频率
    double dDesiredHz;
    //! Contains the real frequency of the thread
    //线程的真实频率
    double dRealHz;
    //! Contains the id of the control thread
    //线程id
    pthread_t pthreadId;
}thread_data;

//! Main function to execute into the thread
//线程的入口函数
void *AuxControlThread(void *threadParam);

using namespace std;

//! Class component
class Component{
    friend void *AuxControlThread(void *threadParam);
	protected:
	
		//! Controls if has been initialized succesfully
		//是否初始化成功
		bool bInitialized;
		
		//! Controls the execution of the Component's thread
		//控制组件线程的执行
		bool bRunning;
		
		//! Contains data for the main thread
		//包含线程的主要数据
		thread_data threadData;
		
		//! Contains data for the secondary threads
		//包含第二线程的数据
		vector<thread_data> vThreadData;
		
		//! State of the Component
		//组件的状态
		
		States iState;
		//! State before
		//之前的状态
		
		States iOldState;
		
		//!	Almacenaremos el nombre del componente para personalizar el log a de cada una de las clases derivadas
		//组件的名字
		string sComponentName;
    private:
        //! Mutex to manage the access to the thread's vector
        //用来管理线程向量的存取的线程锁
        pthread_mutex_t mutexThread;
	public:
		//! Public constructor
		Component(double desired_hz);
		//! Public destructor
		virtual ~Component();
		//! Configures and initializes the component
		//! @return OK
		//! @return INITIALIZED if the component is already intialized
		//! @return ERROR
		virtual ReturnValue Setup();
		//! Closes and frees the reserved resources
		//! @return OK
		//! @return ERROR if fails when closes the devices
		//! @return RUNNING if the component is running
		//! @return NOT_INITIALIZED if the component is not initialized
		virtual ReturnValue ShutDown();
		//! Starts the control thread of the component and its subcomponents
		//! @return OK
		//! @return ERROR starting the thread
		//! @return RUNNING if it's already running
		//! @return NOT_INITIALIZED if it's not initialized
		virtual ReturnValue Start();
		//! Stops the main control thread of the component and its subcomponents
		//! @return OK
		//! @return ERROR if any error has been produced
		//! @return NOT_RUNNING if the main thread isn't running
		virtual ReturnValue Stop();
		
		//! Returns the general state of the Component
		//返回组件的状态
		States GetState();
		
		//! Returns the general state of the Component as string
		//返回组件的通用状态
		virtual char *GetStateString();
		
		//! Returns the general state as string
		
		char *GetStateString(States state);
		//! Method to get current update rate of the thread
		//! @return pthread_hz
		//获取线程的当前更新频率
		double GetUpdateRate();
		
	protected:
		//! All core component functionality is contained in this thread.
		//所有的核心功能组件都在这个线程中
		//!	All of the Component component state machine code can be found here.
		//所有的组件状态机器代码都可以在这里找到
		virtual void ControlThread();
		//! Opens used devices
		//! @return OK
		//! @return ERROR
		virtual ReturnValue Open();
		//! Closes used devices
		//! @return OK
		//! @return ERROR
		virtual ReturnValue Close();
		//! Allocates virtual memory if it's necessary
		//! @return OK
		virtual ReturnValue Allocate();
		//! Frees allocated memory previously
		//! @return OK
		virtual ReturnValue Free();
		//! Configures devices and performance of the component
		//! @return OK
		//! @return ERROR, if the configuration process fails
		virtual ReturnValue Configure();
		//! Actions performed on initial state
		virtual void InitState();
		//! Actions performed on standby state
		virtual void StandbyState();
		//! Actions performed on ready state
		virtual void ReadyState();
		//! Actions performed on the emergency state
		virtual void EmergencyState();
		//! Actions performed on Failure state
		virtual void FailureState();
		//! Actions performed in all states
		virtual void AllState();
		//! Switches between states
		virtual void SwitchToState(States new_state);
        //! Gets the data of the current thread
		ReturnValue GetThreadData(thread_data *data);
    public:
		//! Creates new tasks
		ReturnValue CreateTask(int prio, double frec, void *(*start_routine)(void*));


};

#endif

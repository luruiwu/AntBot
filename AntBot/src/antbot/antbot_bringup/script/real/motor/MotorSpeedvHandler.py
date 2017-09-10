#!/usr/bin/env python
# -*- coding: utf-8 -*-

linearrate=3022.0
angularrate=0.3
def odom_to_speed(cmd_twist_x , cmd_twist_y,cmd_twist_rotation):  
	global 	linearrate
	global angularrate
	# the radius of the robot is 0.07m  and the distance of  two wheels is 0.37m
	# vL=(2*v - w*L)/(2)  vR=2*v +w*L)/2
	#half length of car width length
	#l1=0.52/2
	#l2=0.85/2
	vx=cmd_twist_x
	vy=cmd_twist_y
	w=cmd_twist_rotation
	
	#v1=0-vx+vy-(l1+l2)*w
	#v2=0-vx-vy-(l1+l2)*w	
	#v3=0+vx-vy+(l1+l2)*w	
	#v4=0+vx+vy+(l1+l2)*w	
		
	v1=0+vx+vy-angularrate*w
	v2=0+vx-vy-angularrate*w	
	v3=0-vx-vy-angularrate*w	
	v4=0-vx+vy-angularrate*w	
	#v3=0
	#v4=0
	#R=0.079 #Radius /m 轮子的半径是7.9cm
	#jian su bi
	#Reduction=25 减速比是25
	#v=rpm / 10/60  *2*PI*R
	#v=rpm /600*2*3.1415*0.079
	#
	#w1=v1/R/(2*PI) * 60 *Reduction rpm/min 1/0.079/2/3.14*60*25=3022
	#w2=v1/R/(2*PI) * 60 *Reduction rpm/min
	#w3=v1/R/(2*PI) * 60 *Reduction rpm/min
	#w4=v1/R/(2*PI) * 60 *Reduction rpm/min
	
	w1=int(v1*linearrate)
	w2=int(v2*linearrate)
	w3=int(v3*linearrate)
	w4=int(v4*linearrate)
	
	return w1,w2,w3,w4
def speed_to_odom(speedArray):
	global 	linearrate
	v1=float(speedArray[1])/linearrate
	v2=float(speedArray[2])/linearrate
	v3=float(speedArray[3])/linearrate
	v4=float(speedArray[4])/linearrate
	vx=(v1+v2-v3-v4)/4
	vy=(v1+v4-v2-v3)/4
	vth=(v1+v2+v3+v4)/(4*angularrate)
	return vx,vy,vth
	#return the speed of the turtlebot 
	return v_x,v_y,v_th
def messageHandler(data):
	for i in data:
		if ((i>='0')and(i<='9'))or((i=='-')or(i=='.')):
			Num+=i
		
	if data.find('vl')>-1:
		return 'vl',Num
	elif data.find('vr')>-1:
		return 'vr',Num
	elif data.find('vh')>-1:
		return 'vh',Num
	else:
		return 0.0,0.0

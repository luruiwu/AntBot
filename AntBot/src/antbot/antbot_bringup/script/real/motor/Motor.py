#!/usr/bin/env python
import ModeBus

#write demo
#ADR CMD ADRHigh ADRLow DataHigh DataLow CRCLow CRCHigh
#01 06 02 00 00 64 89 99
#return message
#ADR CMD ADRHigh ADRLow DataHigh DataLow CRCLow CRCHigh
#01 06 02 00 00 64 89 99

#read demo
#ADR CMD ADRHigh ADRLow dataCountHigh dataCountLow CRCLow CRCHigh
#01 03 02 00 00 02 C5 B3
#return message
#ADR CMD byteCount datahigh datalow datahigh datalow CRCLow CRCHigh
#01 03 04 00 b1 1f 40 a3 
def Enable(num,ena):
	arr=[0x06,0x01,0xFC,0x00]
	arr.insert(0,num)
	arr.append(ena)
	newarr=ModeBus.CRC16(arr)
	newarr.insert(0,0xef)
	newarr.insert(0,0xff)
	newarr.insert(0,0xff)
	laststr=arrToStr(newarr)
	return laststr
def SetSpeed(num,speed):
	arr=[0x06,0x01,0x33]
	arr.insert(0,num)
	realspeed = speed&0xffff
	speedHigh=(realspeed>>8)&0x00ff
	speedLow=realspeed&0x00ff
	arr.append(speedHigh)
	arr.append(speedLow)
	newarr=ModeBus.CRC16(arr)
	newarr.insert(0,0xef)
	newarr.insert(0,0xff)
	newarr.insert(0,0xff)
	laststr=arrToStr(newarr)
	return laststr
def GetSpeed(num):
	arr=[0x03,0x06,0x00,0x00,0x01]
	arr.insert(0,num)
	newarr=ModeBus.CRC16(arr)
	newarr.insert(0,0xef)
	newarr.insert(0,0xff)
	newarr.insert(0,0xff)
	laststr=arrToStr(newarr)
	return laststr
def arrToStr(arr):
	str1=''
	for num in arr:
		str1+=chr(num)
	return str1



#!/usr/bin/env python 
def TwoBytes_ToInt(ch1,ch2):
	high=ord(ch1)
	low=ord(ch2)
	if high>0x80:
		num=(high-256)*256+low
	else:
		num=high*256+low
	return num
def TwoBytes_ToUnShort(ch1,ch2):
	high=ord(ch1)
	low=ord(ch2)
	num=high*256+low
	return num
def print16(arr):
		for i in arr:
			print('%#x'%ord(i))
def Int_ToTwoBytes(num):
	numHigh=(realspeed>>8)&0x00ff
	numLow=realspeed&0x00ff
	return numHigh,numLow

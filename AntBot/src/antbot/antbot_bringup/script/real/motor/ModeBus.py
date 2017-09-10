#!/usr/bin/env python 
def CRC16(arr):
    reg_crc=0xFFFF
    for num in arr:
        reg_crc^=num
        for i in range(0,8):
            if(reg_crc&0x01):
                reg_crc=(reg_crc>>1)^0xA001
            else:
                reg_crc=(reg_crc>>1)
    reg_crc_high=reg_crc>>8
    reg_crc_low=reg_crc&0x00ff
    #copy this arr to a new arr
    arrResult=arr[:]
    arrResult.append(reg_crc_low)
    arrResult.append(reg_crc_high)
    #represent the end of the message
    arrResult.append(0x00)	
    #the following is to change list to string 
    return arrResult


    

#!/usr/bin/env python
#coding=utf-8


import json

device_state = {
	"state":0,
	"power":100,
	"direction":0,
	"x":100,
	"y":200,
	"IR.state":0,
	"IR.hand":1
}


device_state_str = json.dumps(device_state)

print device_state_str


data = json.loads(device_state_str)

print data['state']
print data['power']
print data['direction']








#!/usr/bin/env python
#coding=utf-8


#回调函数示例
class Eggs:
	
	def __init__(self, spammer):
		self.spammer = spammer
		
	def OnSpam(self, a, b):
		self.spammer(a, b)
		
def SpamCallback(a, b):
	print 'spam and ' + a + ' and spam and spam and ' + b
	
	
if __name__=="__main__":
	
	e = Eggs(SpamCallback)
	e.OnSpam('green eggs', 'ham')
	

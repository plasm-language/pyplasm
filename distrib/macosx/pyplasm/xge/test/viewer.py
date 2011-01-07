from pyplasm import *

import os,sys
from math import *


# example of defining a new viewer in Python to get events
class PyViewer(Viewer):

	"""
	Questo non puo' funzionare sotto CPYTHON perche' e' multithread (in C)
	e quindi il global lock di python non funzionerebbe!
	"""

	def __init__(self):
		Viewer.__init__(self)

	def Keyboard(self,key,x,y):
		print "[Python] Keyboard",key,x,y
		Viewer.Keyboard(self,key,x,y)
	
	def Mouse(self,args):
		print "[Python] Mouse",args.type,args.button,args.x,args.y,args.delta
		Viewer.Mouse(self,args)
		
	def Resize(self,width,height):
		print "[Python] Resize",width,height
		Viewer.Resize(self,width,height)	
			
		

v=PyViewer()
v.Run()
print "[Python] Viewer has quit"

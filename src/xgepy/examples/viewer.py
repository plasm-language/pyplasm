from pyplasm import *

import os,sys
from math import *


# example of defining a new viewer in Python to get events
class PyViewer(GLCanvas):

	def __init__(self):
		GLCanvas.__init__(self)

	def onKeyboard(self,key,x,y):
		print "[Python] Keyboard",key,x,y
		GLCanvas.onKeyboard(self,key,x,y)
	
	def onMouseDown(self,button,x,y):
		print "[Python] onMouseDown",button,x,y
		GLCanvas.onMouseDown(self,button,x,y)
		
	def onMouseMove(self,button,x,y):
		print "[Python] onMouseMove",button,x,y
		GLCanvas.onMouseMove(self,button,x,y)
		
	def onMouseUp(self,button,x,y):
		print "[Python] onMouseUp",button,x,y
		GLCanvas.onMouseUp(self,button,x,y)				


v=PyViewer()
v.runLoop()
print "[Python] Viewer has quit"

import sys,types,math

# allow the oepngl imports to fail
try:
	import OpenGL.GL   
	Gl = OpenGL.GL
except:
	pass

try:
	import OpenGL.GLU
	Glu  = OpenGL.GLU
except:
	pass

try:
	import OpenGL.GLUT
	Glut = OpenGL.GLUT
except:
	pass

from .xgepy import *
from .fenvs import *







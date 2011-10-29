import sys,types,math

if sys.platform=='cli':
	import clr
	clr.AddReference('System')
	clr.AddReference('Trs.Sdk.Xge')
	clr.AddReference('mscorlib')
	import System 
	from Trs.Sdk.Xge import *
	BOOL_CODE_OR   = Plasm.BoolOpCode.BOOL_CODE_OR
	BOOL_CODE_AND = Plasm.BoolOpCode.BOOL_CODE_AND
	BOOL_CODE_DIFF = Plasm.BoolOpCode.BOOL_CODE_DIFF
	BOOL_CODE_XOR = Plasm.BoolOpCode.BOOL_CODE_XOR
	
else:
	import OpenGL.GL
	import OpenGL.GLU
	import OpenGL.GLUT
	Gl=OpenGL.GL
	Glu=OpenGL.GLU
	Glut=OpenGL.GLUT
	from xgepy import *
	BOOL_CODE_OR   = Plasm.BOOL_CODE_OR
	BOOL_CODE_AND = Plasm.BOOL_CODE_AND
	BOOL_CODE_DIFF = Plasm.BOOL_CODE_DIFF
	BOOL_CODE_XOR = Plasm.BOOL_CODE_XOR



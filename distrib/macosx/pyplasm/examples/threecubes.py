from pyplasm import *

def out():

	mycube=CUBOID([1,1,1])

	
	out=STRUCT([
		mycube,T(1)(1.2),
		mycube,T([1,2,3])([-0.6,-0.3,1]),
		mycube
	])

	return out

VIEW(out())



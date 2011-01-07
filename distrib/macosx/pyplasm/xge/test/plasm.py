from pyplasm import *


import os,sys
from math import *	


# view a 3d cube
Plasm.View(Plasm.cube(3,0,1),False)
	 
# view a 3d simplex
Plasm.View(Plasm.simplex(3),False)
	 
# mkpol in 2d
Plasm.View(Plasm.mkpol(2,[0,0, 1,0 ,1,1, 0,1],[[0,1,2],[2,0,3]]),False)

# mkpol in 3d
Plasm.View(Plasm.mkpol(3,[0,0,0,1,0,0,1,1,0,0,1,0, 0,0,1,1,0,1,1,1,1,0,1,1],[[0,1,2,3,4,5,6,7]]),False)

# get n-dim s-dim	 
assert Plasm.getSpaceDim(Plasm.cube(2))==2
assert Plasm.getPointDim(Plasm.cube(2))==2
	 
# calculate limits
assert Plasm.limits(Plasm.cube(3))==Boxf(Vecf(1,0,0,0),Vecf(1,1,1,1))
	 
# print debuggin infos
Plasm.Print(Plasm.cube(2))
	 
# add properties
assert Plasm.getProperty(Plasm.addProperty(Plasm.cube(2),"pname","pvalue"),"pname")=="pvalue"
assert Plasm.getProperty(Plasm.addProperty(Plasm.cube(2),"pname","pvalue"),"pname")=="pvalue"
	 
# example of structure
args=[Plasm.cube(0),Plasm.translate(Plasm.cube(1),3,1,1),Plasm.translate(Plasm.cube(2),3,1,2),Plasm.translate(Plasm.cube(3),3,1,3)]
Plasm.View(Plasm.Struct(args),False)
 
# duplicate an hpc
Plasm.View(Plasm.copy(Plasm.cube(3)),False)
	 
# example of join 
args=[Plasm.cube(0),Plasm.translate(Plasm.cube(1),3,1,1),Plasm.translate(Plasm.cube(2),3,1,2),Plasm.translate(Plasm.cube(3),3,1,3)]
Plasm.View(Plasm.join(args),False)
	 
# apply a transformation matrix (homo components first row/col)
vmat=Matf([1,0,0,0, 0,1,0,1, 0,0,1,1, 0,0,0,1])
Plasm.View(Plasm.transform(Plasm.cube(3),vmat,vmat.invert()),False)
	 
# scale an hpc
Plasm.View(Plasm.scale(Plasm.cube(3),Vecf(0.0, 1.0,2.0,3.0)),False)
	 
# translate an hpc
Plasm.View(Plasm.translate(Plasm.cube(3),Vecf(0.0, 1.0,2.0,3.0)),False)
	
# rotate an hpc
Plasm.View(Plasm.Struct([Plasm.cube(3),Plasm.rotate(Plasm.cube(3),3,1,2,pi)]),False)
	 
# embed in dimension
Plasm.View(Plasm.embed(Plasm.cube(2),3),False)
	 
# example of power (makes a 3d cube)
Plasm.View(Plasm.power(Plasm.cube(1),Plasm.cube(2)),False)
	 
# skeleyon
Plasm.View(Plasm.skeleton(Plasm.cube(3),1),False)
	 
# example of boolean intersection
Plasm.View(Plasm.boolop(BOOL_CODE_AND,[Plasm.cube(2,-0.5,0.5),Plasm.rotate(Plasm.cube(2,-0.5,0.5),2,1,2,pi/4)]),False)


from pyplasm import *


import os,sys
from math import *	


# view a 3d cube
Plasm.view(Plasm.cube(3,0,1))
	 
# view a 3d simplex
Plasm.view(Plasm.simplex(3))
	 
# mkpol in 2d
Plasm.view(Plasm.mkpol(2,[0,0, 1,0 ,1,1, 0,1],[[0,1,2],[2,0,3]]))

# mkpol in 3d
Plasm.view(Plasm.mkpol(3,[0,0,0,1,0,0,1,1,0,0,1,0, 0,0,1,1,0,1,1,1,1,0,1,1],[[0,1,2,3,4,5,6,7]]))

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
Plasm.view(Plasm.Struct(args))
 
# duplicate an hpc
Plasm.view(Plasm.copy(Plasm.cube(3)))
	 
# example of join 
args=[Plasm.cube(0),Plasm.translate(Plasm.cube(1),3,1,1),Plasm.translate(Plasm.cube(2),3,1,2),Plasm.translate(Plasm.cube(3),3,1,3)]
Plasm.view(Plasm.join(args))
	 
# apply a transformation matrix (homo components first row/col)
vmat=Matf([1,0,0,0, 0,1,0,1, 0,0,1,1, 0,0,0,1])
Plasm.view(Plasm.transform(Plasm.cube(3),vmat,vmat.invert()))
	 
# scale an hpc
Plasm.view(Plasm.scale(Plasm.cube(3),Vecf(0.0, 1.0,2.0,3.0)))
	 
# translate an hpc
Plasm.view(Plasm.translate(Plasm.cube(3),Vecf(0.0, 1.0,2.0,3.0)))
	
# rotate an hpc
Plasm.view(Plasm.Struct([Plasm.cube(3),Plasm.rotate(Plasm.cube(3),3,1,2,pi)]))
	 
# embed in dimension
Plasm.view(Plasm.embed(Plasm.cube(2),3))
	 
# example of power (makes a 3d cube)
Plasm.view(Plasm.power(Plasm.cube(1),Plasm.cube(2)))
	 
# skeleyon
Plasm.view(Plasm.skeleton(Plasm.cube(3),1))
	 
# example of boolean intersection
Plasm.view(Plasm.boolop(BOOL_CODE_AND,[Plasm.cube(2,-0.5,0.5),Plasm.rotate(Plasm.cube(2,-0.5,0.5),2,1,2,pi/4)]))


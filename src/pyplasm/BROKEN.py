from fenvs import *



if False:
  
  p = MKPOL([[[0,0]],[[1]],[[1]]])
  B = MINKOWSKI([  [-1.0/2.0,-1*math.sqrt(3.0/2.0)] , [-1.0/2.0,math.sqrt(3.0/2.0)] , [1,0] ])(p)
  vertices = [[0,0],[1,0],[1,0.5],[0.5,0.5],[0.5,1],[0,1]]
  pol1D = MKPOL([vertices,[[1,2],[2,3],[3,4],[4,5],[5,6],[6,1]],[[1],[2],[3],[4],[5],[6]]])
  pol2D = MKPOL( [vertices,[[1,2,3,4],[4,5,6,1]],[[1,2]]])
  Min0 = STRUCT([T([1,2])(v)(S([1,2])([0.1,0.1])(B)) for v in vertices ])
  Min1 = MINKOWSKI ([[0.1*-1.0/2.0,0.1*-1*math.sqrt(3.0/2.0)],[0.1*-1.0/2.0,0.1*math.sqrt(3.0/2.0)],[0.1*1,0.1*0]])(pol1D)
  Min2 = MINKOWSKI ([[0.1*-1.0/2.0,0.1*-1*math.sqrt(3.0/2.0)],[0.1*-1.0/2.0,0.1*math.sqrt(3.0/2.0)],[0.1*1,0.1*0]])(pol2D)
  A=Hpc.power(Min2,Q(0.05))
  B=Hpc.power(Min0,Q(0.70))
  C=Hpc.power(Min1,Q(0.05))
  VIEW(TOP([TOP([A,B]),C]) )


  #VIEW(POLAR(CUBOID([1,1,1])))  
  
  
	#VIEW(SOLIDIFY(STRUCT(AA(POLYLINE)([
	#	[[0,0],[4,2],[2.5,3],[4,5],[2,5],[0,3],[-3,3],[0,0]],
	#	[[0,3],[0,1],[2,2],[2,4],[0,3]],
	#	[[2,2],[1,3],[1,2],[2,2]]]))))


	#mypol1 = T([1,2])([-5,-5])(CUBOID([10,10]))
	#mypol2 = S([1,2])([0.9,0.9])(mypol1)
	#mypol3 = DIFF([mypol1,mypol2]);

	#VIEW(STRUCT([
	#	  EX([0,10])(mypol3), T(1)(12) ,
	#	  LEX([0,10])(mypol3), T(1)(25) ,
	#	   S(3)(3)(SEX([0,PI])(16)(mypol3))
	#	]))

  
  #((COLOR(RED)(Hpc.cube(3))).getProperty("RGBcolor")==("%s %s %s %s" % (1.0,0.0,0.0,1.0)))
  #((MATERIAL([1,0,0,1,  0,1,0,1,  0,0,0,1,  0,0,0,1,  100])(Hpc.cube(3))).getProperty("VRMLmaterial")==[1,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,100])
  #VIEW(TEXTURE(":images/gioconda.png")(CUBOID([1,1])))

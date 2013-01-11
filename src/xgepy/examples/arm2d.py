from pyplasm import *


#----------------------------------------------------
#-- Set of configurations in Working Space ----------
#----------------------------------------------------

def out():

	CSpath = BEZIERCURVE([[0,0,0],[90,0,0],[90,90,0],[90,90,90]])

	def Intervals(n): 
		return QUOTE([ 1.0/n for i in range(n) ])
	
	def Sampling (n):  
		return [[i/float(n)] for i in range(n+1)]

	def DOF (alpha): 
		return COMP([  T(2)(-18) , R([1,2])((PI/180) * alpha) ])

	rod = T([1,2])([-1,-19])(CUBOID([2,20]))

	def arm(alphas):
		alpha1,alpha2,alpha3=alphas
		return STRUCT([rod, DOF(alpha1), rod, DOF(alpha2), rod, DOF(alpha3), rod])

	Path = MAP(CSpath)(Intervals(18))

	ScaleFact = SIZE(1)(Path)

	ret=STRUCT(AA(arm)(AA(CSpath)(Sampling(16))))
	ret=S([1,2,3])([0.1,0.1,0.1])(ret) 
	return ret


VIEW(out())






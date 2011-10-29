from pyplasm import *
import numpy
import math


# //////////////////////////////////////////////////////////////////
class BallSet:

	#__________________________________________________________________________
	# costruttore
	def __init__(self,max_depth=8):
		self.max_depth = max_depth
		self.ballmap={}         # ogni bounding ball verra' inserita in un python dictionary 
		self.world_box=Box3f()	# questo e' il bounding box globale di tutte le bounding ball
		self.balls=[]           # mi tengo una copia delle balls per l'inserimento successivo

		
	#__________________________________________________________________________
	# aggiunge una bounding ball, call addBall(None) when all balls have been added
	def addBall(self,ball):
	
		# finished adding all the balls
		if ball is None:
		
			print "Creating loose octree with world_box",str(self.world_box)," max_depth",self.max_depth	
			self.octree=Octree(self.world_box,self.max_depth)
			 
			for ball in self.balls:
				R,C=ball.radius,ball.center
				box=Box3f(C-Vec3f([R,R,R]),C+Vec3f([R,R,R]))
				ID=self.octree.getNode(box).getId()
				# assign each ball to it's slot of the loose octree
				if not self.ballmap.has_key(ID): self.ballmap[ID]=[]
				self.ballmap[ID]+=[ball]
				
			return
			
		#assign an unique id
		ball.ID=len(self.balls)
		
		# is a bounding ball to add
		R,C=ball.radius,ball.center
		# note: the inner loose octree works with box instead of balls 
		self.world_box.add(C-Vec3f([R,R,R]))
		self.world_box.add(C+Vec3f([R,R,R]))
		self.balls+=[ball]

	#__________________________________________________________________________
	# test if two balls intersect
	def intersect(self,ball1,ball2):
		return (ball1.center - ball2.center).module()<=(ball1.radius+ball2.radius)	

	#__________________________________________________________________________
	# find all balls intersecting with the given ball
	def findIntersectingBalls(self,ball,node=None):

		# start with the root node of the octree	
		if node is None:
			node=self.octree.root
			
		R,C=ball.radius,ball.center
		box=Box3f(C-Vec3f([R,R,R]),C+Vec3f([R,R,R]))
			
		ret=[]
	
		# il nodo attuale ha un bounding box che non ha intersezione
		if not node.box.intersection(box).isValid(): 
			return ret
			
		# vedi se qualche ball ha intersezione
		id=node.getId()
		
		if self.ballmap.has_key(id):
			for B in self.ballmap[id]:
				if self.intersect(ball,B): ret+=[B]
				
		# vedi anche nei figli (se esistono)
		for I in range(0,8):
			child=node.getChild(I)
			if child: ret+=self.findIntersectingBalls(ball,child)

		return ret
		
		
# //////////////////////////////////////////////////////////////////
# esempio di creazione di un pol complex a partire dalle balls
# //////////////////////////////////////////////////////////////////
def MkPolOfBallSet(ballset,STEP=16,TOLERANCE=1e-6):

	# this are the points of a ball centered in the origin with radius 1
	UNITBALL=[]
	for alpha in numpy.arange(0,math.pi,math.pi/STEP):
		for beta in numpy.arange(0,2*math.pi,2*math.pi/STEP):
			UNITBALL+=[Vec3f(math.sin(alpha)*math.cos(beta),math.sin(alpha)*math.sin(beta),math.cos(alpha))]
			
	# general database for points
	pointdb=[]
	
	# init some structure
	for ball in ballset.balls: 
		ball.planes=[] # will contain oriented cutting planes
		ball.points=[] # will contains Point ID coming from pointdb (is an index!)
	
	# _________________  for each (bi,bj) intersecting find cutting plane h 
	# _________________  and add points of the intersecting circle to the point db
	for bi in ballset.balls:
		for bj in ballset.findIntersectingBalls(bi):
			if bj.ID<=bi.ID: continue # don't want repetitions
			ci,cj=bi.center,bj.center,
			ri,rj=bi.radius,bj.radius
			N=(cj-ci).normalize() # is the normal of the plane
			D=(ci-cj).module()    # is the distance between the two balls
			
			# see http://stackoverflow.com/questions/1406375/finding-intersection-points-between-3-spheres
			# P is the center of the intersecting circle, R is the radius of intersection circle
			dist_i=(ri*ri-rj*rj+D*D)/(2*D) # distance from ci to P
			dist_j=(rj*rj-ri*ri+D*D)/(2*D) # distance from cj to P
			P1,R1=ci+N*dist_i , math.sqrt(ri*ri-dist_i*dist_i)
			P2,R2=cj-N*dist_j , math.sqrt(rj*rj-dist_j*dist_j)
			P,R=(P1+P2)*0.5,(R1+R2)*0.5
			
			# oriented planes
			bi.planes+=[Plane4f(N*+1.0,P)]
			bj.planes+=[Plane4f(N*-1.0,P)]
			
			# find two vector orthogonal to N, this seems to be the more stable way to do it
			u=Vec3f();u.set(N.Abs().minidx(),1.0);u=u.cross(N).normalize()
			v=N.cross(u).normalize()
				  
			for angle in numpy.arange(0,2*math.pi,2*math.pi/STEP):
				pointOnCircle = P + u*(R*math.cos(angle)) + v*(R*math.sin(angle))
			
				# check (the points should be on both spheres!)
				assert ((pointOnCircle-ci).module()-ri)<0.01
				assert ((pointOnCircle-cj).module()-rj)<0.01

				pointdb+=[pointOnCircle.x,pointOnCircle.y,pointOnCircle.z]
				PID=len(pointdb)/3 - 1
				bi.points+=[PID]
				bj.points+=[PID]
				
			hull=[H for H in range(len(pointdb)/3)]
			# Plasm.View(Plasm.mkpol(3,pointdb,[hull]))
			
	# _________________ add points for all spheres only inside ball.splanes
	for ball in ballset.balls:
		for i in range(len(UNITBALL)):
			
			ballPoint=(UNITBALL[i]*ball.radius+ball.center)
			
			bInside=True
			for h in ball.planes:	
				bInside=h.getDistance(ballPoint)<=0
				if not bInside: break
				
			if bInside:
				pointdb+=[ballPoint.x,ballPoint.y,ballPoint.z]
				PID=len(pointdb)/3 - 1
				ball.points+=[PID]
				
	# _________________ create mkpol with points
	# .... todo...

	
		
# //////////////////////////////////////////////////////////////////
# esempio di utilizzo
# //////////////////////////////////////////////////////////////////

if __name__ == "__main__":

	ball1=Ball3f(1.0,Vec3f([2,2,0]))
	ball2=Ball3f(1.5,Vec3f([4,2,0]))

	ballset=BallSet()
	ballset.addBall(ball1)
	ballset.addBall(ball2)
	ballset.addBall(None)
	
	#print "Balls intersecting",str(ball1),ballset.findIntersectingBalls(ball1)
	#print "Balls intersecting",str(ball2),ballset.findIntersectingBalls(ball2)
	
	#other=Ball3f(1,Vec3f([4,4,4]))
	#print "Balls intersecting",str(other),ballset.findIntersectingBalls(other)
	
	# view 
	MkPolOfBallSet(ballset)


#///////////////////////////////
# esempio di qhull
#///////////////////////////////

if True:

	verbose=True
	tolerance=1e-6
	pointdim=3
	npoints=12
	points=[
			0.5,0.5,0, 0.5,0.5,1,
			0,0,0,  1,0,0,  2,1,0,  1,2,0,  0,1,0,
			0,0,1,  1,0,1,  2,1,1,  1,2,1,  0,1,1]
			
	ret=Graph_qhull(pointdim,npoints,points,tolerance,verbose)
	print ret

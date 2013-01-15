from pyplasm import *
import numpy
import math


# //////////////////////////////////////////////////////////////////
class BallSet:

	#__________________________________________________________________________
	# costruttore
	#__________________________________________________________________________
	
	def __init__(self,max_depth=8):
		self.max_depth  = max_depth  # puo' influenzare molto la velocita' di intersezione delle sfere
		self.ballmap    = {}         # ogni bounding ball verra' inserita in un python dictionary 
		self.world_box  = Box3f()	   # questo e' il bounding box globale di tutte le bounding ball
		self.balls      = []         # mi tengo una copia delle balls per l'inserimento successivo

	#__________________________________________________________________________
	# aggiunge una bounding ball, NOTE: call addBall(None) when all balls have been added
	#__________________________________________________________________________
	
	def addBall(self,ball):

		#assign an unique id
		ball.ID=len(self.balls)
		
		# is a bounding ball to add
		R,C=ball.radius,ball.center
		# note: the inner loose octree works with box instead of balls 
		self.world_box.add(C-Vec3f([R,R,R]))
		self.world_box.add(C+Vec3f([R,R,R]))
		self.balls+=[ball]
		
	#__________________________________________________________________________
	# to call when all balls has been added
	#__________________________________________________________________________
	def createOctree(self):
		
		print "Creating loose octree with world_box",str(self.world_box)," max_depth",self.max_depth	
		self.octree=Octree(self.world_box,self.max_depth)
		 
		for ball in self.balls:
			R,C=ball.radius,ball.center
			box=Box3f(C-Vec3f([R,R,R]),C+Vec3f([R,R,R]))
			ID=self.octree.getNode(box).getId()
			# assign each ball to it's slot of the loose octree
			if not self.ballmap.has_key(ID): self.ballmap[ID]=[]
			self.ballmap[ID]+=[ball]	

	#__________________________________________________________________________
	# test if two balls intersect
	#__________________________________________________________________________
	
	def intersect(self,ball1,ball2):
		return (ball1.center - ball2.center).module()<=(ball1.radius+ball2.radius)	

	#__________________________________________________________________________
	# find all balls intersecting with the given ball as input
	#__________________________________________________________________________
	
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
		
	#__________________________________________________________________________
	# return the points (as [Vec3f(x y z), Vec3f(x y z), ...) of unit balls
	#__________________________________________________________________________
	def getPointsOfUnitBall(self,STEP=16):
		ret=[]
		for alpha in numpy.arange(0,math.pi,math.pi/STEP):
			for beta in numpy.arange(0,2*math.pi,2*math.pi/STEP):
				X=math.sin(alpha)*math.cos(beta)
				Y=math.sin(alpha)*math.sin(beta)
				Z=math.cos(alpha)
				ret+=[Vec3f(X,Y,Z)]
		return ret
		
		
		
# //////////////////////////////////////////////////////////////////
# create a graph from balls
# IMPORTANT: I'm not handling the case when 3 balls intersects in the same area
#            here I assume that two balls intersect in a "independent" area!
#            otherwise you will have crashes or the balls does not merge correctly
# //////////////////////////////////////////////////////////////////

def buildGraphFromBalls(balls_,STEP=16,TOLERANCE=1e-6):

	# build the ball set
	ballset=BallSet()
	for ball in balls_:ballset.addBall(ball)
	ballset.createOctree() 
	
	# add some infos to balls
	for ball in ballset.balls: 
		ball.planes = [] # oriented cutting planes
		ball.pids   = [] # point id (as index for pointdb) needed for Graph.qhull
	
	# points of unit ball 
	unitpoints=ballset.getPointsOfUnitBall(STEP)
	
	# general database for points
	pointdb=[]
	
	# this is the Graph I'm creating (only 3d supported)
	g=Graph(3)	

	# find cutting plane between balls
	for bi in ballset.balls:
		for bj in ballset.findIntersectingBalls(bi):
		
			# don't want repetitions
			if bj.ID<=bi.ID: continue 
			ci,cj=bi.center,bj.center
			ri,rj=bi.radius,bj.radius
			N=(cj-ci).normalize() # is the normal of the plane
			D=(ci-cj).module()    # is the distance between the two balls
			
			# see http://stackoverflow.com/questions/1406375/finding-intersection-points-between-3-spheres
			#    P is the center of the intersecting circle
			#    R is the radius of intersection circle
			dist_i=(ri*ri-rj*rj+D*D)/(2*D) # distance from ci to P
			dist_j=(rj*rj-ri*ri+D*D)/(2*D) # distance from cj to P
			P1,R1 = ci+N*dist_i , math.sqrt(ri*ri-dist_i*dist_i)
			P2,R2 = cj-N*dist_j , math.sqrt(rj*rj-dist_j*dist_j)
			P,R=(P1+P2)*0.5,(R1+R2)*0.5
			
			# add oriented planes inside balls
			bi.planes+=[Plane4f(N*+1.0,P)]
			bj.planes+=[Plane4f(N*-1.0,P)]
			
			# find two vector orthogonal to N, this seems to be the more stable way to do it
			u=Vec3f();u.set(N.Abs().minidx(),1.0);u=u.cross(N).normalize()
			v=N.cross(u).normalize()
			
			# add points laying on the intersecting circle
			pids=[]
			
			for angle in numpy.arange(0,2*math.pi,2*math.pi/STEP):
			
				# point on circle
				pc = P + u*(R*math.cos(angle)) + v*(R*math.sin(angle))
			
				# check (the points should be on both spheres!)
				assert ((pc-ci).module()-ri)<0.1
				assert ((pc-cj).module()-rj)<0.1

				pids    += [len(pointdb)/3]
				pointdb += [pc.x,pc.y,pc.z]
				
			# later I will use this for Graph.qhull
			bi.pids+=pids
			bj.pids+=pids

			
	# add internal points for all balls
	# NOTE I'm going a little inside the ball because I want that boundary faces to be "stable"
	for ball in ballset.balls:
	
		pids=[]
		
		for i in range(len(unitpoints)):
		
			# inner point
			ip=(unitpoints[i]*ball.radius+ball.center)
			
			bInside=True
			for h in ball.planes:	
				bInside=h.getDistance(ip) < -1*TOLERANCE
				if not bInside: break
				
			if bInside:
				pids    += [len(pointdb)/3]
				pointdb += [ip.x,ip.y,ip.z]
				
		# needed later for Graph.qhull
		ball.pids+=pids


	# add all the points to the db
	for i in range(len(pointdb)/3):
		cell0d=g.addNode(0) 
		assert cell0d==i+1 # since I'm adding all points at the beginning...
		g.setVecf(cell0d,Vecf(1, pointdb[i*3+0],pointdb[i*3+1],pointdb[i*3+2]))
		
	#  need to keep track of 2d-faces in order to avoid the creation of "common 2d-faces" two times
	FACES={}	
		
	# now create cells
	for ball in ballset.balls:
	
		# add the full cell
		cell3d=g.addNode(3)	
	
		# this is the call to the new function which use QHULL internally
		bVerbose=False
		faces=Graph.qhull(3,pointdb,ball.pids,TOLERANCE,bVerbose)
		
		# failed for some reason? should not happen! if happens set bVerbose=TRUE and see
		# the message in the console
		assert len(faces)>0 
		
		# for each face I need to find its edges
		edges=[[] for i in range(len(faces))]
		
		for I in range(0,len(faces)):
			for J in range(I+1,len(faces)):
				edge=list(set(faces[I]) & set(faces[J]))
				
				# an edge must have exactly 2 vertices
				if len(edge)!=2: continue
				v0,v1=edge[0]+1,edge[1]+1 
				assert v0!=v1
				
				cell1d=g.findFirstCommonNode(v0,v1,Graph.DIRECTION_UP)
				
				# the 1-cell does not exist, create it...
				if cell1d==0:
					cell1d=g.addNode(1)
					g.addArch(v0,cell1d)
					g.addArch(v1,cell1d)
				# ... otherwise just use it
				else:
					pass
					
				# add the edge to both cells
				edges[I]+=[cell1d]
				edges[J]+=[cell1d]
		
		# safety check, each face need to be at least a triangle
		for edge in edges:
			assert len(edge)>=3
		
		# add the 2d-faces
		for I in range(0,len(faces)):
		
			KEY=tuple(sorted(faces[I]))
			
			# face already created...
			if FACES.has_key(KEY):
				cell2d=FACES[KEY]
			# ... otherwise create it
			else:
				cell2d=g.addNode(2)
				FACES[KEY]=cell2d
			
			# add connectivity (2-cell)<->(3-cell)
			g.addArch(cell2d,cell3d)
			
			# add connectivity
			#   (1-cell) <-> (2-cell)
			#   (3-cell) <-> (0.cell)
			for cell1d in edges[I]:
			
				if g.findArch(cell1d,cell2d,Graph.DIRECTION_UP)==0: 
					g.addArch(cell1d,cell2d)
				
				# add double connectivity
				v0=g.getFirstDwNode(cell1d)
				v1=g.getLastDwNode (cell1d)
				if g.findArch(cell3d,v0,Graph.DIRECTION_UP)==0: g.addArch(cell3d,v0)
				if g.findArch(cell3d,v1,Graph.DIRECTION_UP)==0: g.addArch(cell3d,v1)
			
			# order the face (it it fails it means that something wrong happened)
			bFixedOrder=g.orderFace2d(cell2d)
			assert bFixedOrder==True
			
			# find fitting plane
			g.setVecf(cell2d,g.getFittingPlane(cell2d))
			
	# remove unused vertices (can happen!)
	it=g.each(0) 
	unusedVertices=[]
	while not it.end():
		v0=it.getNode()
		it.goForward()
		if (g.getNDw(v0)+g.getNDw(v0))==0: 
			unusedVertices+=[v0]
	
	for v0 in unusedVertices:  g.remNode(v0)
	g.fixBoundaryFaceOrientation(0)
	
	#g.check()
	#g.Print()
	
	return g
	

# esempio di utilizzo
if __name__ == "__main__":
	ball1=Ball3f(1.0,Vec3f([2,2,0]))
	ball2=Ball3f(1.5,Vec3f([4,2,0]))
	g=buildGraphFromBalls([ball1,ball2])
	Plasm.view(Hpc(g))

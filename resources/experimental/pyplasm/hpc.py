

import math,itertools

# this is needed for compiting the hull
import scipy
import scipy.spatial

import sys,functools,math,copy
import numpy

import warnings
warnings.filterwarnings("ignore")

# /////////////////////////////////////////////////////////////////
def ComputeTriangleNormal(p0,p1,p2):
	p0=list(p0) + [0]*(3-len(p0))
	p1=list(p1) + [0]*(3-len(p1))
	p2=list(p2) + [0]*(3-len(p2))
	a=[p1[I]-p0[I] for I in range(3)]
	b=[p2[I]-p0[I] for I in range(3)]
	ret=numpy.cross(a,b)
	N=numpy.linalg.norm(ret)
	if N==0: N=1.0
	return [ret[I] / N for I in range(3)]

# /////////////////////////////////////////////////////////////////
# computeTetOrientation
#  see http://math.stackexchange.com/questions/183030/given-a-tetrahedron-how-to-find-the-outward-surface-normals-for-each-side)
#  see http://www.geuz.org/pipermail/gmsh/2012/007251.html

def GoodTetOrientation(v0,v1,v2,v3):
	v0=list(v0) + [0]*(3-len(v0))
	v1=list(v1) + [0]*(3-len(v1))
	v2=list(v2) + [0]*(3-len(v2))
	v3=list(v3) + [0]*(3-len(v3))
	a=[v3[I]-v1[I] for I in range(3)]
	b=[v2[I]-v1[I] for I in range(3)]
	c=[v0[I]-v1[I] for I in range(3)]
	n=numpy.cross(a,b)
	return numpy.dot(n,c)>0


# //////////////////////////////////////////////////////////////////////
# first element is homo coordinates
class BoxNd:
	
	# constructor
	def __init__(self,*arg):

		# dimension
		if len(arg)==1 and isinstance(arg[0],int):
			self.p1=[+sys.float_info.max] * arg[0]
			self.p2=[-sys.float_info.max] * arg[0]
			
		# two lists
		elif len(arg)==2 and isinstance(arg[0],(list, tuple)) and isinstance(arg[1],(list, tuple)):
			assert(len(arg[0])==len(arg[1]))
			self.p1=copy.copy(arg[0])
			self.p2=copy.copy(arg[1])
		 
		else:
			raise Exception("Invalid constructor call")
		
	# toList
	def toList(self):
		return [copy.copy(self.p1),copy.copy(self.p2)]

	# valid
	def valid(self):
		for i in range(self.dim()):
			if (self.p1[i]>self.p2[i]): return False # they can be equal!
		return True
		
	# __eq__
	def __eq__(self, other):
		return (isinstance(other, self.__class__) and self.__dict__==other.__dict__)
	
	# __ne__
	def __ne__(self, other):
		return not self.__eq__(other)  
		
	#fuzzyEqual
	def fuzzyEqual(self,other,Epsilon=1e-4):
		if not isinstance(other, self.__class__) or other.dim()!=self.dim(): 
			return False
		p1=[abs(a-b)<=Epsilon for a,b in zip(self.p1,other.p1)]
		p2=[abs(a-b)<=Epsilon for a,b in zip(self.p2,other.p2)]
		return (not False in p1) and (not False in p2)
		
	# repr
	def __repr__(self):
			return f"BoxNd({repr(self.p1)}, {repr(self.p2)})"
		
	# dim
	def dim(self):
		return len(self.p1)      
		
	# size
	def size(self):
		return [(To-From) for From,To in zip(self.p1,self.p2)]
		
	# center
	def center(self):
		return [0.5*(From+To) for From,To in zip(self.p1,self.p2)]
	
	# addPoint
	def addPoint(self,point):
		for i in range(self.dim()):
			self.p1[i]=min(self.p1[i],point[i])
			self.p2[i]=max(self.p2[i],point[i])
		return self
		
	# addPoint
	def addPoints(self,points):
		for point in points: self.addPoint(point)
		return self    
		
	# addBox
	def addBox(self,other):
		return self.addPoint(other.p1).addPoint(other.p2)
	
# ////////////////////////////////////////////////////////////
# first row/column are in omogeneous coordinates!
class MatrixNd:
	
	def __init__(self,arg=None):
		if (arg is None):  arg=0
		self.T=numpy.matrix(numpy.identity(arg,dtype=float)) if isinstance(arg,int) else numpy.matrix(arg,dtype=float)
		
	def __getitem__(self, *args):
		return self.T.__getitem__(*args)
	
	def __setitem__(self, *args):
		return self.T.__setitem__(*args)
		
	# __eq__
	def __eq__(self, other):
		return isinstance(other, self.__class__) and (self.T==other.T).all()
	
	# __ne__
	def __ne__(self, other):
		return not self.__eq__(other)  

	# repr
	def __repr__(self):
		
		if (self.isIdentity()):
			return 'MatrixNd('+str(self.dim())+')'  
		else:
			return 'MatrixNd('+repr(self.toList())+')'  
			
	# isIdentity
	def isIdentity(self):
		return numpy.array_equal(self.T,numpy.identity(self.dim()))
		
	# toList
	def toList(self):
		return self.T.tolist()
		
	# transpose
	def transpose(self):
		return MatrixNd(self.T.transpose())
		
	# invert
	def invert(self):
		return MatrixNd(numpy.linalg.inv(self.T))
		
	# dim
	def dim(self):
		assert(self.T.shape[0]==self.T.shape[1])
		return self.T.shape[0]     
			 
	# embed
	def embed(self,dim):
		if (self.dim()>=dim): return self
		ret=MatrixNd(dim)
		ret.T[0:self.dim(),0:self.dim()]=self.T
		return ret
		
	# adjoin
	def adjoin(self,other):
		M,N=self.dim(),other.dim()
		ret=MatrixNd(M+N-1)
	 
		ret[1:M,1:M]=self [1:,1:]
		for I in range(1,M): 
			ret[I,0]=self [I,0]
			ret[0,I]=self [0,I]
		 
		ret[M: ,M: ]=other[1:,1:] 
		for I in range(1,N): 
			ret[I+M-1,0]=other[I,0]
			ret[0,I+M-1]=other[0,I]
		
		return ret
		
	# __mul__
	def __mul__(self, other):
		assert (isinstance(other,MatrixNd))
		return MatrixNd(self.T * other.T) 
		
	# transformPoint
	def transformPoint(self,point):
		assert(isinstance(point,(list, tuple)))
		point=self.T * numpy.matrix([[1.0] + list(point) + [0.0]*(self.dim()-len(point)-1)]).transpose()
		return [point[i,0]/point[0,0] for i in range(1,self.dim())]
		
	# translate (example in 2D: vt([1.0,2.0]))
	@staticmethod
	def translate(vt):
		T=MatrixNd(len(vt)+1)
		for I in range(1,T.dim()): T[I,0]=vt[I-1]
		return T
		
	# scale (example in 2D: vs([1.0,2.0]))
	@staticmethod
	def scale(vs):
		T=MatrixNd(len(vs)+1)
		for I in range(1,T.dim()): T[I,I]=vs[I-1]
		return T

	# rotate (example in 2D i=1 j=2)
	@staticmethod
	def rotate(i,j,angle):
		T=MatrixNd(max([i,j])+1) 
		T[i,i]=+math.cos(angle) ; T[i,j]=-math.sin(angle)
		T[j,i]=+math.sin(angle) ; T[j,j]=+math.cos(angle)
		return T 


# ///////////////////////////////////////////////////////////////
class BuildMkPol:

	# constructor
	def __init__(self,points=[],hulls=None):

		self.db={}
		self.points=[]
		self.hulls=[]

		# default constructor
		if not points:
			return
		
		if hulls is None:
			hulls=[list(range(len(points)))]	

		pdim=len(points[0])
		self.db={}
		self.points=[]
		self.hulls=[]
		for hull in hulls:

			if not hull: 
				continue

			# special case for 1D
			if pdim==1:
				assert len(hull)>=2
				box = BoxNd(1).addPoints([points[I] for I in hull])
				self.addHull([box.p1, box.p2])

			# all other cases
			else:
				hull_points=[points[idx] for idx in hull]
				try:
					h=scipy.spatial.ConvexHull([points[idx] for idx in hull])
					hull_points=[tuple(h.points[idx]) for idx in h.vertices]
				except:
					pass
					
				self.addHull(hull_points)


	# addPoint
	def addPoint(self, p):
		p=tuple(p)
		idx=self.db.get(p,None)
		if idx is not None:
			return idx
		else:
			idx=len(self.db)
			self.db[p]=idx
			self.points.append(p)
			return idx

	# addHull
	def addHull(self,points):
		self.hulls.append([self.addPoint(p) for p in points])

	# dim
	def dim(self):
		if len(self.points)==0: return 0
		return len(self.points[0])
		
	# box
	def box(self):
		ret=BoxNd(self.dim())
		if self.points: ret.addPoints(self.points)
		return ret
		
	# __repr__
	def __repr__(self):
		return f"BuildMkPol(points={self.points}, hulls={self.hulls})"
		
	# toSimplicialForm
	def toSimplicialForm(self):

		if not self.points or not self.hulls:
			return self
		
		pdim=self.dim()
		
		# in 1d the ConvexHull is enough
		if pdim<=1: 
			return self
		
		ret=BuildMkPol()
		for hull in self.hulls:

			# already in simplicial form
			if len(hull)<=(pdim+1):
				ret.addHull([self.points[idx] for idx in hull])

			else:
				d=scipy.spatial.Delaunay([self.points[idx] for idx in hull])
				for simplex in d.simplices: 
					ret.addHull([d.points[idx] for idx in simplex])

		ret.__fixOrientation()
		return ret
	
	# fixOrientation
	def __fixOrientation(self):

		pdim=self.dim()
		if pdim!=2 and pdim!=3:
			return 

		# fix orientation of triangles on the plane (important for a coherent orientation)
		if pdim==2:
			fixed=[]
			for simplex in self.hulls:
				if len(simplex)==3:
					p0=self.points[simplex[0]]
					p1=self.points[simplex[1]]
					p2=self.points[simplex[2]]
					n=ComputeTriangleNormal(p0,p1,p2)
					if n[2]<0: simplex=[simplex[2],simplex[1],simplex[0]]
				fixed.append(simplex)
			self.hulls=fixed
			
		# fix orientation of tetahedra
		if pdim==3:
			fixed=[]
			for simplex in self.hulls:
				if len(simplex)==4:
					p0=self.points[simplex[0]]
					p1=self.points[simplex[1]]
					p2=self.points[simplex[2]]
					p3=self.points[simplex[3]]
					if not GoodTetOrientation(p0,p1,p2,p3): 
						simplex=[simplex[2],simplex[1],simplex[0],simplex[3]]
				fixed.append(simplex)
			self.hulls=fixed
		
		return self
	 
	# getBatches
	def getBatches(self):
		
		dim=self.dim()
					
		simplicial_form=self.toSimplicialForm()
		
		# export point, lines, triangles
		from pyplasm.viewer import GLBatch

		points    = GLBatch(GLBatch.POINTS   ); points   .vertices=[]
		lines     = GLBatch(GLBatch.LINES    ); lines    .vertices=[]
		triangles = GLBatch(GLBatch.TRIANGLES); triangles.vertices=[]; triangles.normals=[]
		
		for hull in simplicial_form.hulls:
			
			hull_dim=len(hull)
			
			if (hull_dim==1):
				p0=simplicial_form.points[hull[0]]; p0=list(p0) + [0.0]*(3-len(p0))
				points.vertices.append(p0) 
				
			elif (hull_dim==2):
				p0=simplicial_form.points[hull[0]]; p0=list(p0) + [0.0]*(3-len(p0))
				p1=simplicial_form.points[hull[1]]; p1=list(p1) + [0.0]*(3-len(p1)) 
				lines.vertices.append(p0);
				lines.vertices.append(p1)
				
			elif (hull_dim==3):
				p0=simplicial_form.points[hull[0]]; p0=list(p0) + [0.0]*(3-len(p0)) 
				p1=simplicial_form.points[hull[1]]; p1=list(p1) + [0.0]*(3-len(p1))
				p2=simplicial_form.points[hull[2]]; p2=list(p2) + [0.0]*(3-len(p2))
				n=ComputeTriangleNormal(p0,p1,p2)
				triangles.vertices.append(p0); triangles.normals.append(n)
				triangles.vertices.append(p1); triangles.normals.append(n)
				triangles.vertices.append(p2); triangles.normals.append(n)    

			elif (hull_dim==4):
				for T in [[0,1,3],[0,3,2],[0,2,1],[1,2,3]]:
					p0=simplicial_form.points[hull[T[0]]]; p0=list(p0) + [0.0]*(3-len(p0)) 
					p1=simplicial_form.points[hull[T[1]]]; p1=list(p1) + [0.0]*(3-len(p1))
					p2=simplicial_form.points[hull[T[2]]]; p2=list(p2) + [0.0]*(3-len(p2))
					n=ComputeTriangleNormal(p0,p1,p2)
					triangles.vertices.append(p0); triangles.normals.append(n)
					triangles.vertices.append(p1); triangles.normals.append(n)
					triangles.vertices.append(p2); triangles.normals.append(n)
					
			else:
				raise Exception("cannot handle geometry with dim>3")

		ret=[]
		if len(points   .vertices)>0: ret.append(points   )
		if len(lines    .vertices)>0: ret.append(lines    )
		if len(triangles.vertices)>0: ret.append(triangles)
		return ret

# ///////////////////////////////////////////////////////////////
class Hpc:
	
	# constructor
	def __init__(self,T=MatrixNd(0),childs=[],properties={}):
		assert(isinstance(childs,list))
		self.childs=childs   
		self.properties=properties    
		
		if len(childs)>0:
			Tdim=max([child.dim() for child in childs])+1
			self.T=T.embed(Tdim)
		else:
			self.T=T    
			
	@staticmethod
	def mkpol(points,hulls=None):
		return Hpc(MatrixNd(),[BuildMkPol(points,hulls)])

	@staticmethod
	def Struct(pols):
		return Hpc(MatrixNd(),pols)    
		
	@staticmethod
	def cube(dim,From=0.0,To=1.0):
		points=[[]]
		for i in range(dim): points=[p +[From] for p in points] + [p + [To] for p in points]
		return Hpc.mkpol(points)
				
	# simplex
	@staticmethod
	def simplex(dim):
		points=[[]]
		for i in range(dim): points=[p + [0.0] for p in points] + [[0.0]*i+[1.0]]
		return Hpc.mkpol(points)
		
	# join
	@staticmethod
	def join(pols):
		points=[]
		for T,properties,obj in Hpc(MatrixNd(),pols).toList():
			points.extend([T.transformPoint(p) for p in obj.points])
		return Hpc.mkpol(points)
		
	# quote
	@staticmethod 
	def quote(sequence):
		pos=0.0
		points,hulls=[[pos]],[]
		for value in sequence:
			next=pos+abs(value)
			points.append([next]) 
			if value>=0: hulls.append([len(points)-2,len(points)-1])
			pos=next
		return Hpc.mkpol(points,hulls)   
		
	# __repr__
	def __repr__(self):
		return "Hpc(" + repr(self.T)+","+repr(self.childs) + ("," + repr(self.properties) if self.properties else "") + ")"         
		
	# dim 
	def dim(self):
		return self.T.dim()-1     
		
	# toList
	def toList(self):
		ret=[]
		Tdim=self.dim()+1
		stack=[[MatrixNd(Tdim),{},self]]
		while (stack):
			T,properties,node=stack.pop()
			
			if isinstance(node,Hpc):
				
				# accumulate matrix
				T=T * node.T.embed(Tdim)
				
				# accumulate properties  
				if node.properties:
					properties=properties.copy()
					for key,value in node.properties.iteritems():
						properties[key]=value         
						
				# depth search
				for child in node.childs:
					stack.append([T,properties,child])
			else:
				
				# this is a final node
				ret.append([T,properties,node])
				
		return ret 
		
	# box
	def box(self):
		box=BoxNd(self.dim())
		for T,properties,obj in self.toList():
			box.addPoints([T.transformPoint(p) for p in obj.points])
		return box      
			
	# transform
	def transform(self,T):
		return Hpc(T,[self])
		
	# translate (example in 2D: vt([1.0,2.0]))
	def translate(self,vt):
		return Hpc(MatrixNd.translate(vt),[self])
		
	# scale (example in 2D: vs([1.0,2.0]))
	def scale(self,vs):
		return Hpc(MatrixNd.scale(vs),[self])

	# rotate (example in 2D i=1 j=2)
	def rotate(self,i,j,angle):
		return Hpc(MatrixNd.rotate(i,j,angle),[self])       

	# power
	@staticmethod 
	def power(a,b):
		
		childs=[]
		
		for T2,properties2,obj2 in b.toList():
			for T1,properties1,obj1 in a.toList():
				
				# combination of point coordinates
				points=[]
				for py in obj2.points:
					for px in obj1.points:
						points.append(tuple(px) + tuple(py))

				# combination of hulls
				hulls=[]
				nx,ny=len(obj1.points),len(obj2.points)
				for hy in obj2.hulls:   
					for hx in obj1.hulls:

						hull=[]
						for A in hx:
							for B in hy:
								hull.append(B*nx + A)

						#l=list(itertools.product(*[hx,hy]))
						#hull=[c[1]*nx + c[0] for c in l]

						hulls.append(hull)
	
				# combination of matrices
				T=T1.adjoin(T2)
				
				childs.append(Hpc(T,[BuildMkPol(points,hulls)]))
				
		return Hpc(MatrixNd(),childs)    
		
	# ukpol
	def ukpol(self):
		points,hulls=[],[]
		for T,properties,obj in self.toList():
			offset=len(points)
			points.extend([T.transformPoint(p) for p in obj.points])
			hulls.extend([[offset+idx for idx in hull] for hull in obj.hulls])
		return [points,hulls]    
		
	# view
	def view(self,title=""):

		# convert to batches
		batches=[]
		for T,properties,obj in self.toList():
			
			# homo must be in last position....
			T=T.embed(4)
			T3D=[
				T[1,1],T[1,2],T[1,3],T[1,0],
				T[2,1],T[2,2],T[2,3],T[2,0],
				T[3,1],T[3,2],T[3,3],T[3,0],
				T[0,1],T[0,2],T[0,3],T[0,0]]
		
			for batch in obj.getBatches():
				batch.prependTransformation(T3D)
				batch.writeProperties(properties)
				batches.append(batch)
				
		from pyplasm.viewer import GLView
		GLView(batches,title=title)
	 
	# mapFn
	def mapFn(self,fn):
		childs=[]
		for T,properties,obj in self.toList():
			obj=obj.toSimplicialForm()
			points=[fn(T.transformPoint(p)) for p in obj.points]
			hulls =obj.hulls
			childs.append(Hpc(MatrixNd(),[BuildMkPol(points,hulls)],properties))  
		ret=Hpc(MatrixNd(),childs)  
		return ret
		
	
	 # toBoundaryForm
	def toBoundaryForm(self):
		
		POINTDB={}
		faces=[]
		for T,properties,obj in self.toList():
			obj=obj.toSimplicialForm()
			
			points,hulls=[T.transformPoint(p) for p in obj.points],obj.hulls
			dim=len(points[0])
		
			mapped={}
			for P in range(len(points)):
				point = tuple(points[P])
				if not point in POINTDB: POINTDB[point]=len(POINTDB)
				mapped[P]=POINTDB[point]
				
			for hull in hulls:
				bfaces=[]
				if len(hull)<(dim+1): 
					bfaces=[range(len(hull))] # is already a boundary face.. probably from a previous bool op
					
				elif len(hull)==(dim+1):
					if   dim==0: bfaces=[[0]]
					elif dim==1: bfaces=[[0],[1]]
					elif dim==2: bfaces=[[0,1],[1,2],[2,0]]
					elif dim==3: bfaces=[[0,1,3],[0,3,2],[0,2,1],[1,2,3]]
					else: raise Exception("not supported")
				else:
					raise Exception("internal error")
					
				for face in bfaces:
					faces.append([mapped[hull[it]] for it in face])        
		
		num_occurrence={}
		for face in faces:
			key=tuple(sorted(face))
			if not key in num_occurrence: num_occurrence[key]=0
			num_occurrence[key]+=1
		faces=[face for face in faces if num_occurrence[tuple(sorted(face))]==1]
		
		points=[None]*len(POINTDB)
		for point,num in POINTDB.items(): 
			points[num]=point  
		
		ret=Hpc.mkpol(points,faces)
		return ret
		
	
import unittest

# //////////////////////////////////////////////////////
class Testing(unittest.TestCase):
		
	def testComputeNormal(self):
			self.assertEqual(ComputeTriangleNormal([0,0,0],[1,0,0],[0,1,0]), [0.0,0.0,1.0])
			self.assertEqual(ComputeTriangleNormal([0,0,0],[0,1,0],[0,0,1]), [1.0,0.0,0.0])
			self.assertEqual(ComputeTriangleNormal([0,0,0],[1,0,1],[1,0,0]), [0.0,1.0,0.0])

	def testGoodTet(self):
		self.assertTrue(GoodTetOrientation([0,0,0],[1,0,0],[0,1,0],[0,0,1]))
		self.assertFalse(GoodTetOrientation([0,0,0],[0,1,0],[1,0,0],[0,0,1]))

	def testBox(self):
		x1,y1,z1=1,2,3
		x2,y2,z2=4,5,6
		b=BoxNd([1.0, x1,y1,z1],[1.0, x2,y2,z2])
		self.assertEqual(b.dim(),4)
		self.assertEqual(b.p1,[1.0, x1,y1,z1])
		self.assertEqual(b.p2,[1.0, x2,y2,z2])
		self.assertTrue(b.valid())
		self.assertEqual(b,BoxNd(*b.toList()))
		self.assertEqual(b.size(),[0.0, x2-x1,y2-y1,z2-z1])
		self.assertEqual(b.center(),[1.0, (x1+x2)/2,(y1+y2)/2,(z1+z2)/2])
		x1,y1,z1=-1,-2,-3
		x2,y2,z2=10,20,30
		b.addPoint([1.0, x1,y1,z1])
		b.addPoint([1.0, x2,y2,z2])
		self.assertEqual(b,BoxNd([1.0, x1,y1,z1],[1.0, x2,y2,z2]))

	def testMat(self):
		T=MatrixNd(4)
		v=[[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]
		T[0,0]+=0.0
		self.assertEqual(T,MatrixNd(v))
		self.assertEqual(T.dim(),4)
		self.assertEqual(T[0,0],1.0)
		self.assertTrue(T.isIdentity())
		self.assertEqual(T.toList(),v)
		self.assertEqual(T.transpose(),T)
		self.assertEqual(T.invert(),T)
		self.assertEqual(T.embed(5),MatrixNd(5))
		self.assertEqual(T * T,T)

		# transform point does not want homo coordinate
		p=[2.0, 3.0, 4.0]
		self.assertEqual(T.transformPoint(p),p)
		self.assertEqual(MatrixNd.translate([10,20,30]),MatrixNd([[1,0,0,0],[10,1,0,0],[20,0,1,0],[30,0,0,1]]))
		self.assertEqual(MatrixNd.scale([10,20,30]),MatrixNd([[1,0,0,0],[0,10,0,0],[0,0,20,0],[0,0,0,30]]))
		angle=math.pi/2
		self.assertEqual(MatrixNd.rotate(1,2,angle),MatrixNd([[1,0,0],[0.0,math.cos(angle),-math.sin(angle)],[0.0,+math.sin(angle),math.cos(angle)]]))

	def testMkPol(self):

		# 1D
		points=[[0],[1],[2],   [8],[9],[10]]
		hulls=[[0,1,2],[3,4,5]]
		obj=BuildMkPol(points,hulls)
		self.assertEqual(obj.dim(),1)
		self.assertEqual(obj.box(),BoxNd([0],[10]))
		obj=obj.toSimplicialForm()
		self.assertEqual(len(obj.points),4)
		self.assertEqual(len(obj.hulls),2)
		self.assertEqual(obj.box(),BoxNd([0],[10]))

		# 2D
		points=[[0,0],[0.2,0.2],[1,0],[0.3,0.3],[1,1],[0.4,0.4],[0,1],[0.5,0.5],[0.2,0.8]]
		hulls=[list(range(len(points)))]
		obj=BuildMkPol(points,hulls)
		self.assertEqual(obj.dim(),2)
		self.assertEqual(obj.box(),BoxNd([0,0],[1,1]))
		obj=obj.toSimplicialForm()
		self.assertEqual(len(obj.points),4)
		self.assertEqual(len(obj.hulls),2)
		self.assertEqual(obj.box(),BoxNd([0,0],[1,1]))

		# 3D
		points=[
			[0.0, 0.0, 0.0],
			[1.0, 0.0, 0.0],
			[1.0, 1.0, 0.0],
			[0.0, 1.0, 0.0],

			[0.0, 0.0, 1.0],
			[1.0, 0.0, 1.0],
			[1.0, 1.0, 1.0],
			[0.0, 1.0, 1.0],			
			
			[0.1, 0.1, 0.1],
			[0.2, 0.2, 0.2],
			[0.3, 0.3, 0.3]]
		hulls=[list(range(len(points)))]

		obj=BuildMkPol(points,hulls)
		self.assertEqual(obj.dim(),3)
		self.assertEqual(obj.box(),BoxNd([0.0,0.0,0.0], [1.0, 1.0, 1.0]))
		obj=obj.toSimplicialForm()
		self.assertEqual(obj.dim(),3)
		self.assertEqual(obj.box(),BoxNd([0.0, 0.0, 0.0], [1.0, 1.0, 1.0]))
		self.assertEqual(len(obj.points),8)
		self.assertEqual(len(obj.hulls),6)

	def testHpc(self):
		points=[
			[0.0,0.0],
			[1.0,0.0],
			[1.0,1.0],
			[0.0,1.0],

			[0.2,0.2],
			[0.3,0.3],
			[0.4,0.4],
			[0.5,0.5],
			[0.2,0.8]]
		hulls=[list(range(len(points)))]		
		obj=Hpc.mkpol(points=points,hulls=hulls)
		self.assertEqual(obj.dim(),2)
		self.assertEqual(obj.box(),BoxNd([0,0],[1,1]))

		obj=Hpc.Struct([obj])
		self.assertEqual(obj.dim(),2)
		self.assertEqual(obj.box(),BoxNd([0,0],[1,1]))

		# cube
		obj=Hpc.cube(3, 0.0,1.0)
		self.assertEqual(obj.dim(),3)
		self.assertEqual(obj.box(),BoxNd([0,0,0],[1,1,1]))
		self.assertEqual(obj.box(),BoxNd([0,0,0],[1,1,1]))
		v=obj.toList()
		self.assertEqual(len(v),1)
		T,properties,obj=v[0]
		obj=obj.toSimplicialForm()
		self.assertEqual(obj.dim(),3)
		self.assertEqual(obj.box(),BoxNd([0.0, 0.0, 0.0], [1.0, 1.0, 1.0]))
		self.assertEqual(len(obj.points),8)
		self.assertEqual(len(obj.hulls),6)

		# simplex
		obj=Hpc.simplex(3)
		self.assertEqual(obj.dim(),3)
		self.assertEqual(obj.box(),BoxNd([0,0,0],[1,1,1]))
		v=obj.toList()
		self.assertEqual(len(v),1)
		T,properties,obj=v[0]
		obj=obj.toSimplicialForm()
		self.assertEqual(obj.dim(),3)
		self.assertEqual(obj.box(),BoxNd([0.0, 0.0, 0.0], [1.0, 1.0, 1.0]))
		self.assertEqual(len(obj.points),4)
		self.assertEqual(len(obj.hulls),1)

		# quote
		obj=Hpc.quote([1,-2,1])
		self.assertEqual(obj.box(),BoxNd([0.0],[4.0]))
		T,properties,obj=obj.toList()[0]
		self.assertEqual(len(obj.points),4)
		self.assertEqual(obj.hulls,[[0,1],[2,3]])

		# join
		obj=Hpc.join([
			Hpc.cube(2, 0.0, 1.0), 
			Hpc.cube(2, 0.0,1.0)])
		T,properties,obj=obj.toList()[0]
		obj=obj.toSimplicialForm()
		self.assertEqual(len(obj.points),4)
		self.assertEqual(len(obj.hulls),2)

		# transform
		obj=Hpc.cube(2,0.0,1.0).transform(MatrixNd(3))
		T,properties,obj=obj.toList()[0]
		self.assertEqual(T.dim(),3)
		self.assertEqual(obj.box(),BoxNd([0.0,0.0],[1.0,1.0]))

		# power
		obj=Hpc.power(Hpc.cube(2, 0.0,1.0),Hpc.cube(1, 10.0, 20.0))
		l=obj.toList()
		self.assertEqual(len(l),1)
		T,properties,obj=l[0]
		print(T,obj)
		self.assertEqual(len(obj.points),8)
		self.assertEqual(obj.box(),BoxNd([0,0,10],[1,1,20]))

		# not tested
		# ukpol
		# mapFn
		# toBoundaryFor



######################################################################
if __name__=="__main__":

	tests=Testing()
	tests.testComputeNormal()
	tests.testGoodTet()
	tests.testBox()
	tests.testMat()
	tests.testMat()
	tests.testMkPol()
	tests.testHpc()
	print("all test ok")

	





 
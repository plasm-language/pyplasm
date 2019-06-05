import math
import sys

from math            import *
from OpenGL.GL			import *
from OpenGL.GLU		import *

import glfw

# /////////////////////////////////////////////////////////////////////////
class Point3d:
	
	# constructor
	def __init__(self, x=0.0, y=0.0, z=0.0):
		self.x = float(x)
		self.y = float(y)
		self.z = float(z)
		
	# repr
	def __repr__(self):
		return 'Point3d(%f,%f,%f)' % (self.x,self.y,self.z)
	
	# __getitem__
	def __getitem__(self, key):
		return {0:self.x,1:self.y,2:self.z}[key]
			
	# __setitem__
	def __setitem__(self, key, value):
		if key==0: 
			self.x=value
		elif key==1:
			self.y=value
		elif key==2:
			self.z=value
		else:
			raise Exception("iternal error")
			
	# __len__
	def __len__(self):
		return 3
		
	# toList	
	def toList(self):
		return [self.x,self.y,self.z]			
	
	# __eq__
	def __eq__(self, other):
		if self.x == other.x and self.y == other.y and self.z == other.z: return True
		else: return False

	# __add__
	def __add__(self, o):
		return Point3d((self.x + o.x), (self.y + o.y), (self.z + o.z))
	
	# __sub__
	def __sub__(self, o):
		return Point3d((self.x - o.x), (self.y - o.y), (self.z - o.z))
	
	# __mul__
	def __mul__(self, vs):
		return Point3d(self.x*vs,self.y*vs,self.z*vs)
	
	# __rmul__
	def __rmul__(self, vs):
		return Point3d(self.x*vs,self.y*vs,self.z*vs)
	
	# __iadd__
	def __iadd__(self, o):
		self.x += o.x
		self.y += o.y
		self.z += o.z
		return self
	
	# __isub__
	def __isub__(self, o):
		self.x -= o.x
		self.y -= o.y
		self.z -= o.z
		return self
	
	# __neg__
	def __neg__(self):
		return Point3d(-self.x, -self.y, -self.z)
		
	# module
	def module2(self):
		return (self.x*self.x) + (self.y*self.y) + (self.z*self.z)	
			
	# module
	def module(self):
		return sqrt(self.module2())
	
	# normalized
	def normalized(self):
		len=self.module()
		return Point3d((self.x / len), (self.y / len), (self.z / len))

	# dotProduct
	def dotProduct(self, o):
		return (self.x * o.x) + (self.y * o.y) + (self.z * o.z)

	# crossProduct
	def crossProduct(self,v): 
		return Point3d(
			self.y * v.z - v.y * self.z, 
			self.z * v.x - v.z * self.x, 
			self.x * v.y - v.x * self.y)



# /////////////////////////////////////////////////////////////////////////
class Box3d:
	
	# constructor
	def __init__(self,p1=Point3d(0,0,0),p2=Point3d(0,0,0)):
		self.p1=p1
		self.p2=p2

	@staticmethod	
	def invalid():
		m,M=sys.float_info.min,sys.float_info.max
		return Box3d(Point3d(M,M,M),Point3d(m,m,m))
		 
	# valid
	def valid(self):
		return self.p1[0]<=self.p2[0] and self.p1[1]<=self.p2[1] and self.p1[2]<=self.p2[2] 
		
	# getPoints
	def getPoints(self):
		return [
			Point3d(self.p1.x,self.p1.y,self.p1.z),
			Point3d(self.p2.x,self.p1.y,self.p1.z),
			Point3d(self.p2.x,self.p2.y,self.p1.z),
			Point3d(self.p1.x,self.p2.y,self.p1.z),
			Point3d(self.p1.x,self.p1.y,self.p2.z),
			Point3d(self.p2.x,self.p1.y,self.p2.z),
			Point3d(self.p2.x,self.p2.y,self.p2.z),
			Point3d(self.p1.x,self.p2.y,self.p2.z),]
				
	# getPoint
	def getPoint(self,I):
		return self.getPoints()[I]
		
	# repr
	def __repr__(self):
		return 'Box3d('+repr(self.p1)+', '+repr(self.p2)+')'
				
	# size
	def size(self):
		return self.p2-self.p1
		
	# center
	def center(self):
		return 0.5*(self.p1+self.p2)
	
	# addPoint
	def addPoint(self,p):
		for i in range(3):
			self.p1[i]=min(self.p1[i],p[i])
			self.p2[i]=max(self.p2[i],p[i])
		return self
		
	# addPoint
	def addPoints(self,points):
		for point in points: 
			self.addPoint(point)
		return self		
		
	# addBox
	def addBox(self,other):
		self.addPoint(other.p1)
		self.addPoint(other.p2)
		return self
	
# ///////////////////////////////////////////////////////////////////
class Matrix(object):

	def __init__(self, mat=[
				1.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0]):
		self.mat = mat 
				
	# toList	
	def toList(self):
		return [self.mat[I] for I in range(16)]

	# __repr__
	def __repr__(self):
		return 'Matrix(%s)' % (",".join([str(it) for it in self.mat]),)

	# __mul__
	def __mul__(self, other):
		
		if isinstance(other,float):
			return Matrix([self.mat[I]*other for I in range(16)])
			
		if isinstance(other,Point3d):
			p=other
			X=(self.mat[ 0]*(p.x)+self.mat[ 1]*(p.y)+self.mat[ 2]*(p.z)+ self.mat[ 3]*(1.0))
			Y=(self.mat[ 4]*(p.x)+self.mat[ 5]*(p.y)+self.mat[ 6]*(p.z)+ self.mat[ 7]*(1.0))
			Z=(self.mat[ 8]*(p.x)+self.mat[ 9]*(p.y)+self.mat[10]*(p.z)+ self.mat[11]*(1.0))
			W=(self.mat[12]*(p.x)+self.mat[13]*(p.y)+self.mat[14]*(p.z)+ self.mat[15]*(1.0))
			if (W==0): W=1.0 
			return Point3d(X/W,Y/W,Z/W)
		
		A=self.mat
		B=other.mat
		return Matrix([
			A[ 0]*B[ 0]+A[ 1]*B[ 4]+A[ 2]*B[ 8]+A[ 3]*B[12],
			A[ 0]*B[ 1]+A[ 1]*B[ 5]+A[ 2]*B[ 9]+A[ 3]*B[13],
			A[ 0]*B[ 2]+A[ 1]*B[ 6]+A[ 2]*B[10]+A[ 3]*B[14],
			A[ 0]*B[ 3]+A[ 1]*B[ 7]+A[ 2]*B[11]+A[ 3]*B[15], 
			A[ 4]*B[ 0]+A[ 5]*B[ 4]+A[ 6]*B[ 8]+A[ 7]*B[12], 
			A[ 4]*B[ 1]+A[ 5]*B[ 5]+A[ 6]*B[ 9]+A[ 7]*B[13], 
			A[ 4]*B[ 2]+A[ 5]*B[ 6]+A[ 6]*B[10]+A[ 7]*B[14], 
			A[ 4]*B[ 3]+A[ 5]*B[ 7]+A[ 6]*B[11]+A[ 7]*B[15], 
			A[ 8]*B[ 0]+A[ 9]*B[ 4]+A[10]*B[ 8]+A[11]*B[12], 
			A[ 8]*B[ 1]+A[ 9]*B[ 5]+A[10]*B[ 9]+A[11]*B[13], 
			A[ 8]*B[ 2]+A[ 9]*B[ 6]+A[10]*B[10]+A[11]*B[14], 
			A[ 8]*B[ 3]+A[ 9]*B[ 7]+A[10]*B[11]+A[11]*B[15], 
			A[12]*B[ 0]+A[13]*B[ 4]+A[14]*B[ 8]+A[15]*B[12], 
			A[12]*B[ 1]+A[13]*B[ 5]+A[14]*B[ 9]+A[15]*B[13], 
			A[12]*B[ 2]+A[13]*B[ 6]+A[14]*B[10]+A[15]*B[14], 
			A[12]*B[ 3]+A[13]*B[ 7]+A[14]*B[11]+A[15]*B[15]])
			
	# transpose
	def transpose(self):
		return Matrix([
			self.mat[ 0],self.mat[ 4],self.mat[ 8],self.mat[12],
			self.mat[ 1],self.mat[ 5],self.mat[ 9],self.mat[13],
			self.mat[ 2],self.mat[ 6],self.mat[10],self.mat[14],
			self.mat[ 3],self.mat[ 7],self.mat[11],self.mat[15],
		])

	# inverted
	def inverted(self):
		m=self.transpose().mat
		inv=[0.0]*16
		inv[ 0] =  m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]+ m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10]
		inv[ 4] = -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]- m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10]
		inv[ 8] =  m[4]*m[ 9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]+ m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[ 9]
		inv[12] = -m[4]*m[ 9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]- m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[ 9]
		inv[ 1] = -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]- m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10]
		inv[ 5] =  m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]+ m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10]
		inv[ 9] = -m[0]*m[ 9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]- m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[ 9]
		inv[13] =  m[0]*m[ 9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]+ m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[ 9]
		inv[ 2] =  m[1]*m[ 6]*m[15] - m[1]*m[ 7]*m[14] - m[5]*m[2]*m[15]+ m[5]*m[3]*m[14] + m[13]*m[2]*m[ 7] - m[13]*m[3]*m[ 6]
		inv[ 6] = -m[0]*m[ 6]*m[15] + m[0]*m[ 7]*m[14] + m[4]*m[2]*m[15]- m[4]*m[3]*m[14] - m[12]*m[2]*m[ 7] + m[12]*m[3]*m[ 6]
		inv[10] =  m[0]*m[ 5]*m[15] - m[0]*m[ 7]*m[13] - m[4]*m[1]*m[15]+ m[4]*m[3]*m[13] + m[12]*m[1]*m[ 7] - m[12]*m[3]*m[ 5]
		inv[14] = -m[0]*m[ 5]*m[14] + m[0]*m[ 6]*m[13] + m[4]*m[1]*m[14]- m[4]*m[2]*m[13] - m[12]*m[1]*m[ 6] + m[12]*m[2]*m[ 5]
		inv[ 3] = -m[1]*m[ 6]*m[11] + m[1]*m[ 7]*m[10] + m[5]*m[2]*m[11]- m[5]*m[3]*m[10] - m[ 9]*m[2]*m[ 7] + m[ 9]*m[3]*m[ 6]
		inv[ 7] =  m[0]*m[ 6]*m[11] - m[0]*m[ 7]*m[10] - m[4]*m[2]*m[11]+ m[4]*m[3]*m[10] + m[ 8]*m[2]*m[ 7] - m[ 8]*m[3]*m[ 6]
		inv[11] = -m[0]*m[ 5]*m[11] + m[0]*m[ 7]*m[ 9] + m[4]*m[1]*m[11]- m[4]*m[3]*m[ 9] - m[ 8]*m[1]*m[ 7] + m[ 8]*m[3]*m[ 5]
		inv[15] =  m[0]*m[ 5]*m[10] - m[0]*m[ 6]*m[ 9] - m[4]*m[1]*m[10]+ m[4]*m[2]*m[ 9] + m[ 8]*m[1]*m[ 6] - m[ 8]*m[2]*m[ 5]
		det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12]
		return (Matrix(inv) * (1.0/det)) .transpose() if det else Matrix()
		
	# toQuaternion
	def toQuaternion(self):
		
		kRot=[[self.mat[ 0],self.mat[ 1],self.mat[ 2]],
				[self.mat[ 4],self.mat[ 5],self.mat[ 6]],
				[self.mat[ 8],self.mat[ 9],self.mat[10]]]
				
		fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2]
		
		if fTrace>0.0:
			fRoot = math.sqrt(fTrace + 1.0)
			w = 0.5*fRoot
			fRoot = 0.5/fRoot
			x = (kRot[2][1]-kRot[1][2])*fRoot
			y = (kRot[0][2]-kRot[2][0])*fRoot
			z = (kRot[1][0]-kRot[0][1])*fRoot
			return Quaternion(w,x,y,z)
		else:
			s_iNext = [1, 2, 0]
			i = 0
			if ( kRot[1][1] > kRot[0][0] ) : i = 1
			if ( kRot[2][2] > kRot[i][i] ) : i = 2
			j = s_iNext[i]
			k = s_iNext[j]
			fRoot = math.sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0)
			Q = [0,0,0]
			Q[i] = 0.5 * fRoot
			fRoot = 0.5/fRoot
			w = (kRot[k][j]-kRot[j][k])*fRoot
			Q[j] = (kRot[j][i]+kRot[i][j])*fRoot
			Q[k] = (kRot[k][i]+kRot[i][k])*fRoot
			return Quaternion(w,Q[0],Q[1],Q[2])			
		
	# translate
	@staticmethod	
	def translate(vt):
		return Matrix([
				1.0, 0.0, 0.0, vt[0],
				0.0, 1.0, 0.0, vt[1],
				0.0, 0.0, 1.0, vt[2],
				0.0, 0.0, 0.0, 1.0])	
				
	# scale
	@staticmethod	
	def scale(vs):
		return Matrix([
				vs[0], 0.0, 0.0, 0.0,
				0.0, vs[1], 0.0, 0.0,
				0.0, 0.0, vs[2], 0.0,
				0.0, 0.0, 0.0, 1.0])					
		
	# lookAt
	@staticmethod	
	def lookAt(eye, center, up):
		forward=(center-eye).normalized()
		side   = forward.crossProduct(up).normalized()
		up     =side.crossProduct(forward)
		m = Matrix([
			side[0],up[0],-forward[0], 0.0,
			side[1],up[1],-forward[1], 0.0,
			side[2],up[2],-forward[2], 0.0,
			0.0,0.0,0.0,1.0
		])
		return m.transpose() * Matrix.translate(-1*eye)
		
	# getLookAt
	def getLookAt(self):
		vmat=self.inverted().mat
		pos=Point3d(  vmat[3], vmat[7], vmat[11])
		dir=Point3d( -vmat[2],-vmat[6],-vmat[10]).normalized()
		vup=Point3d(  vmat[1], vmat[5], vmat[ 9]).normalized()
		return [pos,dir,vup]		
		
	# perspective
	@staticmethod	
	def perspective(fovy, aspect, zNear, zFar):
	  radians =  math.radians(fovy/2.0)
	  cotangent = math.cos(radians) / math.sin(radians)
	  m=Matrix()
	  m.mat[ 0] = cotangent / aspect
	  m.mat[ 5] = cotangent
	  m.mat[10] = -(zFar + zNear) / (zFar - zNear)
	  m.mat[11] = -1
	  m.mat[14] = -2 * zNear * zFar / (zFar - zNear)
	  m.mat[15] = 0
	  return m.transpose()
		
	
# ///////////////////////////////////////////////////////////////////////////////
class Quaternion:

	# constructor
	def __init__(self,fW =1.0,fX=0.0,fY=0.0,fZ=0.0):
		self.w = fW
		self.x = fX
		self.y = fY
		self.z = fZ

	# fromAxisAndAngle
	@staticmethod	
	def fromAxisAndAngle(axis,angle):
		axis=axis.normalized()
		halfangle = 0.5*angle
		fSin = sin(halfangle)
		return Quaternion(cos(halfangle),fSin*axis.x,fSin*axis.y,fSin*axis.z)

	# toMatrix
	def toMatrix(self):
		fTx  = 2.0*self.x
		fTy  = 2.0*self.y
		fTz  = 2.0*self.z
		fTwx = fTx*self.w
		fTwy = fTy*self.w
		fTwz = fTz*self.w
		fTxx = fTx*self.x
		fTxy = fTy*self.x
		fTxz = fTz*self.x
		fTyy = fTy*self.y
		fTyz = fTz*self.y
		fTzz = fTz*self.z
		return Matrix([
			1.0-(fTyy+fTzz),    (fTxy-fTwz),    (fTxz+fTwy),0.0,
			    (fTxy+fTwz),1.0-(fTxx+fTzz),    (fTyz-fTwx),0.0,
			    (fTxz-fTwy),    (fTyz+fTwx),1.0-(fTxx+fTyy),0.0,
			0.0,0.0,0.0,1.0])

	# getAxis
	def getAxis(self):
		len = self.x*self.x+self.y*self.y+self.z*self.z
		if len > 0.0:
			ilen = 1.0/sqrt(len)
			return Point3d(x*ilen,y*ilen,z*ilen)
		else:
			return Point3d(1,0,0)

	# getAngle
	def getAngle(self):
		len = self.x*self.x+self.y*self.y+self.z*self.z
		w=self.w
		if w<-1:w=-1
		if w>+1:w=+1
		return 2.0*acos(w) if len>0.0 else 0.0

	# __mul__
	def __mul__(self, other):
		if isinstance(other,float):
			return Quaternion(other*self.w,other*self.x,other*self.y,other*self.z)
		else:
			return Quaternion(
				self.w * other.w - self.x * other.x - self.y * other.y - self.z * other.z,
				self.w * other.x + self.x * other.w + self.y * other.z - self.z * other.y,
				self.w * other.y + self.y * other.w + self.z * other.x - self.x * other.z,
				self.w * other.z + self.z * other.w + self.x * other.y - self.y * other.x)	
				
				



# ////////////////////////////////////////////
class GLVertexBuffer:

	destroy_list = []	

	# constructor
	def __init__(self, data):
		
		self.id = glGenBuffers(1)
		if not isinstance(data[0],float):
			raise Exception("internal error")

		nbytes=len(data)*4 # 4 is sizeof(float)
		
		glBindBuffer (GL_ARRAY_BUFFER, self.id)
		import numpy
		glBufferData (GL_ARRAY_BUFFER, nbytes, numpy.array(data, dtype="float32"), GL_STATIC_DRAW)
		glBindBuffer(GL_ARRAY_BUFFER, 0)
	 
	# destructor
	def __del__(self):
		GLVertexBuffer.destroy_list.append(self.id)


# ////////////////////////////////////////////
class Texture:
	
	# constructor
	def __init__(self):
		self.bpp=0
		self.width=0
		self.height=0
		self.c_buffer=None

# ////////////////////////////////////////////
class GLTexture:

	destroy_list = []

	# constructor
	def __init__(self, texture):
		self.id=glGenTextures(1)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
		glBindTexture(GL_TEXTURE_2D, self.id)
		format={8:GL_LUMINANCE,24:GL_RGB,32:GL_RGBA}[texture.bpp]
		gluBuild2DMipmaps(GL_TEXTURE_2D,bpp/8,texture.width, texture.height,format,GL_UNSIGNED_BYTE,texture.c_buffer)
		glBindTexture(GL_TEXTURE_2D,0)
	 
	# destructor
	def __del__(self):
		GLTexture.destroy_list.append(self.id)



# ////////////////////////////////////////////
class GLMesh:
	
	POINTS    = 0x0000
	LINES     = 0x0001
	TRIANGLES  = 0x0004

	# constructor
	def __init__(self,primitive=POINTS):
		
		self.primitive=primitive
		self.T=Matrix() 
		self.texture=None
		self.vertices=None
		self.normals=None
		self.colors=None
		self.texturecoords=None
		
		self.gpu_texture 		 =None
		self.gpu_vertices 	=None
		self.gpu_normals 		 =None
		self.gpu_colors 		 =None 	
		self.gpu_texturecoords=None 
		
		self.ambient	=(0.2,0.2,0.2,1.0) 
		self.diffuse	=(0.8,0.8,0.8,1.0) 
		self.specular =(0.1,0.1,0.1,1.0)
		self.emission =(0.0,0.0,0.0,0.0)
		self.shininess=100.0
		

	# computeTriangleNormal
	@staticmethod	
	def computeTriangleNormal(p0,p1,p2):
		return (p1-p0).crossProduct(p2-p0).normalized()
		
	# computeNormals
	def computeNormals(self):
		if not self.normals and self.primitive==GLMesh.TRIANGLES:
			self.normals=[]
			for I in range(0,len(self.vertices),9):
				p0=Point3d(self.vertices[I+0],self.vertices[I+1],self.vertices[I+2])
				p1=Point3d(self.vertices[I+3],self.vertices[I+4],self.vertices[I+5])
				p2=Point3d(self.vertices[I+6],self.vertices[I+7],self.vertices[I+8])
				n=GLMesh.computeTriangleNormal(p0,p1,p2)
				self.normals+=[n.x,n.y,n.z]*3
		
	# getDimension	
	def getDimension(self):
		return 3	
		
	# cuboid
	@staticmethod	
	def Cuboid(box=Box3d(Point3d(0,0,0),Point3d(1,1,1))):
		points=box.getPoints()
		vertices=[]
		for face in [[0, 1, 2, 3],[3, 2, 6, 7],[7, 6, 5, 4],[4, 5, 1, 0],[5, 6, 2, 1],[7, 4, 0, 3]]:
			p0,p1,p2,p3 = points[face[0]],points[face[1]],points[face[2]],points[face[3]]
			p0,p1,p2,p3 = p3,p2,p1,p0
			vertices+=[p0.x,p0.y,p0.z,p1.x,p1.y,p1.z,p2.x,p2.y,p2.z]
			vertices+=[p0.x,p0.y,p0.z,p2.x,p2.y,p2.z,p3.x,p3.y,p3.z]
			
		ret=GLMesh(GLMesh.TRIANGLES)
		ret.vertices = vertices
		ret.computeNormals()
		return ret
			
	# getBoundingBox
	def getBoundingBox(self):
		box=Box3d.invalid()
		for I in range(0,len(self.vertices),3):
			point=Point3d(self.vertices[I+0],self.vertices[I+1],self.vertices[I+2])
			box.addPoint(point)
		return box
		
	# prependTransformation
	def prependTransformation(self,T):
		self.T=T * self.T
		
	# glRender
	def glRender(self):
		
		if (self.colors is None):
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT	, self.ambient)
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE	, self.diffuse)
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , self.specular)
			glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, self.shininess)
			if (self.diffuse[3]<1):
				glEnable(GL_BLEND)
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
			 
		if (not self.vertices is None):
			if self.gpu_vertices is None: 
				self.gpu_vertices=GLVertexBuffer(self.vertices)
			glBindBuffer(GL_ARRAY_BUFFER, self.gpu_vertices.id)
			glVertexPointer(3,GL_FLOAT,0,None)
			glBindBuffer(GL_ARRAY_BUFFER,0)	
			glEnableClientState(GL_VERTEX_ARRAY)
	
		if (not	self.normals	is None):
			if self.gpu_normals is None: 
				self.gpu_normals=GLVertexBuffer(self.normals)
			glBindBuffer(GL_ARRAY_BUFFER, self.gpu_normals.id)
			glNormalPointer(GL_FLOAT,0,None)
			glBindBuffer(GL_ARRAY_BUFFER,0)	
			glEnableClientState(GL_NORMAL_ARRAY)			
	
		if (not	self.colors	is None):
			if self.gpu_colors is None: 
				self.gpu_colors=GLVertexBuffer(self.colors)
			glBindBuffer(GL_ARRAY_BUFFER, self.gpu_colors.id)
			glColorPointer(3,GL_FLOAT,0,None)
			glBindBuffer(GL_ARRAY_BUFFER,0)	
			glEnableClientState(GL_COLOR_ARRAY)
			glEnable(GL_COLOR_MATERIAL)
	
		if (self.texture and self.texturecoords):
			glColor4f(1,1,1,1)
			
			if (self.gpu_texture is None): 
				self.gpu_texture=GLTexture(self.texture)
	
			glActiveTexture(GL_texture)
			glClientActiveTexture (GL_texture)
			glBindTexture(GL_TEXTURE_2D, self.gpu_texture.id)
			glEnable(GL_TEXTURE_2D)
			
			if self.gpu_texturecoords is None: 
				self.gpu_texturecoords=GLVertexBuffer(self.gpu_texturecoords)
				
			glBindBuffer(GL_ARRAY_BUFFER, self.gpu_texturecoords.id)
			glTexCoordPointer(2,GL_FLOAT,0,None)
			glBindBuffer(GL_ARRAY_BUFFER,0)
			glEnableClientState(GL_TEXTURE_COORD_ARRAY)
	
		glPushMatrix()
		glMultMatrixf(self.T.transpose().toList())
		num_vertices=int(len(self.vertices)/3)
		glDrawArrays(self.primitive, 0, num_vertices)
		glPopMatrix()
	
		if (self.diffuse[3]<1) :
			glDisable(GL_BLEND)
	
		if (self.texture and self.texturecoords):
			glActiveTexture(GL_texture)
			glClientActiveTexture(GL_texture)
			glDisableClientState(GL_TEXTURE_COORD_ARRAY)
			glDisable(GL_TEXTURE_2D)
	
		if (self.colors):
			glDisableClientState(GL_COLOR_ARRAY)
			glDisable(GL_COLOR_MATERIAL)
	
		if (self.normals) :
			glDisableClientState(GL_NORMAL_ARRAY)
	
		if (self.vertices):
			glDisableClientState(GL_VERTEX_ARRAY)


# ///////////////////////////////////////////////////////////////////////////////
class PlasmViewer:
	
	# constructor
	def __init__(self):
		self.W=1024
		self.H=768
		self.fov=60.0
		self.button=0
		self.objects=[]
		
	# setObjects
	def addObjects(self,value):
		
		self.objects+=value
		
		# calculate bounding box -> (-1,+1) ^3
		BOX=Box3d.invalid()
		for obj in self.objects:
			box=obj.getBoundingBox()
			BOX.addBox(box)
		S=BOX.size()
		maxsize=max([S.x,S.y,S.z])
		
		for obj in self.objects:
			obj.T=Matrix.translate(Point3d(-1.0,-1.0,-1.0)) * Matrix.scale(Point3d(2.0/maxsize,2.0/maxsize,2.0/maxsize)) * Matrix.translate(-BOX.p1) 
		
		self.pos = Point3d(3,3,3)
		self.dir = (Point3d(0,0,0)-self.pos).normalized()
		self.vup = Point3d(0,0,1)
		
		maxsize=2.0
		self.zNear		 = maxsize / 50.0
		self.zFar		 = maxsize * 10.0
		self.walk_speed = maxsize / 100.0		
		self.redisplay()	

	# redisplay
	def redisplay(self):
		pass			
		
	#run
	def run(self):
		
		glfw.init()
		self.win = glfw.create_window(self.W, self.H, "Plasm", None, None)	
		glfw.make_context_current(self.win)			
		
		glfw.set_window_size_callback  (self.win, self.resizeGL)  
		glfw.set_key_callback          (self.win, self.keyPressEvent)	
		glfw.set_cursor_pos_callback   (self.win, self.mouseMoveEvent)
		glfw.set_mouse_button_callback (self.win, self.mouseButtonEvent)
		glfw.set_scroll_callback       (self.win, self.mouseWheelEvent)	
		
		self.initializeGL()
		while not glfw.window_should_close(self.win):
			self.glRender()
			glfw.swap_buffers(self.win)
			glfw.poll_events()
		glfw.terminate()		
		
	# resizeGL
	def resizeGL(self, win, width, height):
		if (width==0 or height==0): return
		self.W = width
		self.H = height
		self.redisplay()				
		
	# mouseButtonEvent
	def mouseButtonEvent(self,win,button,action,mods):
		
		button={0:1,1:3,2:2}[button]
		
		if action == glfw.PRESS and self.button==0:
			self.button=button
			self.redisplay()
			return
			
		if action==glfw.RELEASE and button==self.button:
			self.button=0
			self.redisplay()
			return
		
		
	# mouseMoveEvent
	def mouseMoveEvent(self,win,x,y):
		
		button=self.button
		
		if (button==0):
			self.mouse_beginx = x
			self.mouse_beginy = y		
			return
			
		deltax = float(x - self.mouse_beginx)	 
		deltay = float(self.mouse_beginy - y)
		W=self.W
		H=self.H			
			
		modelview=self.getModelviewMatrix()
		
		if (button==1):
			screen_center=Point3d(W/2.0,H/2.0,0.0)
			a=(Point3d((float)(self.mouse_beginx-screen_center[0]), (float)(H-self.mouse_beginy-screen_center[1]), 0))*(1.0/min([W,H]))
			b=(Point3d((float)(               x -screen_center[0]), (float)(H-                y-screen_center[1]), 0))*(1.0/min([W,H]))
			a.z=math.pow(2.0, -0.5 * a.module())
			b.z=math.pow(2.0, -0.5 * b.module())
			a = a.normalized()
			b = b.normalized()
			axis = a.crossProduct(b).normalized()
			angle = math.acos(a.dotProduct(b))
			q=Quaternion.fromAxisAndAngle(axis, angle) *  modelview.toQuaternion()
			modelview=Matrix.translate(Point3d(modelview.mat[3],modelview.mat[7],modelview.mat[11])) * q.toMatrix()

		elif (button==3):
			modelview = Matrix.translate(Point3d(deltax* self.walk_speed,deltay* self.walk_speed,0.0) ) * modelview

		self.pos,self.dir,self.vup=modelview.getLookAt()	

		self.mouse_beginx = x
		self.mouse_beginy = y
		self.redisplay()		
		
	# mouseWheelEvent
	def mouseWheelEvent(self,win,dx,dy):
		K=(+1 if dy>0 else -1)*self.walk_speed
		self.pos=self.pos+self.dir*K
		self.redisplay()	

	# keyPressEvent
	def keyPressEvent(self,win, key, scancode, action, mods):
		
		if action != glfw.PRESS:
			return	
			
		x,y=0.5*self.W,0.5*self.H	
		
		if key == glfw.KEY_ESCAPE: 
			self.exitNow = True
			return		
		
		if (key=="+" or key=="="):
			self.walk_speed*=0.95
			return 
	
		if (key=='-' or key=="_"):
			self.walk_speed*=(1.0/0.95)
			return 
	
		if (key=='w'):
				dir=self.unproject(x,y)
				self.pos=self.pos+dir*self.walk_speed
				self.redisplay()
				return 
	
		if (key=='s'):
				dir=self.unproject(x,y)
				self.pos=self.pos-dir*self.walk_speed
				self.redisplay()
				return 
				
		if (key=='a'	or key==glfw.KEY_LEFT):
			right=Point3d.crossProduct(self.dir,self.vup).normalized()
			self.pos=self.pos-right*self.walk_speed
			self.redisplay()
			return 
	
		if (key=='d' or key==glfw.KEY_RIGHT):
			right=Point3d.crossProduct(self.dir,self.vup).normalized()
			self.pos=self.pos+right*self.walk_speed
			self.redisplay()
			return				 
	
		if (key==glfw.KEY_UP):
			self.pos=self.pos+self.vup*self.walk_speed
			self.redisplay()
			return 
	
		if (key==glfw.KEY_DOWN):
			self.pos=self.pos-self.vup*self.walk_speed
			self.redisplay()
			return 		
			
	# getModelviewMatrix
	def getModelviewMatrix(self):
		return Matrix.lookAt(self.pos,self.pos+self.dir,self.vup)
				
	# getProjectionMatrix
	def getProjectionMatrix(self):
		return Matrix.perspective(self.fov,self.W/float(self.H),self.zNear,self.zFar)
	
	# initializeGL
	def initializeGL(self):
		glEnable(GL_LIGHTING)
		glEnable(GL_POINT_SMOOTH)
		glEnable(GL_DEPTH_TEST)
		glEnable(GL_NORMALIZE)
		glShadeModel(GL_SMOOTH)
		glDepthFunc(GL_LEQUAL)
		glDisable(GL_COLOR_MATERIAL)
		glDisable(GL_CULL_FACE)
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
		glClearDepth(1.0)
		glClearColor(0.3,0.4,0.5, 0.00)

		glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,0)
		glLightfv(GL_LIGHT0,GL_AMBIENT	, [+1.00,+1.00,+1.00,+1.00])
		glLightfv(GL_LIGHT0,GL_DIFFUSE	, [+1.00,+1.00,+1.00,+1.00])
		glLightfv(GL_LIGHT0,GL_SPECULAR , [+1.00,+1.00,+1.00,+1.00])

		glActiveTexture(GL_TEXTURE1)
		glClientActiveTexture (GL_TEXTURE1)
		glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S		 ,GL_REPEAT)
		glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T		 ,GL_REPEAT)
		glTexParameterf(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,GL_LINEAR)
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE	 ,GL_MODULATE)
		glTexParameteri(GL_TEXTURE_2D	,GL_TEXTURE_MIN_FILTER ,GL_NEAREST_MIPMAP_LINEAR)

		glActiveTexture(GL_TEXTURE0)
		glClientActiveTexture (GL_TEXTURE0)
		glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S		 ,GL_REPEAT)
		glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T		 ,GL_REPEAT)
		glTexParameterf(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,GL_LINEAR)
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE	 ,GL_MODULATE)
		glTexParameteri(GL_TEXTURE_2D	,GL_TEXTURE_MIN_FILTER ,GL_NEAREST_MIPMAP_LINEAR)	  
		
	# unproject
	def project(self,pos):
		model=self.getModelviewMatrix ().transpose().toList()
		proj =self.getProjectionMatrix().transpose().toList()
		viewport = [ 0, 0, self.W, self.H]
		return gluProject(pos.x,pos.y,pos.z,model,proj,viewport)
		
	# unproject
	def unproject(self,x,y):
		model=self.getModelviewMatrix ().transpose().toList()
		proj =self.getProjectionMatrix().transpose().toList()
		viewport = [ 0, 0, self.W, self.H]
		P1 = gluUnProject(x,self.H-y,-1,model,proj,viewport)		
		P2 = gluUnProject(x,self.H-y,+1,model,proj,viewport)	 
		P1=Point3d(P1[0],P1[1],P1[2])
		P2=Point3d(P2[0],P2[1],P2[2])
		return (P2-P1).normalized() 
		 			 
		 
	# glRender
	def glRender(self):
		
		# destruction of old vertex buffers
		for it in GLVertexBuffer.destroy_list:
			glDeleteBuffers(1, GLuint(it))	 
		GLVertexBuffer.destroy_list=[]
			
		# destruction of old textures
		for it in GLTexture.destroy_list:
			glDeleteTextures(1, GLuint(it))	
		GLTexture.destroy_list=[]				
		
		glViewport(0,0,self.W,self.H)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		
		# set projection
		glMatrixMode(GL_PROJECTION)
		glLoadMatrixf(self.getProjectionMatrix().transpose().toList())
		
		# set modelview
		glMatrixMode(GL_MODELVIEW)	
		glLoadMatrixf(self.getModelviewMatrix().transpose().toList())

		# draw axis
		if (True):
			glDisable(GL_LIGHTING)
			glLineWidth(3)
			glBegin(GL_LINES)
			glColor3f(1,0,0);glVertex3i(0,0,0);glVertex3i(1,0,0)
			glColor3f(0,1,0);glVertex3i(0,0,0);glVertex3i(0,1,0)
			glColor3f(0,0,1);glVertex3i(0,0,0);glVertex3i(0,0,1)
			glEnd()
			glLineWidth(1)
			glEnable(GL_LIGHTING)
			
		# set default light
		if (True):
			glEnable(GL_LIGHTING)
			glEnable(GL_LIGHT0)
			glLightfv(GL_LIGHT0, GL_POSITION, [self.pos.x,self.pos.y,self.pos.z,1.0])
			glLightfv(GL_LIGHT0, GL_AMBIENT,	[1, 1, 1, 1])
			glLightfv(GL_LIGHT0, GL_DIFFUSE,	[1, 1, 1, 1])
			glLightfv(GL_LIGHT0, GL_SPECULAR, [1, 1, 1, 1])

		# draw objects
		for obj in self.objects:
			obj.glRender()	
				
		 # draw lines
		if True:
			glDisable(GL_LIGHTING)
			glDepthMask(False) 
			glLineWidth(2)
			glColor3f(0.5,0.5,0.5)
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE)
			for obj in self.objects:
				if obj.getDimension()>=2: 
					obj.glRender()	 
			glDepthMask(True)
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL)
			glLineWidth(1)			 

if __name__ == "__main__":
	viewer=PlasmViewer()
	viewer.addObjects([GLMesh.Cuboid()])
	viewer.run()
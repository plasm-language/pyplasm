import math
import sys

from math            import *
from OpenGL.GL			import *
from OpenGL.GLU		import *

import glfw

# /////////////////////////////////////////////////////////////////////////
class Point3d:
	
	def __init__(self,x=0.0, y=0.0, z=0.0):
		self.x = float(x)
		self.y = float(y)
		self.z = float(z)
		
	# __repr__
	def __repr__(self):
		return "Point3d(%f,%f,%f)" % (self.x,self.y,self.z)
		
	def __getitem__(self, key):
		return {0:self.x,1:self.y,2:self.z}[key]
			
	def __setitem__(self, key, value):
		if key==0: 
			self.x=value
		elif key==1:
			self.y=value
		elif key==2:
			self.z=value
		else:
			raise Exception("iternal error")
	
	def __add__(self, o):
		return Point3d((self.x + o.x), (self.y + o.y), (self.z + o.z))
	
	def __sub__(self, o):
		return Point3d((self.x - o.x), (self.y - o.y), (self.z - o.z))
		
	def __mul__(self, vs):
		return Point3d(self.x*vs,self.y*vs,self.z*vs)		

	def __rmul__(self, vs):
		return Point3d(self.x*vs,self.y*vs,self.z*vs)
	
	def __iadd__(self, o):
		self.x += o.x
		self.y += o.y
		self.z += o.z
		return self
	
	def __isub__(self, o):
		self.x -= o.x
		self.y -= o.y
		self.z -= o.z
		return self
	
	def __neg__(self):
		return Point3d(-self.x, -self.y, -self.z)
		
		
def length(p):
	return sqrt((p.x*p.x) + (p.y*p.y) + (p.z*p.z))

def normalized(vec):
	len=length(vec)
	return Point3d((vec.x / len), (vec.y / len), (vec.z / len))

def dot(a, b):
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z)

def cross(a,b): 
	return Point3d(
		a.y * b.z - b.y * a.z, 
		a.z * b.x - b.z * a.x, 
		a.x * b.y - b.x * a.y)
	
# /////////////////////////////////////////////////////////////////////////
class Point4d:
	
	def __init__(self,x=0.0, y=0.0, z=0.0, w=0.0):
		self.x = float(x)
		self.y = float(y)
		self.z = float(z)
		self.w = float(w)	
	
	def __getitem__(self, key):
		return {0:self.x, 1:self.y, 2:self.z, 3:self.w}[key]
			
	def __setitem__(self, key, value):
		if key==0: 
			self.x=value
		elif key==1:
			self.y=value
		elif key==2:
			self.z=value
		elif key==3:
			self.w=value
		else:
			raise Exception("iternal error")	
	
		
# /////////////////////////////////////////////////////////////////////
class Box3dClass:
	def __init__(self):
		pass
		
def Box3d(p1=Point3d(),p2=Point3d()):
	ret=Box3dClass()
	ret.p1=p1
	ret.p2=p2
	return ret

def invalid_box():
	m,M=sys.float_info.min,sys.float_info.max
	return Box3d(Point3d(M,M,M),Point3d(m,m,m))	
	
def add_point(box,p):
	for i in range(3):
		box.p1[i]=min(box.p1[i],p[i])
		box.p2[i]=max(box.p2[i],p[i])
	
	
# /////////////////////////////////////////////////////////////////////
class MatrixClass:
	def __init__(self):
		pass
		
	# __repr__
	def __repr__(self):
		return "MatrixClass([%s])" % (",".join([str(it) for it in self.mat],),)


def Matrix(mat=[1.0, 0.0, 0.0, 0.0,  0.0, 1.0, 0.0, 0.0,  0.0, 0.0, 1.0, 0.0,  0.0, 0.0, 0.0, 1.0]):
	ret=MatrixClass()
	ret.mat = mat
	return ret
	
def transpose(T):
	return Matrix([
		T.mat[ 0],T.mat[ 4],T.mat[ 8],T.mat[12],
		T.mat[ 1],T.mat[ 5],T.mat[ 9],T.mat[13],
		T.mat[ 2],T.mat[ 6],T.mat[10],T.mat[14],
		T.mat[ 3],T.mat[ 7],T.mat[11],T.mat[15],
	])

def transform_point(T,p):
	X=(T.mat[ 0]*(p.x)+T.mat[ 1]*(p.y)+T.mat[ 2]*(p.z)+ T.mat[ 3]*(1.0))
	Y=(T.mat[ 4]*(p.x)+T.mat[ 5]*(p.y)+T.mat[ 6]*(p.z)+ T.mat[ 7]*(1.0))
	Z=(T.mat[ 8]*(p.x)+T.mat[ 9]*(p.y)+T.mat[10]*(p.z)+ T.mat[11]*(1.0))
	W=(T.mat[12]*(p.x)+T.mat[13]*(p.y)+T.mat[14]*(p.z)+ T.mat[15]*(1.0))
	return Point4d(X,Y,Z,W)	
	
def matrix_prod(A,B):
	A,B=A.mat,B.mat
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

def inv(T):
	m=transpose(T).mat
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
	if det==0: return Matrix()
	return transpose(Matrix([it * (1.0/det) for it in inv])) 
	
	
def matrix_drop_w(T):
	ret=MatrixClass()
	ret.mat = [
		T.mat[0],T.mat[1],T.mat[ 2],
		T.mat[4],T.mat[5],T.mat[ 6],
		T.mat[8],T.mat[9],T.mat[10]]
	return ret	
	
def translate_matrix(vt):
	return Matrix([
			1.0, 0.0, 0.0, vt[0],
			0.0, 1.0, 0.0, vt[1],
			0.0, 0.0, 1.0, vt[2],
			0.0, 0.0, 0.0, 1.0])	
		
def scale_matrix(vs):
	return Matrix([
			vs[0], 0.0, 0.0, 0.0,
			0.0, vs[1], 0.0, 0.0,
			0.0, 0.0, vs[2], 0.0,
			0.0, 0.0, 0.0, 1.0])					
	
def lookat_matrix(eye, center, up):
	forward=normalized(center-eye)
	side   = normalized(cross(forward,up))
	up     = cross(side,forward)
	m = Matrix([
		side[0],up[0],-forward[0], 0.0,
		side[1],up[1],-forward[1], 0.0,
		side[2],up[2],-forward[2], 0.0,
		0.0,0.0,0.0,1.0
	])
	ret= matrix_prod(transpose(m) , translate_matrix(-1*eye))
	return ret
	

def perspective_matrix(fovy, aspect, zNear, zFar):
	radians =  math.radians(fovy/2.0)
	cotangent = math.cos(radians) / math.sin(radians)
	m=Matrix()
	m.mat[ 0] = cotangent / aspect
	m.mat[ 5] = cotangent
	m.mat[10] = -(zFar + zNear) / (zFar - zNear)
	m.mat[11] = -1
	m.mat[14] = -2 * zNear * zFar / (zFar - zNear)
	m.mat[15] = 0
	ret=transpose(m)
	return ret
  
def decompose_modelview(T):
	vmat=inv(T).mat
	pos=Point3d(  vmat[3], vmat[7], vmat[11])
	dir=normalized(Point3d( -vmat[2],-vmat[6],-vmat[10]))
	vup=normalized(Point3d(  vmat[1], vmat[5], vmat[ 9]))
	return [pos,dir,vup]		
	
	
	
# /////////////////////////////////////////////////////////////////////
class QuaternionClass:
	def __init__(self):
		pass
	# __repr__
	def __repr__(self):
		return "Quaternion(%f,%f,%f,%f)" % (self.w,self.x,self.y,self.z)

def Quaternion(fW =1.0,fX=0.0,fY=0.0,fZ=0.0):
	ret=QuaternionClass()
	ret.w = fW
	ret.x = fX
	ret.y = fY
	ret.z = fZ
	return ret	
	
def QuaternionFromAxisAndAngle(axis,angle):
	axis=normalized(axis)
	halfangle = 0.5*angle
	fSin = sin(halfangle)
	return Quaternion(cos(halfangle),fSin*axis.x,fSin*axis.y,fSin*axis.z)
	
	
def to_quaternion(T):
	
	kRot=[[T.mat[ 0],T.mat[ 1],T.mat[ 2]],
			[T.mat[ 4],T.mat[ 5],T.mat[ 6]],
			[T.mat[ 8],T.mat[ 9],T.mat[10]]]
			
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
	


def to_matrix(quat):
	fTx  = 2.0*quat.x
	fTy  = 2.0*quat.y
	fTz  = 2.0*quat.z
	fTwx = fTx*quat.w
	fTwy = fTy*quat.w
	fTwz = fTz*quat.w
	fTxx = fTx*quat.x
	fTxy = fTy*quat.x
	fTxz = fTz*quat.x
	fTyy = fTy*quat.y
	fTyz = fTz*quat.y
	fTzz = fTz*quat.z
	return Matrix([
		1.0-(fTyy+fTzz),    (fTxy-fTwz),    (fTxz+fTwy),0.0,
		    (fTxy+fTwz),1.0-(fTxx+fTzz),    (fTyz-fTwx),0.0,
		    (fTxz-fTwy),    (fTyz+fTwx),1.0-(fTxx+fTyy),0.0,
		0.0,0.0,0.0,1.0])


def quaternion_prod(a,b):
	return Quaternion(
		a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
		a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
		a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
		a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x)		
	
	
# /////////////////////////////////////////////////////////////////////
import numpy

class VertexBufferClass:
	
	def __init__(self):
		pass
		
	def __del__(self):
		if self.id>=0:
			glDeleteBuffers(1, self.id)	 
	
def VertexBuffer(vector):
	if not isinstance(vector[0],float):
		raise Exception("internal error")
	ret=VertexBufferClass()
	ret.id=0
	ret.vector=vector
	return ret

def enable_attribute(location,buffer,num_components):
	if buffer.id<=0: buffer.id = glGenBuffers(1)	
	glBindBuffer(GL_ARRAY_BUFFER, buffer.id)
	size_of_float32=4
	c_size=len(buffer.vector)*size_of_float32
	glBufferData (GL_ARRAY_BUFFER, c_size, numpy.array(buffer.vector, dtype="float32"), GL_STATIC_DRAW)
	glVertexAttribPointer(location,num_components,GL_FLOAT,GL_FALSE,0,None)
	glEnableVertexAttribArray(location)	
	glBindBuffer(GL_ARRAY_BUFFER, 0)	
	

# /////////////////////////////////////////////////////////////////////
class MeshClass:
	def __init__(self):
		pass
		
POINTS    = 0x0000
LINES     = 0x0001
TRIANGLES  = 0x0004
		
def Mesh(primitive):
	
	ret=MeshClass()
	ret.primitive=primitive
	ret.T=Matrix() 
	
	ret.vertex_array=None
	ret.vertices=None
	ret.normals=None
	ret.colors=None
	
	return ret
	

def cuboid(box):
	
	points=[
		Point3d(box.p1.x,box.p1.y,box.p1.z),
		Point3d(box.p2.x,box.p1.y,box.p1.z),
		Point3d(box.p2.x,box.p2.y,box.p1.z),
		Point3d(box.p1.x,box.p2.y,box.p1.z),
		Point3d(box.p1.x,box.p1.y,box.p2.z),
		Point3d(box.p2.x,box.p1.y,box.p2.z),
		Point3d(box.p2.x,box.p2.y,box.p2.z),
		Point3d(box.p1.x,box.p2.y,box.p2.z)
	]
	
	faces=[[0, 1, 2, 3],[3, 2, 6, 7],[7, 6, 5, 4],[4, 5, 1, 0],[5, 6, 2, 1],[7, 4, 0, 3]]
	
	vertices=[]
	for face in faces:
		p3,p2,p1,p0 = points[face[0]],points[face[1]],points[face[2]],points[face[3]] # reverse order
		vertices+=[p0.x,p0.y,p0.z,p1.x,p1.y,p1.z,p2.x,p2.y,p2.z]
		vertices+=[p0.x,p0.y,p0.z,p2.x,p2.y,p2.z,p3.x,p3.y,p3.z]
		
	ret=Mesh(TRIANGLES)
	ret.vertices = VertexBuffer(vertices)
	ret.normals  = VertexBuffer(compute_normals(vertices))
	return ret
	
def axis(box):
	ret=Mesh(LINES)
	p0=[box.p1[0],box.p1[1],box.p1[2]]
	p1=[box.p2[0],box.p2[1],box.p2[2]]
	p0=[p0[0],p0[1],p0[2]]
	px=[p1[0],p0[1],p0[2]]
	py=[p0[0],p1[1],p0[2]]
	pz=[p0[0],p0[1],p1[2]]
	r=[1.0,0.0,0.0,1.0]
	g=[0.0,1.0,0.0,1.0]
	b=[0.0,0.0,1.0,1.0]
	ret.vertices=VertexBuffer(p0+px+p0+py+p0+pz)
	ret.colors=VertexBuffer(r+r+g+g+b+b)
	return ret



def compute_normal(p0,p1,p2):
	return normalized(cross(p1-p0,p2-p0))
	
def compute_normals(vertices):
	ret=[]
	for I in range(0,len(vertices),9):
		p0=Point3d(vertices[I+0],vertices[I+1],vertices[I+2])
		p1=Point3d(vertices[I+3],vertices[I+4],vertices[I+5])
		p2=Point3d(vertices[I+6],vertices[I+7],vertices[I+8])
		n=compute_normal(p0,p1,p2)
		ret+=[n.x,n.y,n.z]*3
	return ret


def get_bounding_box(mesh):
	box=invalid_box()
	V=mesh.vertices.vector
	for I in range(0,len(V),3):
		point=Point3d(V[I+0],V[I+1],V[I+2])
		add_point(box,point)
	return box	
		
		
# /////////////////////////////////////////////////////////////
class FrustumMapClass:
	def __init__(self):
		pass	

		
def FrustumMap(viewport,projection,modelview):
	
	ret=FrustumMapClass()
	
	ret.viewport=Matrix4(
		viewport[2] / 2.0 ,                   0,       0, viewport[0] + viewport[2] / 2.0,
		                  0,  viewport[3] / 2.0,       0, viewport[1] + viewport[3] / 2.0,
		                  0,                  0, 1 / 2.0, 1 / 2.0)	
	ret.projection=projection
	ret.modelview=modelview
	
	ret.inv_viewport   = inv(ret.viewport)
	ret.inv_projection = inv(ret.projection)
	ret.inv_modelview  = inv(ret.modelview)
	
	return ret
	
def project_point(map,p3):
	p4=transform_point(map.viewport  .T , transform_point(map.projection.T , transform_point(map.modelview .T , Point4d(p3[0],p3[1],p3[2],1.0))))
	return Point3d(p4[0]/p4[3],p4[1]/p4[3],p4[2]/p4[3])

def unproject_point(map,p2, Z = 0.0):
	p4 = transform_point(map.inv_modelview  , transform_point(map.inv_projection , transform_point(map.inv_viewport   , Point4d(p2[0], p2[1], Z, 1.0))))
	if p4.w==0: p4.w=1
	return Point3d(p4[0]/p4[3],p4[1]/p4[3],p4[2]/p4[3])	
		

# /////////////////////////////////////////////////////////////////////
class ViewerClass:
	def __init__(self):
		pass
		
viewer=None	

def View(meshes):
	
	global viewer
	
	viewer=ViewerClass()
	viewer.W=1024
	viewer.H=768
	viewer.fov=60.0
	viewer.button=0
	viewer.meshes=meshes
	viewer.shader=None
	
	# calculate bounding box -> (-1,+1) ^3
	BOX=invalid_box()
	for mesh in viewer.meshes:
		box=get_bounding_box(mesh)
		add_point(BOX,box.p1)
		add_point(BOX,box.p2)
	S=BOX.p2-BOX.p1
	maxsize=max([S.x,S.y,S.z])
	
	for mesh in viewer.meshes:
		mesh.T=matrix_prod(translate_matrix(Point3d(-1.0,-1.0,-1.0)) , matrix_prod(scale_matrix(Point3d(2.0/maxsize,2.0/maxsize,2.0/maxsize)) , translate_matrix(-BOX.p1)))
	
	viewer.pos = Point3d(3,3,3)
	viewer.dir = normalized(Point3d(0,0,0)-viewer.pos)
	viewer.vup = Point3d(0,0,1)
	
	maxsize=2.0
	viewer.zNear		 = maxsize / 50.0
	viewer.zFar		 = maxsize * 10.0
	viewer.walk_speed = maxsize / 100.0		
	redisplay(viewer)			
	
	glfw.init()
	
	glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)
	glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 2)
	glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, GL_TRUE)
	glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
	
	viewer.win = glfw.create_window(viewer.W, viewer.H, "Plasm", None, None)	
	glfw.make_context_current(viewer.win)			
	
	glfw.set_window_size_callback  (viewer.win, on_resize_evt)  
	glfw.set_key_callback          (viewer.win, on_keypress_evt)	
	glfw.set_cursor_pos_callback   (viewer.win, on_mousemove_evt)
	glfw.set_mouse_button_callback (viewer.win, on_mousebutton_evt)
	glfw.set_scroll_callback       (viewer.win, on_mousewheel_evt)	
	
	while not glfw.window_should_close(viewer.win):
		glRender(viewer)
		glfw.swap_buffers(viewer.win)
		glfw.poll_events()
	glfw.terminate()		
	
def redisplay(viewer):
	pass			

def on_resize_evt(win, width, height):
	global viewer
	if (width==0 or height==0): return
	viewer.W = width
	viewer.H = height
	redisplay(viewer)				
	

def on_mousebutton_evt(win,button,action,mods):
	global viewer
	
	button={0:1,1:3,2:2}[button]
	
	if action == glfw.PRESS and viewer.button==0:
		viewer.button=button
		redisplay(viewer)		
		return
		
	if action==glfw.RELEASE and button==viewer.button:
		viewer.button=0
		redisplay(viewer)		
		return
	
def on_mousemove_evt(win,x,y):
	
	global viewer
	button=viewer.button
	
	if (button==0):
		viewer.mouse_beginx = x
		viewer.mouse_beginy = y		
		return
		
	deltax = float(x - viewer.mouse_beginx)	 
	deltay = float(viewer.mouse_beginy - y)
	W=viewer.W
	H=viewer.H			
		
	modelview=get_modelview (viewer)
	
	if (button==1):
		screen_center=Point3d(W/2.0,H/2.0,0.0)
		a=(Point3d((float)(viewer.mouse_beginx-screen_center[0]), (float)(H-viewer.mouse_beginy-screen_center[1]), 0))*(1.0/min([W,H]))
		b=(Point3d((float)(                 x -screen_center[0]), (float)(H-                  y-screen_center[1]), 0))*(1.0/min([W,H]))
		a.z=math.pow(2.0, -0.5 * length(a))
		b.z=math.pow(2.0, -0.5 * length(b))
		a = normalized(a)
		b = normalized(b)
		axis = normalized(cross(a,b))
		angle = math.acos(dot(a,b))
		q=quaternion_prod(QuaternionFromAxisAndAngle(axis, angle),to_quaternion(modelview))
		t=Point3d(modelview.mat[3],modelview.mat[7],modelview.mat[11])
		modelview=matrix_prod(translate_matrix(t) , to_matrix(q))

	elif (button==3):
		t=Point3d(deltax* viewer.walk_speed,deltay* viewer.walk_speed,0.0)
		modelview = matrix_prod(translate_matrix(t ) , modelview)

	viewer.pos,viewer.dir,viewer.vup=decompose_modelview(modelview)	

	viewer.mouse_beginx = x
	viewer.mouse_beginy = y
	redisplay(viewer)			
	
def on_mousewheel_evt(win,dx,dy):
	global viewer
	K=(+1 if dy>0 else -1)*viewer.walk_speed
	viewer.pos=viewer.pos+viewer.dir*K
	redisplay(viewer)		

def on_keypress_evt(win,key, scancode, action, mods):
	
	global viewer
	if action != glfw.PRESS:
		return	
		
	x,y=0.5*viewer.W,0.5*viewer.H	
	
	if key == glfw.KEY_ESCAPE: 
		viewer.exitNow = True
		return		
	
	if (key=="+" or key=="="):
		viewer.walk_speed*=0.95
		return 

	if (key=='-' or key=="_"):
		viewer.walk_speed*=(1.0/0.95)
		return 

	if (key=='w'):
			dir=unproject(viewer,x,y)
			viewer.pos=viewer.pos+dir*viewer.walk_speed
			redisplay(viewer)		
			return 

	if (key=='s'):
			dir=unproject(viewer,x,y)
			viewer.pos=viewer.pos-dir*viewer.walk_speed
			redisplay(viewer)		
			return 
			
	if (key=='a'	or key==glfw.KEY_LEFT):
		right=normalized(cross(viewer.dir,viewer.vup))
		viewer.pos=viewer.pos-right*viewer.walk_speed
		redisplay(viewer)		
		return 

	if (key=='d' or key==glfw.KEY_RIGHT):
		right=normalized(cross(viewer.dir,viewer.vup))
		viewer.pos=viewer.pos+right*viewer.walk_speed
		redisplay(viewer)		
		return				 

	if (key==glfw.KEY_UP):
		viewer.pos=viewer.pos+viewer.vup*viewer.walk_speed
		redisplay(viewer)		
		return 

	if (key==glfw.KEY_DOWN):
		viewer.pos=viewer.pos-viewer.vup*viewer.walk_speed
		redisplay(viewer)		
		return 		
		
def get_modelview(viewer):
	return lookat_matrix(viewer.pos,viewer.pos+viewer.dir,viewer.vup)
			
def get_projection(viewer):
	return perspective_matrix(viewer.fov,viewer.W/float(viewer.H),viewer.zNear,viewer.zFar)

def project(viewer,pos):
	viewport,projection,modelview = [ 0, 0, viewer.W, viewer.H], get_projection(viewer), get_modelview(viewer)
	map=FrustumMap(viewport,projection,modelview)
	return project_point(map,pos.x,pos.y,pos.z)
	
def unproject(viewer,x,y):
	viewport,projection,modelview = [ 0, 0, viewer.W, viewer.H], get_projection(viewer), get_modelview(viewer)
	map=FrustumMap(viewport,projection,modelview)
	P1=unproject_point(map,x,viewer.H-y,-1)
	P2=unproject_point(map,x,viewer.H-y,+1)
	return normalized(P2-P1) 
	
# /////////////////////////////////////////////////////////////////////
class ShaderProgramClass:
	def __init__(self):
		pass

def create_shader(type,source):
	id = glCreateShader(type)
	glShaderSource(id, source)
	glCompileShader(id)
	if glGetShaderiv(id, GL_COMPILE_STATUS) != GL_TRUE:
		raise RuntimeError('Shader compilation failed: %s' % (glGetShaderInfoLog(id),))
	return id
	
def ShaderProgram(vertex, fragment):
	ret=ShaderProgramClass()
	ret.program_id = glCreateProgram()
	glAttachShader(ret.program_id, create_shader(GL_VERTEX_SHADER  ,vertex  ))
	glAttachShader(ret.program_id, create_shader(GL_FRAGMENT_SHADER,fragment))
	glLinkProgram(ret.program_id)
	if glGetProgramiv(ret.program_id, GL_LINK_STATUS) != GL_TRUE:
		raise RuntimeError('Error linking program: %s' % (glGetProgramInfoLog(ret.program_id),))
	return ret





phong_shader_vertex="""
uniform mat4 u_modelview_matrix;
uniform mat4 u_projection_matrix;
uniform vec4 u_color;

attribute  vec4 a_position;

#if LIGHTING_ENABLED
attribute  vec3 a_normal;
#endif

#if COLOR_ATTRIBUTE_ENABLED
attribute vec4 a_color;
#endif

#if LIGHTING_ENABLED
uniform mat3 u_normal_matrix;
uniform vec3 u_light_position;
varying vec3 v_normal;
varying vec3 v_light_dir;
varying vec3 v_eye_vec;
#endif

#if COLOR_ATTRIBUTE_ENABLED
  varying vec4 v_color;
#endif

void main() 
{
	vec4 eye_pos= u_modelview_matrix * a_position;
	
#if LIGHTING_ENABLED	
	v_normal = u_normal_matrix * a_normal;
	vec3 vVertex = vec3(u_modelview_matrix * a_position);
	v_light_dir  = normalize(u_light_position - vVertex);
	v_eye_vec    = normalize(-vVertex);
#endif	

#if COLOR_ATTRIBUTE_ENABLED
	v_color=a_color;
#endif
	
	gl_Position = u_projection_matrix * eye_pos;
}
"""

phong_shader_fragment="""
uniform vec4 u_color;

#if LIGHTING_ENABLED
varying vec3 v_normal;
varying vec3 v_light_dir;
varying vec3 v_eye_vec;
#endif

#if COLOR_ATTRIBUTE_ENABLED
  varying vec4 v_color;
#endif

void main() 
{
	vec4 frag_color=u_color; 
	
  #if LIGHTING_ENABLED
	vec3 N = normalize(v_normal   );
	vec3 L = normalize(v_light_dir);
	vec3 E = normalize(v_eye_vec  );

	vec4  u_material_ambient  = vec4(0.2,0.2,0.2,1.0);
	vec4  u_material_diffuse  = vec4(0.8,0.8,0.8,1.0);
	vec4  u_material_specular = vec4(0.1,0.1,0.1,1.0);
	float u_material_shininess=100.0;	
	
	if(gl_FrontFacing)
	{
		frag_color = u_material_ambient;
		float NdotL = abs(dot(N,L));
		if (NdotL>0.0)
			{
			vec3 R = reflect(-L, N);
			float NdotHV = abs(dot(R, E));
			frag_color += u_material_diffuse * NdotL;
			frag_color += u_material_specular * pow(NdotHV,u_material_shininess);
		}
	}
	else
	{
		frag_color = u_material_ambient;
		float NdotL = abs(dot(-N,L));
		if (NdotL>0.0);
		{
			vec3 R = reflect(-L, -N);
			float NdotHV=abs(dot(R, E));
			frag_color += u_material_diffuse * NdotL;
			frag_color += u_material_specular * pow(NdotHV,u_material_shininess);
		}
	}
#endif

#if COLOR_ATTRIBUTE_ENABLED
	frag_color =v_color;
#endif

	gl_FragColor = frag_color;
}
"""

phong_shaders={}
def phong_shader(lighting_enabled,color_attribute_enabled):
	
	key=(lighting_enabled,color_attribute_enabled)
	if key in phong_shaders:
		return phong_shaders[key]
	
	defines=""
	if lighting_enabled:
		defines+="#define LIGHTING_ENABLED 1\n"
		
	if color_attribute_enabled:
		defines+="#define COLOR_ATTRIBUTE_ENABLED 1\n"
	
	v=defines + phong_shader_vertex
	f=defines + phong_shader_fragment
	phong_shaders[key]=ShaderProgram(v,f)	
	return phong_shaders[key]		
	
	
def glRender(viewer):	
	
	glEnable(GL_DEPTH_TEST)
	glDepthFunc(GL_LEQUAL)
	glDisable(GL_CULL_FACE)
	glClearDepth(1.0)
	glClearColor(0.3,0.4,0.5, 0.00)

	glViewport(0,0,viewer.W,viewer.H)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	
	for mesh in viewer.meshes:
		
		PROJECTION = get_projection(viewer)
		MODELVIEW  = get_modelview (viewer)
		modelview=matrix_prod(MODELVIEW, mesh.T)
		normal_matrix = matrix_drop_w(transpose(inv(modelview)))
		lightpos=transform_point(MODELVIEW,Point4d(viewer.pos.x,viewer.pos.y,viewer.pos.z,1.0))			
		
		for bRenderLines in [False,True]:

			shader=phong_shader(lighting_enabled=True if mesh.normals else False,color_attribute_enabled=True if mesh.colors else False)

			if mesh.primitive==TRIANGLES:
				glPolygonOffset(-1.0,-1.0)
				glEnable(GL_POLYGON_OFFSET_LINE)
				if bRenderLines:
					glPolygonMode(GL_FRONT_AND_BACK,GL_LINE)	
					shader=phong_shader(lighting_enabled=False,color_attribute_enabled=False)
			else:
				if bRenderLines: 
					continue
				
			a_position          = glGetAttribLocation(shader.program_id, "a_position")
			a_normal            = glGetAttribLocation(shader.program_id, "a_normal")
			a_color             = glGetAttribLocation(shader.program_id, "a_color")
			
			u_modelview_matrix  = glGetUniformLocation(shader.program_id, "u_modelview_matrix" )
			u_projection_matrix = glGetUniformLocation(shader.program_id, "u_projection_matrix")	
			u_normal_matrix     = glGetUniformLocation(shader.program_id, "u_normal_matrix" )	
			u_color             = glGetUniformLocation(shader.program_id, "u_color")
			u_light_position    = glGetUniformLocation(shader.program_id, "u_light_position")
			
			glUseProgram(shader.program_id)			
			
			glUniformMatrix4fv(u_modelview_matrix ,1, GL_TRUE, numpy.array(modelview .mat   , dtype="float32"))
			glUniformMatrix4fv(u_projection_matrix,1, GL_TRUE, numpy.array(PROJECTION.mat   , dtype="float32"))
			glUniformMatrix3fv(u_normal_matrix    ,1, GL_TRUE, numpy.array(normal_matrix.mat, dtype="float32"))
			
			if u_light_position>=0:
				glUniform3f(u_light_position,lightpos[0]/lightpos[3],lightpos[1]/lightpos[3],lightpos[2]/lightpos[3])				
			
			if not mesh.vertex_array: mesh.vertex_array=glGenVertexArrays(1)
			glBindVertexArray(mesh.vertex_array)						
			
			# positions
			if mesh.vertices and a_position>=0:
				enable_attribute(a_position,mesh.vertices,3)
			
			# normals
			if mesh.normals and a_normal>=0:
				enable_attribute(a_normal  ,mesh.normals ,3)
			
			# colors
			if mesh.colors and a_color>=0:
				enable_attribute(a_color  ,mesh.colors ,4)
				
			# render lines
			if u_color>=0:
				if bRenderLines:
					glUniform4f(u_color,0.0,0.0,0.0,1.0)	
				else:
					glUniform4f(u_color,0.5,0.5,0.5,1.0)	
						
			num_vertices=int(len(mesh.vertices.vector)/3)
			glDrawArrays(mesh.primitive, 0, num_vertices)
			
			if mesh.vertices and a_position>=0:
				glDisableVertexAttribArray(a_position)
			
			if mesh.normals and a_normal>=0:
				glDisableVertexAttribArray(a_normal)
				
			if mesh.colors and a_color>=0:	
				enable_attribute(a_color  ,mesh.colors ,4)
				
			glBindVertexArray(0)
			glUseProgram(0)	
			
			glDepthMask(True)
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL)
			glDisable(GL_POLYGON_OFFSET_LINE)
			
if __name__ == "__main__":
	View([cuboid(Box3d(Point3d(0,0,0),Point3d(1,1,1))),axis(Box3d(Point3d(-1,-1,-1),Point3d(2,2,2)))])
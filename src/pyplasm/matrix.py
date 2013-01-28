import numpy
import math

# ////////////////////////////////////////////////////////////
# Matrix class (first row/column are in omogeneous coordinates!)
# ////////////////////////////////////////////////////////////
class Matrix:
  
  def __init__(self,arg=None):
    if (arg is None):  arg=0
    self.T=numpy.matrix(numpy.identity(arg)) if isinstance(arg,int) else numpy.matrix(arg,dtype=float)
    
  def __getitem__(self, *args):
    return self.T.__getitem__(*args)
  
  def __setitem__(self, *args):
    return self.T.__setitem__(*args)
    
  # repr
  def __repr__(self):
    
    if (self.isIdentity()):
      return 'Matrix('+str(self.dim())+')'  
    else:
      return 'Matrix('+repr(self.toList())+')'  
      
  # isIdentity
  def isIdentity(self):
    return numpy.array_equal(self.T,numpy.identity(self.dim()))
    
  # toList
  def toList(self):
    return self.T.tolist()
    
  # transpose
  def transpose(self):
    return Matrix(self.T.transpose())
    
  # invert
  def invert(self):
    return Matrix(numpy.linalg.inv(self.T))
    
  # dim
  def dim(self):
    assert(self.T.shape[0]==self.T.shape[1])
    return self.T.shape[0]     
       
  # embed
  def embed(self,dim):
    if (self.dim()>=dim): return self
    ret=Matrix(dim)
    ret.T[0:self.dim(),0:self.dim()]=self.T
    return ret
    
  # adjoin
  def adjoin(self,other):
    M,N=self.dim(),other.dim()
    ret=Matrix(M+N-1)
   
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
    assert (isinstance(other,Matrix))
    return Matrix(self.T * other.T) 
    
  # transformPoint
  def transformPoint(self,point):
    assert(isinstance(point,(list, tuple)))
    point=self.T * numpy.matrix([[1.0] + list(point) + [0.0]*(self.dim()-len(point)-1)]).transpose()
    return [point[i,0]/point[0,0] for i in range(1,self.dim())]
    
  # translate (example in 2D: vt([1.0,2.0]))
  @staticmethod
  def translate(vt):
    T=Matrix(len(vt)+1)
    for I in range(1,T.dim()): T[I,0]=vt[I-1]
    return T
    
  # scale (example in 2D: vs([1.0,2.0]))
  @staticmethod
  def scale(vs):
    T=Matrix(len(vs)+1)
    for I in range(1,T.dim()): T[I,I]=vs[I-1]
    return T

  # rotate (example in 2D i=1 j=2)
  @staticmethod
  def rotate(i,j,angle):
    T=Matrix(max([i,j])+1) 
    T[i,i]=+math.cos(angle) ; T[i,j]=-math.sin(angle)
    T[j,i]=+math.sin(angle) ; T[j,j]=+math.cos(angle)
    return T      
    
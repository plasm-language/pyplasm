import math
import itertools  

from matrix import *
from mkpol  import *
from box    import *
from viewer import *

# ///////////////////////////////////////////////////////////////
# Hpc class
# ///////////////////////////////////////////////////////////////
class Hpc:
  
  # constructor
  def __init__(self,T=Matrix(0),childs=[],properties={}):
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
    return Hpc(Matrix(),[MkPol(points,hulls)])

  @staticmethod
  def Struct(pols):
    return Hpc(Matrix(),pols)    
    
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
    for T,properties,obj in Hpc(Matrix(),pols).toList():
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
    stack=[[Matrix(Tdim),{},self]]
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
    box=Box(self.dim())
    for T,properties,obj in self.toList():
      box.addPoints([T.transformPoint(p) for p in obj.points])
    return box      
      
  # transform
  def transform(self,T):
    return Hpc(T,[self])
    
  # translate (example in 2D: vt([1.0,2.0]))
  def translate(self,vt):
    return Hpc(Matrix.translate(vt),[self])
    
  # scale (example in 2D: vs([1.0,2.0]))
  def scale(self,vs):
    return Hpc(Matrix.scale(vs),[self])

  # rotate (example in 2D i=1 j=2)
  def rotate(self,i,j,angle):
    return Hpc(Matrix.rotate(i,j,angle),[self])       

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
            hulls.append([c[1]*nx + c[0] for c in list(itertools.product(*[hx,hy]))])
  
        # combination of matrices
        T=T1.adjoin(T2)
        
        childs.append(Hpc(T,[MkPol(points,hulls)]))
        
    return Hpc(Matrix(),childs)    
    
  # ukpol
  def ukpol(self):
    points,hulls=[],[]
    for T,properties,obj in self.toList():
      offset=len(points)
      points.extend([T.transformPoint(p) for p in obj.points])
      hulls.extend([[offset+idx for idx in hull] for hull in obj.hulls])
    return [points,hulls]    
    
  # view
  def view(self):
    objs=[]
    for T,properties,obj in self.toList():
      
      # homo must be in last position....
      T=T.embed(4)
      T3D=Matrix([
        [T[1,1],T[1,2],T[1,3],T[1,0]],
        [T[2,1],T[2,2],T[2,3],T[2,0]],
        [T[3,1],T[3,2],T[3,3],T[3,0]],
        [T[0,1],T[0,2],T[0,3],T[0,0]]])
    
      RF=obj.getBatches()
      for render in RF:
        render.prependTransformation(T3D)
        render.writeProperties(properties)
        objs.append(render)
    Viewer.view(objs)
   
  # mapFn
  def mapFn(self,fn):
    childs=[]
    for T,properties,obj in self.toList():
      obj=obj.toSimplicialForm()
      points=[fn(T.transformPoint(p)) for p in obj.points]
      hulls =obj.hulls
      childs.append(Hpc(Matrix(),[MkPol(points,hulls)],properties))  
    ret=Hpc(Matrix(),childs)  
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
          elif dim==3: bfaces=Batch3D.TET_ORIENTED_TRIANGLES
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
    for point,num in POINTDB.iteritems(): 
      points[num]=point  
    

    ret=Hpc.mkpol(points,faces)
    return ret
    
    
    
# ///////////////////////////////////////////////////////////
# ///////////////////////////////////////////////////////////

def selftest():
  #import doctest
  #failed, total = doctest.testmod()
  #print "%d/%d failed" % (failed, total)
  
  if False:
    print Hpc.cube(3).view()
  
  if False:
    N,M=16,16
    domain=Hpc.power(Hpc.quote([math.pi/N]*N).translate([-math.pi/2]),Hpc.quote([2*math.pi/M]*M))
    obj=domain.mapFn(lambda p: [math.cos(p[0])*math.sin(p[1]),math.cos(p[0])*math.cos(p[1]),math.sin(p[0])])
    obj.view()
    
  if False:
    print Hpc.mkpol([[0],[1],[2],[3],[4],[5]],[[5,3],[0,1]])

if __name__=="__main__":
    selftest()
    

    





 
import scipy
import scipy.spatial

from batch3d import *
   
# ///////////////////////////////////////////////////////////////
# MkPol class
# ///////////////////////////////////////////////////////////////
class MkPol:
  
  def __init__(self,points=[[]],hulls=None):
    self.__cached_simplicial_form__=None # optimization...
    self.points = points
    self.hulls  = hulls if hulls is not None else [range(len(points))]
    
    # filter not used points (at least the box() will be correct!)
    used=set([item for sublist in self.hulls for item in sublist])
    if len(used)!=len(points):
      unused=set(range(len(points)))-used
      
      # filter used points
      new_points,map_hulls=[],{}
      for N in range(len(points)):
        if not N in unused:
          new_points.append(self.points[N])
          map_hulls[N]=len(map_hulls)
        
      # remap hulls
      new_hulls=[[map_hulls[idx] for idx in hull] for hull in self.hulls]
      
      self.points=new_points
      self.hulls =new_hulls
    
  # dim
  def dim(self):
    if len(self.points)==0: return 0
    return len(self.points[0])
    
  # box
  def box(self):
    ret=Box(self.dim())
    ret.addPoints(self.points)
    return ret
    
  # __repr__
  def __repr__(self):
    return "MkPol(" + repr(self.points) +"," + repr(self.hulls)+")" 
    
  # toSimplicialForm
  def toSimplicialForm(self):
    
    dim=self.dim()
    
    # in zero dimension you can have only a point in zero!
    if (dim==0): 
      return MkPol()
      
    # already calculated
    if (self.__cached_simplicial_form__):
      return self.__cached_simplicial_form__
 
    POINTDB   = {}
    SIMPLICES = []      
  
    for hull in self.hulls:

      # already triangulated (hoping it's full dimensional, cannot be sure)            
      if (len(hull)<=(dim+1)):
        points    = [self.points[I] for I in hull]
        simplices = [range(len(points))]
        
      # special case for 1D
      elif (dim==1):
        box       = Box(1).addPoints([self.points[I] for I in hull])
        points    = [[box.p1],[box.p2]]
        simplices = [[0,1]]
        
      # all other cases
      else:
        delaunay  = scipy.spatial.Delaunay([self.points[I] for I in hull])
        points    = delaunay.points
        simplices = delaunay.vertices
      
      mapped={}
      for P in range(len(points)):
        point = tuple(points[P])
        if not point in POINTDB: POINTDB[point]=len(POINTDB)
        mapped[P]=POINTDB[point]
          
      for simplex in simplices:
        SIMPLICES.append([mapped[I] for I in simplex])

    POINTS=[None]*len(POINTDB)
    for point,num in POINTDB.iteritems(): 
      POINTS[num]=point
      
    # fix orientation of triangles on the plane (important for a coherent orientation)
    if (dim==2):
      fixed_orientation=[]
      for simplex in SIMPLICES:
        if len(simplex)==3:
          p0=POINTS[simplex[0]]
          p1=POINTS[simplex[1]]
          p2=POINTS[simplex[2]]
          n=Batch3D.computeNormal(p0,p1,p2)
          if n[2]<0:
            simplex=[simplex[2],simplex[1],simplex[0]]
        fixed_orientation.append(simplex)
      SIMPLICES=fixed_orientation
      
    # fix orientation of tetahedra
    if (dim==3):
      fixed_orientation=[]
      for simplex in SIMPLICES:
        if len(simplex)==4:
          p0=POINTS[simplex[0]]
          p1=POINTS[simplex[1]]
          p2=POINTS[simplex[2]]
          p3=POINTS[simplex[3]]
          if not Batch3D.goodTetOrientation(p0,p1,p2,p3): 
            simplex=[simplex[2],simplex[1],simplex[0],simplex[3]]
        fixed_orientation.append(simplex)
      SIMPLICES=fixed_orientation       
      
    # store for the next time!
    self.__cached_simplicial_form__=MkPol(POINTS,SIMPLICES) 
    return self.__cached_simplicial_form__
   
  # getBatches
  def getBatches(self):
    
    dim=self.dim()
          
    SF=self.toSimplicialForm()
    
    # export point, lines, triangles
    points    = Batch3D(Batch3D.POINTS   ); points   .vertices=[]
    lines     = Batch3D(Batch3D.LINES    ); lines    .vertices=[]
    triangles = Batch3D(Batch3D.TRIANGLES); triangles.vertices=[]; triangles.normals=[]
    
    for hull in SF.hulls:
      
      hull_dim=len(hull)
      
      if (hull_dim==1):
        p0=SF.points[hull[0]]; p0=list(p0) + [0.0]*(3-len(p0))
        points.vertices.append(p0) 
        
      elif (hull_dim==2):
        p0=SF.points[hull[0]]; p0=list(p0) + [0.0]*(3-len(p0))
        p1=SF.points[hull[1]]; p1=list(p1) + [0.0]*(3-len(p1)) 
        lines.vertices.append(p0);
        lines.vertices.append(p1)
        
      elif (hull_dim==3):
        p0=SF.points[hull[0]]; p0=list(p0) + [0.0]*(3-len(p0)) 
        p1=SF.points[hull[1]]; p1=list(p1) + [0.0]*(3-len(p1))
        p2=SF.points[hull[2]]; p2=list(p2) + [0.0]*(3-len(p2))
        n=Batch3D.computeNormal(p0,p1,p2)
        triangles.vertices.append(p0); triangles.normals.append(n)
        triangles.vertices.append(p1); triangles.normals.append(n)
        triangles.vertices.append(p2); triangles.normals.append(n)    

      elif (hull_dim==4):
        for T in Batch3D.TET_ORIENTED_TRIANGLES:
          p0=SF.points[hull[T[0]]]; p0=list(p0) + [0.0]*(3-len(p0)) 
          p1=SF.points[hull[T[1]]]; p1=list(p1) + [0.0]*(3-len(p1))
          p2=SF.points[hull[T[2]]]; p2=list(p2) + [0.0]*(3-len(p2))
          n=Batch3D.computeNormal(p0,p1,p2)
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

    
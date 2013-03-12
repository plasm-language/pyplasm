import sys

class Box:
  
  # constructor
  def __init__(self,*arg):
    if len(arg)==1 and isinstance(arg[0],int):
      self.p1=[sys.float_info.max] * arg[0]
      self.p2=[sys.float_info.min] * arg[0]
      
    elif len(arg)==2 and isinstance(arg[0],(list, tuple)) and isinstance(arg[1],(list, tuple)):
      assert(len(arg[0])==len(arg[1]))
      self.p1=arg[0]
      self.p2=arg[1]
     
    else:
      raise Exception("Invalid constructor call")

  # valid
  def valid(self):
    for i in range(self.dim()):
      if (self.p1[i]>self.p2[i]): return False
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
      return 'Box('+repr(self.p1)+', '+repr(self.p2)+')'
    
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
  
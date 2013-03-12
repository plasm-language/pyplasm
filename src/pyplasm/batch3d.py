from OpenGL.GL      import *
from OpenGL.GLU     import *

from PyQt4.QtGui    import *
from PyQt4.QtCore   import *
from PyQt4.QtOpenGL import *

from matrix import *
from box    import *

# ////////////////////////////////////////////
# GLVertexBuffer
# ////////////////////////////////////////////
class GLVertexBuffer:

  destroy_list = []

  def __init__(self, data,usage=GL_STATIC_DRAW):
    data=reduce(lambda x, y: x + y, data)
    self.id = glGenBuffers (1)
    glBindBuffer (GL_ARRAY_BUFFER, self.id)
    glBufferData (GL_ARRAY_BUFFER, len(data)*4, numpy.array(data, dtype="float32"), usage)
    glBindBuffer(GL_ARRAY_BUFFER, 0)
   
  def __del__(self):
    GLVertexBuffer.destroy_list.append(self.id)


  
# ////////////////////////////////////////////
# GLVertexBuffer
# ////////////////////////////////////////////
class GLTexture:

  destroy_list = []

  def __init__(self, texture):
    self.id=glGenTextures(1)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
    glBindTexture(GL_TEXTURE_2D, self.id)
    format=(GL_RGB if texture.bpp==24 else (GL_RGBA if texture.bpp==32 else GL_LUMINANCE))
    gluBuild2DMipmaps(GL_TEXTURE_2D,texture.bpp/8,texture.width, texture.height,format,GL_UNSIGNED_BYTE,texture.buffer)
    glBindTexture(GL_TEXTURE_2D,0)
   
  def __del__(self):
    GLTexture.destroy_list.append(self.id)



# ////////////////////////////////////////////
# Batch3D class
# ////////////////////////////////////////////
class Batch3D:
  
  POINTS         = 0x0000
  LINES          = 0x0001
  TRIANGLES      = 0x0004

  def __init__(self,primitive=POINTS):
    self.primitive=primitive
    self.T=Matrix(4) 
    self.texture0      =None; self.gpu_texture0      =None;
    self.texture1      =None; self.gpu_texture1      =None;
    self.vertices      =None; self.gpu_vertices      =None
    self.normals       =None; self.gpu_normals       =None
    self.colors        =None; self.gpu_colors        =None   
    self.texture0coords=None; self.gpu_texture0coords=None 
    self.texture1coords=None; self.gpu_texture1coords=None
    self.ambient  =(0.2,0.2,0.2,1.0) 
    self.diffuse  =(0.8,0.8,0.8,1.0) 
    self.specular =(0.1,0.1,0.1,1.0)
    self.emission =(0.0,0.0,0.0,0.0)
    self.shininess=100.0
    self.properties={}
    
  # __str__
  def __str__(self):
    return str({"primitive":self.primitive,"vertices":self.vertices,"normals":self.normals})       

  # computeNormal
  @staticmethod  
  def computeNormal(p0,p1,p2):
    p0=list(p0) + [0]*(3-len(p0))
    p1=list(p1) + [0]*(3-len(p1))
    p2=list(p2) + [0]*(3-len(p2))
    n=QVector3D.crossProduct(QVector3D(*p1)-QVector3D(*p0),QVector3D(*p2)-QVector3D(*p0)).normalized()
    return [n.x(),n.y(),n.z()]    
    
  # computeTetOrientation
  #  see http://math.stackexchange.com/questions/183030/given-a-tetrahedron-how-to-find-the-outward-surface-normals-for-each-side)
  #  see http://www.geuz.org/pipermail/gmsh/2012/007251.html
  TET_ORIENTED_TRIANGLES=[[0,1,3],[0,3,2],[0,2,1],[1,2,3]]
  @staticmethod  
  def goodTetOrientation(v0,v1,v2,v3):
    v0=list(v0) + [0]*(3-len(v0));v0=QVector3D(*v0)
    v1=list(v1) + [0]*(3-len(v1));v1=QVector3D(*v1)
    v2=list(v2) + [0]*(3-len(v2));v2=QVector3D(*v2)
    v3=list(v3) + [0]*(3-len(v3));v3=QVector3D(*v3)
    n=QVector3D.crossProduct(v3-v1,v2-v1)
    return QVector3D.dotProduct(n,v0-v1)>0
    
  # dim  
  def dim(self):
    return 3  
    
  # box
  def box(self):
    return Box(3).addPoints([self.T.transformPoint(p) for p in self.vertices])
    
  # prependTransformation
  def prependTransformation(self,T):
    self.T=T * self.T
    
  # writeProperties
  def writeProperties(self,properties):
    for key,value in properties.iteritems():
      self.properties[key]=value
 
  # render
  def render(self):
    
    # destruction of old vertex buffers
    for it in GLVertexBuffer.destroy_list:
      glDeleteBuffers(1, GLuint(it))   
    GLVertexBuffer.destroy_list=[]
      
    # destruction of old textures
    for it in GLTexture.destroy_list:
      glDeleteTextures(1, GLuint(it))  
    GLTexture.destroy_list=[]      
    
    if (self.colors is None):
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , self.ambient)
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , self.diffuse)
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , self.specular)
      glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, self.shininess )
      if (self.diffuse[3]<1):
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
       
    if (not self.vertices is None):
      if self.gpu_vertices is None: self.gpu_vertices=GLVertexBuffer(self.vertices)
      glBindBuffer(GL_ARRAY_BUFFER, self.gpu_vertices.id)
      glVertexPointer(3,GL_FLOAT,0,None)
      glBindBuffer(GL_ARRAY_BUFFER,0)  
      glEnableClientState(GL_VERTEX_ARRAY)
  
    if (not  self.normals  is None):
      if self.gpu_normals is None: self.gpu_normals=GLVertexBuffer(self.normals)
      glBindBuffer(GL_ARRAY_BUFFER, self.gpu_normals.id)
      glNormalPointer(GL_FLOAT,0,None)
      glBindBuffer(GL_ARRAY_BUFFER,0)  
      glEnableClientState(GL_NORMAL_ARRAY)      
  
    if (not  self.colors  is None):
      if self.gpu_colors is None: self.gpu_colors=GLVertexBuffer(self.colors)
      glBindBuffer(GL_ARRAY_BUFFER, self.gpu_colors.id)
      glColorPointer(3,GL_FLOAT,0,None)
      glBindBuffer(GL_ARRAY_BUFFER,0)  
      glEnableClientState(GL_COLOR_ARRAY)
      glEnable(GL_COLOR_MATERIAL)
  
    if (self.texture0 and self.texture0coords):
      glColor4f(1,1,1,1)
      if (self.gpu_texture0 is None): self.gpu_texture0=GLTexture(self.texture0)
  
      glActiveTexture       (GL_TEXTURE0)
      glClientActiveTexture (GL_TEXTURE0)
      glBindTexture         (GL_TEXTURE_2D, self.gpu_texture0.id)
      glEnable              (GL_TEXTURE_2D)
      
      if self.gpu_texture0coords is None: self.gpu_texture0coords=GLVertexBuffer(self.gpu_texture0coords)
      glBindBuffer(GL_ARRAY_BUFFER, self.gpu_texture0coords.id)
      glTexCoordPointer(2,GL_FLOAT,0,None)
      glBindBuffer(GL_ARRAY_BUFFER,0)
      glEnableClientState(GL_TEXTURE_COORD_ARRAY)
  
    if (self.texture1 and self.texture1coords):
      glDisable(GL_LIGHTING)
      glColor3f(1,1,1)
      if (self.gpu_texture1 is None): self.gpu_texture1=GLTexture(self.texture1)
      glActiveTexture       (GL_TEXTURE1)
      glClientActiveTexture (GL_TEXTURE1)
      glBindTexture         (GL_TEXTURE_2D, self.gpu_texture1.id)
      glEnable              (GL_TEXTURE_2D)
      
      if self.gpu_texture1coords is None: self.gpu_texture1coords=GLVertexBuffer(self.gpu_texture1coords)
      glBindBuffer(GL_ARRAY_BUFFER, self.gpu_texture1coords.id)
      glTexCoordPointer(2,GL_FLOAT,0,None)
      glBindBuffer(GL_ARRAY_BUFFER,0)
      glEnableClientState(GL_TEXTURE_COORD_ARRAY)      
      
      glActiveTexture       (GL_TEXTURE0)
      glClientActiveTexture (GL_TEXTURE0)
  
    glPushMatrix()
    glMultMatrixf(self.T.transpose().toList())
    num_vertices=len(self.vertices)
    glDrawArrays(self.primitive, 0, num_vertices)
    glPopMatrix()
  
    if (self.diffuse[3]<1) :
      glDisable(GL_BLEND)
  
    if (self.texture1 and self.texture1coords):
      glActiveTexture(GL_TEXTURE1)
      glClientActiveTexture(GL_TEXTURE1)
      glDisableClientState(GL_TEXTURE_COORD_ARRAY)
      glDisable(GL_TEXTURE_2D)
      glEnable(GL_LIGHTING)
      glActiveTexture(GL_TEXTURE0)
      glClientActiveTexture(GL_TEXTURE0)
  
    if (self.texture0 and self.texture0coords):
      glActiveTexture(GL_TEXTURE0)
      glClientActiveTexture(GL_TEXTURE0)
      glDisableClientState(GL_TEXTURE_COORD_ARRAY)
      glDisable(GL_TEXTURE_2D)
  
    if (self.colors):
      glDisableClientState(GL_COLOR_ARRAY)
      glDisable(GL_COLOR_MATERIAL)
  
    if (self.normals) :
      glDisableClientState(GL_NORMAL_ARRAY)
  
    if (self.vertices):
      glDisableClientState(GL_VERTEX_ARRAY)
          

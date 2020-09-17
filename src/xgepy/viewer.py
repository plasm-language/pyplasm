import signal
import math
import ctypes

from pyplasm        import *
from OpenGL.GL      import *
from PyQt4.QtCore   import *
from PyQt4.QtGui    import *
from PyQt4.QtOpenGL import *


# ///////////////////////////////////////////////////////////////////////////////
class QtViewer(QGLWidget):
  
  # ////////////////////////////////////////////
  # constructor
  # ////////////////////////////////////////////
  def __init__(self, parent = None):
    super(QtViewer, self).__init__(parent)
    self.draw_lines=False
    self.draw_axis=True
    self.mouse_x=0
    self.mouse_y=0
    self.trackball_mode=True
    self.trackball_center=Vec3f(0,0,0)
    self.frustum=Frustum()
    self.frustum.guessBestPosition(Box3f(Vec3f(-1,-1,-1),Vec3f(+1,+1,+1)))
    self.octree=None
    self.resize (1024,768)   
    #self.setMouseTracking(True)
  
  # ////////////////////////////////////////////
  # initializeGL
  # ////////////////////////////////////////////
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
    glLightfv(GL_LIGHT0,GL_AMBIENT  , [+1.00,+1.00,+1.00,+1.00])
    glLightfv(GL_LIGHT0,GL_DIFFUSE  , [+1.00,+1.00,+1.00,+1.00])
    glLightfv(GL_LIGHT0,GL_SPECULAR , [+1.00,+1.00,+1.00,+1.00])
    #glLightfv(GL_LIGHT0,GL_EMISSION , [+1.00,+1.00,+1.00,+1.00])

    glActiveTexture       (GL_TEXTURE1)
    glClientActiveTexture (GL_TEXTURE1)
    glTexParameteri       (GL_TEXTURE_2D , GL_TEXTURE_WRAP_S     ,GL_REPEAT)
    glTexParameteri       (GL_TEXTURE_2D , GL_TEXTURE_WRAP_T     ,GL_REPEAT)
    glTexParameterf       (GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,GL_LINEAR)
    glTexEnvf             (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE   ,GL_MODULATE)
    glTexParameteri       (GL_TEXTURE_2D  ,GL_TEXTURE_MIN_FILTER ,GL_NEAREST_MIPMAP_LINEAR)

    glActiveTexture       (GL_TEXTURE0)
    glClientActiveTexture (GL_TEXTURE0)
    glTexParameteri       (GL_TEXTURE_2D , GL_TEXTURE_WRAP_S     ,GL_REPEAT)
    glTexParameteri       (GL_TEXTURE_2D , GL_TEXTURE_WRAP_T     ,GL_REPEAT)
    glTexParameterf       (GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,GL_LINEAR)
    glTexEnvf             (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE   ,GL_MODULATE)
    glTexParameteri       (GL_TEXTURE_2D  ,GL_TEXTURE_MIN_FILTER ,GL_NEAREST_MIPMAP_LINEAR)   
    
  # ////////////////////////////////////////////
  # redisplay
  # ////////////////////////////////////////////
  def redisplay(self):
    self.frustum.refresh()
    self.update()    
  
  # ////////////////////////////////////////////
  # resizeGL
  # ////////////////////////////////////////////
  def resizeGL(self, width, height):
    
    if (width==0 or height==0):
      return
  
    self.frustum.x=0
    self.frustum.y=0
    self.frustum.width =width
    self.frustum.height=height
  
    zNear=0.0010
    zFar =1000.0
    
    if (not self.octree is None):
      maxdim = self.octree.world_box.maxsize()
      zNear  = maxdim / 50.0 
      zFar   = maxdim * 10
  
    self.frustum.projection_matrix=Mat4f.perspective(DEFAULT_FOV,width/float(height),zNear,zFar)
    self.redisplay()     
  
      
  # ////////////////////////////////////////////
  # setOctree
  # ////////////////////////////////////////////
  def setOctree(self,octree):
    self.octree=octree
    if (octree is None): return
    self.trackball_center =self.octree.world_box.center()
    self.frustum.guessBestPosition(self.octree.world_box)
    self.redisplay()
      
  # ////////////////////////////////////////////
  # createTexture
  # ////////////////////////////////////////////
  def createTexture(self,texture):
    if (texture.gpu): return
    texid=glGenTextures(1)
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1)
    glBindTexture (GL_TEXTURE_2D, texid)
    format=(GL_RGB if texture.bpp==24 else (GL_RGBA if texture.bpp==32 else GL_LUMINANCE))
    gluBuild2DMipmaps(GL_TEXTURE_2D,texture.bpp/8,texture.width, texture.height,format, GL_UNSIGNED_BYTE, texture.buffer)
    texture.gpu=Texture.Gpu(texid)	
 
  # ////////////////////////////////////////////
  # destroyTextureGpu
  # ////////////////////////////////////////////
  def destroyTextureGpu(self,texid):
    glDeleteTextures(1,texid)
  
  # ////////////////////////////////////////////
  # mousePressEvent
  # ////////////////////////////////////////////
  def mousePressEvent(self,event):
    self.mouse_x = event.x()
    self.mouse_y = event.y()
    
  
  # ////////////////////////////////////////////
  # mouseMoveEvent
  # ////////////////////////////////////////////
  def mouseMoveEvent(self,event):
  
    x=event.x()
    y=event.y()
    button=event.buttons() 
  
    if (button==Qt.NoButton):
      self.mouse_x = x
      self.mouse_y = y    
      return

    if (not self.trackball_mode):
      dx=0.5*math.pi*(x-self.mouse_x)/float(self.frustum.width )
      dy=0.5*math.pi*(y-self.mouse_y)/float(self.frustum.height)
  
      if (button==Qt.LeftButton):
  	    dir=self.frustum.dir.rotate(self.frustum.vup,-dx)
  	    rot_axis=dir.cross(self.frustum.vup).normalize()
  	    dir=dir.rotate(rot_axis,-dy).normalize()
  	    self.frustum.dir=dir
  	    
      elif (button==Qt.MiddleButton):
  	    self.frustum.pos=self.frustum.pos + self.frustum.vup*-dy*self.frustum.walk_speed*10 + self.frustum.right*dx*self.frustum.walk_speed*10
  
      self.redisplay()
      
    else:
      vmat=Mat4f.lookat(
        self.frustum.pos.x                   ,self.frustum.pos.y                   ,self.frustum.pos.z,
        self.frustum.pos.x+self.frustum.dir.x,self.frustum.pos.y+self.frustum.dir.y,self.frustum.pos.z+self.frustum.dir.z,
        self.frustum.vup.x                   ,self.frustum.vup.y                   ,self.frustum.vup.z) * Mat4f.translate(self.trackball_center*+1)
  
      rotation=Quaternion(vmat)
      translate=Vec3f(vmat[3],vmat[7],vmat[11])-self.trackball_center
  
      deltax = float(x - self.mouse_x)   
      deltay = float(self.mouse_y - y)
      W=self.frustum.width
      H=self.frustum.height
  
      if (button==Qt.LeftButton):
  	    Min = (W if W<=H else H)*0.5
  	    offset=Vec3f(W/2.0, H/2.0, 0)
  	    a=(Vec3f(float(self.mouse_x), float(H-self.mouse_y), 0)-offset)*(1/Min)
  	    b=(Vec3f(float(           x), float(H-           y), 0)-offset)*(1/Min)
  	    a.set(2, pow(2.0, -0.5 * a.module()))
  	    b.set(2, pow(2.0, -0.5 * b.module()))
  	    a = a.normalize()
  	    b = b.normalize()
  	    axis = a.cross(b).normalize()
  	    angle = math.acos(a*b)
  	    TRACKBALLSCALE=1.0
  	    rotation = Quaternion(axis, angle * TRACKBALLSCALE) * rotation
  
      elif (button==Qt.MiddleButton):
  	    translate -= Vec3f(0,0,deltay) * self.frustum.walk_speed
  
      elif (button==Qt.RightButton):
  	    translate += Vec3f(deltax, deltay, 0) * self.frustum.walk_speed
  
      vmat=Mat4f.translate(translate) * \
           Mat4f.translate(self.trackball_center*+1)* \
           Mat4f.rotate(rotation.getAxis(),rotation.getAngle())* \
           Mat4f.translate(self.trackball_center*-1)
  
      vmat=vmat.invert()	
      self.frustum.pos=Vec3f(  vmat[3], vmat[7], vmat[11])
      self.frustum.dir=Vec3f( -vmat[2],-vmat[6],-vmat[10])
      self.frustum.vup=Vec3f(  vmat[1], vmat[5], vmat[ 9])
  
      self.redisplay()
  
    self.mouse_x = x
    self.mouse_y = y
    
  # ////////////////////////////////////////////
  # mouseReleaseEvent
  # ////////////////////////////////////////////
  def mouseReleaseEvent(self,event):
    self.mouse_x = event.x()
    self.mouse_y = event.y()    
  
  # ////////////////////////////////////////////
  # wheelEvent
  # ////////////////////////////////////////////
  def wheelEvent(self,event):
    K=(+1 if event.delta()>0 else -1)*self.frustum.walk_speed
    self.frustum.pos=self.frustum.pos + self.frustum.dir * K
    self.redisplay()  
  
  # ////////////////////////////////////////////
  # keyPressEvent
  # ////////////////////////////////////////////
  def keyPressEvent(self,event):
    
    key=event.key()
    x,y=self.width()/2,self.height()/2
    
    if (key==Qt.Key_Escape):
      self.close()
      return 

    if (key==Qt.Key_Plus or key==Qt.Key_Equal):
      self.frustum.walk_speed*=0.95
      return 
  
    if (key==Qt.Key_Minus or key==Qt.Key_Underscore):
      self.frustum.walk_speed*=(1.0/0.95)
      return 
  
    if (key==Qt.Key_X):
      self.draw_axis=not self.draw_axis
      self.redisplay()
      return 
  
    if (key==Qt.Key_L):
      self.draw_lines=not self.draw_lines
      self.redisplay()
      return 
  
    if (key==Qt.Key_F):
        if (self.debug_frustum is None):
          self.debug_frustum=Frustum(self.frustum)
        else:
          self.debug_frustum=None
  
        self.redisplay()
        return 
        
    if (key==Qt.Key_Space):
        self.trackball_mode=not self.trackball_mode
        if (not self.trackball_mode): self.frustum.fixVup()
        self.redisplay()
        return 
  
    if (key==Qt.Key_W):
        ray=self.frustum.unproject(x,y)
        self.frustum.pos+=ray.dir*self.frustum.walk_speed
        self.redisplay()
        return 
  
    if (key==Qt.Key_S):
        ray=self.frustum.unproject(x,y)
        self.frustum.pos-=ray.dir*self.frustum.walk_speed
        self.redisplay()
        return 
        
    if (key==Qt.Key_A  or key==Qt.Key_Left):
      self.frustum.pos-=self.frustum.right*self.frustum.walk_speed
      self.redisplay()
      return 
  
    if (key==Qt.Key_D or key==Qt.Key_Right):
      self.frustum.pos+=self.frustum.right*self.frustum.walk_speed
      self.redisplay()
      return         
  
    if (key==Qt.Key_Up):
      self.frustum.pos+=self.frustum.vup*self.frustum.walk_speed
      self.redisplay()
      return 
  
    if (key==Qt.Key_Down):
      self.frustum.pos-=self.frustum.vup*self.frustum.walk_speed
      self.redisplay()
      return 

  # ////////////////////////////////////////////
  # C_PTR
  # ////////////////////////////////////////////
  def C_PTR(self,array):
    return ctypes.cast (array.c_ptr().__long__ (), ctypes.POINTER(ctypes.c_float))
 

  # ////////////////////////////////////////////
  # renderBatch
  # ////////////////////////////////////////////
  def renderBatch(self,batch):
  
    if (batch.colors is None):
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , [batch.ambient .r,batch.ambient .g,batch.ambient .b,batch.ambient .a])
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , [batch.diffuse .r,batch.diffuse .g,batch.diffuse .b,batch.diffuse .a] )
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , [batch.specular.r,batch.specular.g,batch.specular.b,batch.specular.a])
      #glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION , [batch.emission.r,batch.emission.g,batch.emission.b,batch.emission.a])
      glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS,  batch.shininess )
      if (batch.diffuse.a<1):
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
       

    if (not batch.vertices  is None):
      glVertexPointer(3, GL_FLOAT, 0,self.C_PTR(batch.vertices))
      glEnableClientState(GL_VERTEX_ARRAY)
  
    if (not  batch.normals  is None):
      glNormalPointer(GL_FLOAT, 0, self.C_PTR(batch.normals))
      glEnableClientState(GL_NORMAL_ARRAY)
  
    if (not  batch.colors  is None):
      glColorPointer(3,GL_FLOAT, 0, self.C_PTR(batch.colors))
      glEnableClientState(GL_COLOR_ARRAY)
      glEnable(GL_COLOR_MATERIAL)
  
    if (batch.texture0 and batch.texture0coords):
      
      glColor4f(1,1,1,1)
  
      if (batch.texture0.gpu is None):
        createTexture(batch.texture0)
  
      glActiveTexture       (GL_TEXTURE0)
      glClientActiveTexture (GL_TEXTURE0)
      glBindTexture         (GL_TEXTURE_2D, batch.texture0.gpu.id)
      glEnable(GL_TEXTURE_2D)
      glTexCoordPointer (2, GL_FLOAT, 0,  self.C_PTR(batch.texture0coords))
      glEnableClientState(GL_TEXTURE_COORD_ARRAY)
  
  
    if (batch.texture1 and batch.texture1coords):
      
      glDisable(GL_LIGHTING)
      glColor3f(1,1,1)
  
      if (batch.texture1.gpu is None):
        createTexture(batch.texture1)
          
      glActiveTexture       (GL_TEXTURE1)
      glClientActiveTexture (GL_TEXTURE1)
      glBindTexture         (GL_TEXTURE_2D, batch.texture1.gpu.id)
      glEnable              (GL_TEXTURE_2D)
      glTexCoordPointer     (2, GL_FLOAT, 0,  self.C_PTR(batch.texture1coords))
      glEnableClientState   (GL_TEXTURE_COORD_ARRAY)
      glActiveTexture       (GL_TEXTURE0)
      glClientActiveTexture (GL_TEXTURE0)
  
    glPushMatrix()

    glMultMatrixf([batch.matrix[0],batch.matrix[4],batch.matrix[ 8],batch.matrix[12],
                   batch.matrix[1],batch.matrix[5],batch.matrix[ 9],batch.matrix[13],
                   batch.matrix[2],batch.matrix[6],batch.matrix[10],batch.matrix[14],
                   batch.matrix[3],batch.matrix[7],batch.matrix[11],batch.matrix[15]])
  
    num_vertices=batch.vertices.size()/3
    glDrawArrays(batch.primitive, 0, num_vertices)
    glPopMatrix()
  
    if (batch.diffuse[3]<1) :
      glDisable(GL_BLEND)
  
    if (batch.texture1 and batch.texture1coords):
      glActiveTexture(GL_TEXTURE1)
      glClientActiveTexture(GL_TEXTURE1)
      glDisableClientState(GL_TEXTURE_COORD_ARRAY)
      glDisable(GL_TEXTURE_2D)
      glEnable(GL_LIGHTING)
      glActiveTexture(GL_TEXTURE0)
      glClientActiveTexture(GL_TEXTURE0)
  
    if (batch.texture0 and batch.texture0coords):
      glActiveTexture(GL_TEXTURE0)
      glClientActiveTexture(GL_TEXTURE0)
      glDisableClientState(GL_TEXTURE_COORD_ARRAY)
      glDisable(GL_TEXTURE_2D)
  
    if (batch.colors):
      glDisableClientState(GL_COLOR_ARRAY)
      glDisable(GL_COLOR_MATERIAL)
  
    if (batch.normals) :
      glDisableClientState(GL_NORMAL_ARRAY)
  
    if (batch.vertices):
      glDisableClientState(GL_VERTEX_ARRAY)
      
    if (self.draw_lines and batch.primitive>=Batch.TRIANGLES):
      glDepthMask(False) 
      glLineWidth(2)
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE)
      save_ambient=batch.ambient
      save_diffuse=batch.diffuse
      batch.setColor(Color4f(0,0,0,0.05))
      self.draw_lines=False
      self.renderBatch(batch)
      self.draw_lines=True
      batch.ambient=save_ambient
      batch.diffuse=save_diffuse
      glDepthMask(True)
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL)
      glLineWidth(1)      
  
  # ////////////////////////////////////////////
  # renderOctree
  # ////////////////////////////////////////////
  def renderOctree(self,octree):
    transparents=[]
    #frustum=self.debug_frustum if self.debug_frustum else self.frustum
    frustum=self.frustum

    
    it_frustum=self.octree.find(frustum)
    
    while (not it_frustum.end()):
      node=it_frustum.getNode()
      batches=node.batches

      for batch in batches:
        if (frustum.intersect(batch.getBox())):
          if (batch.diffuse.a<1):
            transparents.append(batch)
          else:
            self.renderBatch(batch)

      it_frustum.moveNext()

    for batch in reversed(transparents):
      self.renderBatch(batch)    
      

  # ////////////////////////////////////////////
  # renderScene
  # ////////////////////////////////////////////
  def paintGL(self):
    
    # set viewpport
    glViewport(self.frustum.x,self.frustum.y,self.frustum.width,self.frustum.height)
    
    # clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    
    # set projection
    glMatrixMode(GL_PROJECTION)
    projection=self.frustum.projection_matrix.transpose();
    glLoadMatrixf([projection[i] for i in range(16)])
    
    # set modelview
    glMatrixMode(GL_MODELVIEW)  
    modelview=self.frustum.getModelviewMatrix().transpose()  
    glLoadMatrixf([modelview[i] for i in range(16)])    

    # draw axis
    if (self.draw_axis):
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
      glLightfv(GL_LIGHT0, GL_POSITION, [self.frustum.pos.x, self.frustum.pos.y, self.frustum.pos.z,1.0])
      glLightfv(GL_LIGHT0, GL_AMBIENT,  [1, 1, 1, 1])
      glLightfv(GL_LIGHT0, GL_DIFFUSE,  [1, 1, 1, 1])
      glLightfv(GL_LIGHT0, GL_SPECULAR, [1, 1, 1, 1])
      #glLightfv(GL_LIGHT0, GL_EMISSION, [1, 1, 1, 1])     
  
    # draw the octree
    if (not self.octree is None):
      self.renderOctree(self.octree)

# QtVIEW
def QtVIEW(obj):
  app = QApplication(["QtViewer"])
  viewer = QtViewer()
  viewer.setWindowTitle('QtViewer')
  octree=Octree(Plasm.getBatches(obj))
  viewer.setOctree(octree)
  viewer.show()
  return app.exec_()

def main():
  QtVIEW(Plasm.cube(3))  

if __name__ == '__main__':
    main()

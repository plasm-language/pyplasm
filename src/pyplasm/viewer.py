import math

from OpenGL.GL      import *
from OpenGL.GLU     import *

from PyQt4.QtGui    import *
from PyQt4.QtCore   import *
from PyQt4.QtOpenGL import *

from box import *



# ///////////////////////////////////////////////////////////////////////////////
class Viewer(QGLWidget):
  
  # ////////////////////////////////////////////
  # constructor
  # ////////////////////////////////////////////
  def __init__(self):
    super(Viewer, self).__init__(None)

  
  # ////////////////////////////////////////////
  # view
  # ////////////////////////////////////////////
  @staticmethod  
  def view(objs):
    app = QApplication(["Viewer"])
    
    # calculate bounding box
    box=Box(3)
    if len(objs):
      for obj in objs:
        box.addBox(obj.box())
   
    if not box.valid():
      box=Box(3)
      box.addPoint([-1,-1,-1])
      box.addPoint([+1,+1,+1])  
    
    viewer=Viewer()
    viewer.setWindowTitle('Viewer')
    viewer.objs=objs
    viewer.resize(1024,768)
    viewer.guessBestPosition(box)  
    viewer.redisplay()  
    viewer.show()
    return app.exec_()      
    
  # ////////////////////////////////////////////
  # guessBestPosition
  # ////////////////////////////////////////////
  def guessBestPosition(self,box):
    p1,p2=QVector3D(box.p1[0],box.p1[1],box.p1[2]),QVector3D(box.p2[0],box.p2[1],box.p2[2])
    self.center = (p1+p2)*0.5
    self.size   = (p2-p1)
    
    self.pos = self.center+self.size*1.5
    self.dir = (self.center-self.pos).normalized()
    self.vup = QVector3D(0,0,1)
    
    maxsize=max([self.size.x(),self.size.y(),self.size.z()])
    self.zNear      = maxsize / 50.0
    self.zFar       = maxsize * 10.0
    self.walk_speed = maxsize / 100.0  
    
  # ////////////////////////////////////////////     
  # getModelviewMatrix
  # ////////////////////////////////////////////  
  def getModelviewMatrix(self):
    # set modelview
    glMatrixMode(GL_MODELVIEW)  
    glPushMatrix()
    glLoadIdentity()
    gluLookAt(self.pos.x()             ,self.pos.y()             ,self.pos.z(),
              self.pos.x()+self.dir.x(),self.pos.y()+self.dir.y(),self.pos.z()+self.dir.z(),
              self.vup.x()             ,self.vup.y()             ,self.vup.z())  
    ret = glGetDoublev(GL_MODELVIEW_MATRIX)   
    glPopMatrix()
    return ret
      
  # ////////////////////////////////////////////      
  # getProjectionMatrix
  # ////////////////////////////////////////////  
  def getProjectionMatrix(self):
    glMatrixMode(GL_PROJECTION)
    glPushMatrix()
    glLoadIdentity()
    gluPerspective(60.0,self.W/float(self.H),self.zNear,self.zFar)
    ret  = glGetDoublev(GL_PROJECTION_MATRIX)
    glPopMatrix()  
    return ret    
  

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
    self.right=QVector3D.crossProduct(self.dir,self.vup).normalized()
    self.update()     
  
  # ////////////////////////////////////////////
  # resizeGL
  # ////////////////////////////////////////////
  def resizeGL(self, width, height):
    if (width==0 or height==0): return
    self.W = width
    self.H = height
    maxdim = max([self.size.x(),self.size.y(),self.size.z()])
    self.zNear  = maxdim / 50.0 
    self.zFar   = maxdim * 10
    self.redisplay()     
    
  # ////////////////////////////////////////////
  # mousePressEvent
  # ////////////////////////////////////////////
  def mousePressEvent(self,event):
    self.mouse_beginx = event.x()
    self.mouse_beginy = event.y()
    
  # ////////////////////////////////////////////
  # mouseMoveEvent
  # ////////////////////////////////////////////
  def mouseMoveEvent(self,event):
  
    x=event.x()
    y=event.y()
    button=event.buttons() 
  
    if (button==Qt.NoButton):
      self.mouse_beginx = x
      self.mouse_beginy = y    
      return
      
    vmat=QMatrix4x4()
    
    vmat.lookAt(self.pos,self.pos+self.dir,self.vup)
    vmat.translate(self.center.x(),self.center.y(),self.center.z())
    

    kRot=[[vmat[0,0],vmat[0,1],vmat[0,2]],
          [vmat[1,0],vmat[1,1],vmat[1,2]],
          [vmat[2,0],vmat[2,1],vmat[2,2]]]
    fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2]
    if (fTrace>0.0):
      fRoot = math.sqrt(fTrace + 1.0)
      w = 0.5*fRoot
      fRoot = 0.5/fRoot
      x = (kRot[2][1]-kRot[1][2])*fRoot
      y = (kRot[0][2]-kRot[2][0])*fRoot
      z = (kRot[1][0]-kRot[0][1])*fRoot
      rotation=QQuaternion(w,x,y,z)
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
      rotation=QQuaternion(w,Q[0],Q[1],Q[2])

    translate=QVector3D(vmat[0,3],vmat[1,3],vmat[2,3])-self.center

    deltax = float(x - self.mouse_beginx)   
    deltay = float(self.mouse_beginy - y)
    W=self.W
    H=self.H

    if (button==Qt.LeftButton):
	    Min = 0.5 * min([W,H])
	    offset=QVector3D(W/2.0, H/2.0, 0)
	    a=(QVector3D((float)(self.mouse_beginx), (float)(H-self.mouse_beginy), 0)-offset)/Min
	    b=(QVector3D((float)(                x), (float)(H-                y), 0)-offset)/Min
	    a.setZ(math.pow(2.0, -0.5 * a.length()))
	    b.setZ(math.pow(2.0, -0.5 * b.length()))
	    a = a.normalized()
	    b = b.normalized()
	    axis = QVector3D.crossProduct(a,b).normalized()
	    angle = math.acos(QVector3D.dotProduct(a,b))
	    rotation = QQuaternion.fromAxisAndAngle(axis, 180.0*angle/math.pi) * rotation

    elif (button==Qt.MiddleButton):
	    translate -= QVector3D(0,0,deltay) * self.walk_speed

    elif (button==Qt.RightButton):
	    translate += QVector3D(deltax,deltay,0) * self.walk_speed
	    
    vmat=QMatrix4x4()
    vmat.translate(translate.x(),translate.y(),translate.z())
    vmat.translate(self.center.x(),self.center.y(),self.center.z())
    vmat.rotate(rotation)
    vmat.translate(-self.center.x(),-self.center.y(),-self.center.z())

    vmat=vmat.inverted()[0]	
    self.pos=QVector3D(  vmat[0,3], vmat[1,3], vmat[2,3])
    self.dir=QVector3D( -vmat[0,2],-vmat[1,2],-vmat[2,2])
    self.vup=QVector3D(  vmat[0,1], vmat[1,1], vmat[2,1])

    self.mouse_beginx = x;
    self.mouse_beginy = y;
    self.redisplay()
    
  # ////////////////////////////////////////////
  # mouseReleaseEvent
  # ////////////////////////////////////////////
  def mouseReleaseEvent(self,event):
    self.mouse_beginx = event.x()
    self.mouse_beginy = event.y()    
  
  # ////////////////////////////////////////////
  # wheelEvent
  # ////////////////////////////////////////////
  def wheelEvent(self,event):
    K=(+1 if event.delta()>0 else -1)*self.walk_speed
    self.pos=self.pos+self.dir*K
    self.redisplay()  

  # ////////////////////////////////////////////
  # unproject
  # ////////////////////////////////////////////  
  def unproject(self,x,y):
    model=self.getModelviewMatrix ()
    proj =self.getProjectionMatrix()
    viewport = [ 0, 0, self.W, self.H]
    P1 = gluUnProject(x,self.H-y,-1,model,proj,viewport)    
    P2 = gluUnProject(x,self.H-y,+1,model,proj,viewport)   
    P1=QVector3D(P1[0],P1[1],P1[2])
    P2=QVector3D(P2[0],P2[1],P2[2])
    return (P2-P1).normalized() 
      
  # ////////////////////////////////////////////
  # keyPressEvent
  # ////////////////////////////////////////////
  def keyPressEvent(self,event):
    
    key=event.key()
    x,y=0.5*self.W,0.5*self.H
    
    if (key==Qt.Key_Escape):
      self.close()
      return 

    if (key==Qt.Key_Plus or key==Qt.Key_Equal):
      self.walk_speed*=0.95
      return 
  
    if (key==Qt.Key_Minus or key==Qt.Key_Underscore):
      self.walk_speed*=(1.0/0.95)
      return 
  
    if (key==Qt.Key_W):
        dir=self.unproject(x,y)
        self.pos=self.pos+dir*self.walk_speed
        self.redisplay()
        return 
  
    if (key==Qt.Key_S):
        dir=self.unproject(x,y)
        self.pos=self.pos-dir*self.walk_speed
        self.redisplay()
        return 
        
    if (key==Qt.Key_A  or key==Qt.Key_Left):
      self.pos=self.pos-self.right*self.walk_speed
      self.redisplay()
      return 
  
    if (key==Qt.Key_D or key==Qt.Key_Right):
      self.pos=self.pos+self.right*self.walk_speed
      self.redisplay()
      return         
  
    if (key==Qt.Key_Up):
      self.pos=self.pos+self.vup*self.walk_speed
      self.redisplay()
      return 
  
    if (key==Qt.Key_Down):
      self.pos=self.pos-self.vup*self.walk_speed
      self.redisplay()
      return 

  # drawBox  
  def drawBox(self):
    v=[(0,0,0),(1,0,0),(1,1,0),(0,1,0),(0,0,1),(1,0,1),(1,1,1),(0,1,1)]        
    f=[(0,3,2,1),(4,5,6,7),(0,1,5,4),(5,1,2,6),(7,6,2,3),(4,7,3,0)]
    n=[(0,0,-1),(0,0,+1),(0,-1,0),(+1,0,0),(0,+1,0),(-1,0,0)]           
    glBegin(GL_QUADS)
    for i in range(6):
      glNormal3fv(n[i])
      glVertex3fv(v[f[i][0]])
      glVertex3fv(v[f[i][1]])
      glVertex3fv(v[f[i][2]])
      glVertex3fv(v[f[i][3]])
    glEnd()
         
  # ////////////////////////////////////////////
  # renderScene
  # ////////////////////////////////////////////
  def paintGL(self):
    
    # set viewpport
    glViewport(0,0,self.W,self.H)
    
    # clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    
    # set projection
    glMatrixMode(GL_PROJECTION)
    glLoadMatrixf(self.getProjectionMatrix())
    
    # set modelview
    glMatrixMode(GL_MODELVIEW)  
    glLoadMatrixf(self.getModelviewMatrix())

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
      glLightfv(GL_LIGHT0, GL_POSITION, [self.pos.x(),self.pos.y(),self.pos.z(),1.0])
      glLightfv(GL_LIGHT0, GL_AMBIENT,  [1, 1, 1, 1])
      glLightfv(GL_LIGHT0, GL_DIFFUSE,  [1, 1, 1, 1])
      glLightfv(GL_LIGHT0, GL_SPECULAR, [1, 1, 1, 1])
  

    #if len(self.objs)==0:
    #  self.drawBox()
      
    # draw the objs
    if True:
      for obj in self.objs:
        obj.render()
        
     # draw lines
    if True:
      glDisable(GL_LIGHTING)
      glDepthMask(False) 
      glLineWidth(2)
      glColor3f(0.5,0.5,0.5)
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE)
      for obj in self.objs:
        if obj.dim()>=2: obj.render()     
      glDepthMask(True)
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL)
      glLineWidth(1)       


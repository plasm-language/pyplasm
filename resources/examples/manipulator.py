
from pyplasm import *


# //////////////////////////////////////////////////
class PyViewer(GLCanvas):

	def __init__(self):
		GLCanvas.__init__(self)
		self.manipulator=Manipulator()		
		self.box=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		self.T=Mat4f() # identity matrix
		self.manipulator.setObject(self.box,self.T)
		
	def onKeyboard(self,key,x,y):
		if (key==ord('t') or key==ord('T')):
			self.manipulator.setOperation(Manipulator.TRANSLATE)
			self.manipulator.setObject(self.box,self.T)
			self.Redisplay()
			return
		
		if (key==ord('s') or key==ord('S')):
			self.manipulator.setOperation(Manipulator.SCALE)
			self.manipulator.setObject(self.box, self.T)
			self.Redisplay()
			return
		
		if (key == ord('r') or key == ord('R')):
			manipulator.setOperation(Manipulator.ROTATE)
			self.manipulator.setObject(self.box, self.T)
			self.Redisplay()
			return
	
		GLCanvas.onKeyboard(self,key,x,y);
		
	def onMouseDown(self,button,x,y):
		ray=self.frustum.unproject(x,y)
		self.manipulator.onMouseDown(button,x,y,ray)
		self.redisplay()
		
	def onMouseMove(self,button,x,y):
		ray=self.frustum.unproject(x,y)
		self.manipulator.onMouseMove(button,x,y,ray)
		self.redisplay()
		
	def onMouseUp(self,button,x,y):
		ray=self.frustum.unproject(x,y)
		self.manipulator.onMouseUp(button,x,y,ray)
		self.redisplay()				

	def renderOpenGL(self):	
		self.clearScreen()
		self.setViewport(self.frustum.x,self.frustum.y,self.frustum.width,self.frustum.height);
		self.setProjectionMatrix(self.frustum.projection_matrix);
		self.setModelviewMatrix(self.frustum.getModelviewMatrix());
		self.setDefaultLight(self.frustum.pos)		
		batch=Batch.Cube(self.box)
		batch.matrix= self.T * batch.matrix
		self.renderBatch(batch)
		self.manipulator.render(self)


viewer=PyViewer()
viewer.runLoop()

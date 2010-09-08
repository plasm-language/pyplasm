
from pyplasm import *


# //////////////////////////////////////////////////
class PyViewer(Viewer):

	"""
	Questo non puo' funzionare sotto CPYTHON perche' e' multithread (in C)
	e quindi il global lock di python non funzionerebbe!
	"""

	def __init__(self):
		Viewer.__init__(self)
		
		self.manipulator=Manipulator()		
		self.box=Box3f(Vec3f(0,0,0),Vec3f(1,1,1))
		self.T=Mat4f() # identity matrix
		self.manipulator.setObject(self.box,self.T)
		
		
	def Keyboard(self,key,x,y):

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
	
		Viewer.Keyboard(self,key,x,y);
		
	def Mouse(self,args):
		ray=self.frustum.unproject(args.x,args.y)
		self.manipulator.Mouse(args,ray)
		self.Redisplay()

	def Render(self):	
		self.engine.ClearScreen        ()
		self.engine.SetViewport        (self.frustum.x,self.frustum.y,self.frustum.width,self.frustum.height);
		self.engine.SetProjectionMatrix(self.frustum.projection_matrix);
		self.engine.SetModelviewMatrix (self.frustum.getModelviewMatrix());
		self.engine.SetDefaultLight    (self.frustum.pos)		
		batch=Batch.Cube(self.box)
		batch.matrix= self.T * batch.matrix
		self.engine.Render(batch)
		self.manipulator.Render(self.engine)
		self.engine.FlushScreen()


viewer=PyViewer()
viewer.Run()

from pyplasm import *

batches=[]
batches+=Batch.openObj("examples/load_obj.obj")
octree=Octree(batches)
glcanvas=GLCanvas()
glcanvas.setOctree(octree)
glcanvas.runLoop()


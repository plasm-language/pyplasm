from pyplasm import *


# example of batch creation (to call ONLY ONE TIME!!!!)
def createMyBatchOnlyOneTime():

	ret=Batch()

	#triangles
	ret.primitive=Batch.TRIANGLES 
	ret.matrix=Mat4f() #identity matrix

	# here you can setup the material
	ret.ambient    = Color4f(0.1,0.1,0.1,1);#RGBA materials
	ret.diffuse    = Color4f(0.5,0.5,0.5,1); 
	ret.specular   = Color4f(0.8,0.8,0.8,1);
	ret.emission   = Color4f(0.0,0.0,0.0,1);
	ret.shininess   = 100;

	# 3d vertices (example: a triangle has 3 vertices, for a total of 9 floats)
	ret.vertices=Array([0,0,0, 1,0,0, 1,1,0,  0,0,0, 1,1,0,0,1,0]) 

	#3d normals  (example: a triangle has 3 normal vector, for a total of 9 floats)
	ret.normals =Array([0,0,1, 0,0,1, 0,0,1,  0,0,1, 0,0,1,0,0,1]) 

	# you can add also texturing if you want
	# ....

	return ret

#this is the list of batches you want to display
batches=[]

# example of loading an Wavefront *.obj file  (supported also PLY)
# !!!!!!!!!!!!change the path if not correct!!!!!!!!
batches+=Batch.openObj("src/pyplasm/examples/sphere.obj")

# very important to share the GPU memory: build a "main" batch only one time and then customize it...
#... in this way you dont' replicate vertex/normals/textures!!!
main_batch=createMyBatchOnlyOneTime()
batch1=Batch(main_batch); batch1.matrix=Mat4f.translate(2,0,0); batch1.diffuse=Color4f.Yellow()
batch2=Batch(main_batch); batch2.matrix=Mat4f.translate(0,2,0); batch2.diffuse=Color4f.Red()
batches+=[batch1,batch2]

# now if you want to see some Plasm Hpc with these "fast" GPU batches
cube=CUBOID([1,1,1])
batches+=Plasm.getBatches(cube)

# organize the batch in a loose octree
octree=Octree(batches)

# create the viewer and run it
viewer=GLCanvas()
viewer.setOctree(octree)
viewer.runLoop()


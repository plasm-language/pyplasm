

# /////////////////////////////////////////////////////////////////////
mutable struct GLMesh

	primitive::Int32
	T::Matrix4
	vertex_array::GLVertexArray 
	
	vertices::GLVertexBuffer
	normals::GLVertexBuffer
	colors::GLVertexBuffer
	
	# constructor
	function GLMesh()
		ret=new(POINTS,Matrix4(),GLVertexArray(),GLVertexBuffer(),GLVertexBuffer(),GLVertexBuffer())
		finalizer(releaseGpuResources, ret)
		return ret
	end
	
	# constructor
	function GLMesh(primitive)
		ret=new(primitive,Matrix4(),GLVertexArray(),GLVertexBuffer(),GLVertexBuffer(),GLVertexBuffer())
		finalizer(releaseGpuResources, ret)
		return ret
	end
	
end

# /////////////////////////////////////////////////////////////////////
function releaseGpuResources(mesh::GLMesh)
	releaseGpuResources(mesh.vertex_array)
	releaseGpuResources(mesh.vertices)
	releaseGpuResources(mesh.normals)
	releaseGpuResources(mesh.colors)
end



# ///////////////////////////////////////////////////////////////////////
function computeNormal(p0::Point3d,p1::Point3d,p2::Point3d)
	return normalized(cross(p1-p0,p2-p0))
end
	

# ///////////////////////////////////////////////////////////////////////
function getBoundingBox(mesh::GLMesh)
	box=invalidBox()
	vertices=mesh.vertices.vector
	for I in 1:3:length(vertices)
		point=Point3d(vertices[I+0],vertices[I+1],vertices[I+2])
		addPoint(box,point)
	end
	return box
end


# ////////////////////////////////////////////////////////////////////////
function GLCuboid(box::Box3d)
	points=getPoints(box)
	
	faces=[[1, 2, 3, 4],[4, 3, 7, 8],[8, 7, 6, 5],[5, 6, 2, 1],[6, 7, 3, 2],[8, 5, 1, 4]]
	
	vertices=Vector{Float32}()
	normals =Vector{Float32}()	
	for face in faces
	
		p3,p2,p1,p0 = points[face[1]],points[face[2]],points[face[3]],points[face[4]] # reverse order
		n=0.5*(computeNormal(p0,p1,p2) + computeNormal(p0,p2,p3))
		
		append!(vertices,p0); append!(normals,n)
		append!(vertices,p1); append!(normals,n)
		append!(vertices,p2); append!(normals,n)
		append!(vertices,p0); append!(normals,n)
		append!(vertices,p2); append!(normals,n)
		append!(vertices,p3); append!(normals,n)
	end	
		
	ret=GLMesh(GL_TRIANGLES)
	ret.vertices = GLVertexBuffer(vertices)
	ret.normals  = GLVertexBuffer(normals)
	return ret
end

	# ////////////////////////////////////////////////////////////////////////
function GLAxis(p0::Point3d,p1::Point3d)

	vertices=Vector{Float32}()
	colors  =Vector{Float32}()
	
	R=Point4d(1,0,0,1); append!(vertices,p0); append!(vertices,Point3d(p1[1],p0[2],p0[3])); append!(colors,R); append!(colors,R)
	G=Point4d(0,1,0,1); append!(vertices,p0); append!(vertices,Point3d(p0[1],p1[2],p0[3])); append!(colors,G); append!(colors,G)
	B=Point4d(0,0,1,1); append!(vertices,p0); append!(vertices,Point3d(p0[1],p0[2],p1[3])); append!(colors,B); append!(colors,B)
	
	ret=GLMesh(GL_LINES)
	ret.vertices=GLVertexBuffer(vertices)
	ret.colors  =GLVertexBuffer(colors)
	return ret
end
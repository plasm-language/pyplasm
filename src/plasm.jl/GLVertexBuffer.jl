

# /////////////////////////////////////////////////////////////////////
mutable struct GLVertexBuffer

	id::Int32
	vector::Vector{Float32}
	
	# constructor
	function GLVertexBuffer()
		ret=new(-1,[])
		finalizer(releaseGpuResources, ret)
		return ret
	end
	
	# constructor
	function GLVertexBuffer(vector::Vector{Float32})
		ret=new(-1,vector)
		finalizer(releaseGpuResources, ret)
		return ret
	end	
	
end

# /////////////////////////////////////////////////////////////////////
function releaseGpuResources(buffer::GLVertexBuffer)
	global __release_gpu_resources__
	if buffer.id>=0
		id=buffer.id
		buffer.id=-1
		glDeleteLater(function()  glDeleteBuffers(1,[id]) end)
	end
end

# /////////////////////////////////////////////////////////////////////
function enableAttribute(location::Int32,buffer::GLVertexBuffer,num_components::Int64)
	if length(buffer.vector)==00 || location<0 return end
	if buffer.id<0 buffer.id=glGenBuffer() end
	glBindBuffer(GL_ARRAY_BUFFER, buffer.id)
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer.vector), buffer.vector, GL_STATIC_DRAW)
	glVertexAttribPointer(location,num_components,GL_FLOAT,false,0,C_NULL)
	glEnableVertexAttribArray(location)	
	glBindBuffer(GL_ARRAY_BUFFER, 0)	
end	

# /////////////////////////////////////////////////////////////////////
function disableAttribute(location::Int32,buffer::GLVertexBuffer)
	if length(buffer.vector)==00 || location<0 return end
	glDisableVertexAttribArray(location)
end


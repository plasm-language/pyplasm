

# /////////////////////////////////////////////////////////////////////
mutable struct GLVertexArray

	id::Int32
	
	# constructor
	function GLVertexArray()
		ret=new(-1)
		finalizer(releaseGpuResources, ret)
		return ret
	end
		
end


# /////////////////////////////////////////////////////////////////////
function releaseGpuResources(array::GLVertexArray)
	global __release_gpu_resources__
	if array.id>=0
		id=array.id
		array.id=-1
		glDeleteLater(function() glDeleteVertexArrays(1,[id]) end)	
	end
end

# /////////////////////////////////////////////////////////////////////
function enableVertexArray(array::GLVertexArray)

	# not needed or osx
	if Sys.isapple() return end

	if array.id<0
		array.id=glGenVertexArray()
	end
	glBindVertexArray(array.id)
end

# /////////////////////////////////////////////////////////////////////
function disableVertexArray(array::GLVertexArray)

	# not needed or osx
	if Sys.isapple() return end

	glBindVertexArray(0)
end
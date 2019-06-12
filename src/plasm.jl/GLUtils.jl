

# /////////////////////////////////////////////////////////////////////
function glGenBuffer()
	id = GLuint[0]
	glGenBuffers(1, id)
	glCheckError("generating a buffer, array, or texture")
	id[]
end

# /////////////////////////////////////////////////////////////////////
function glGenVertexArray()
	id = GLuint[0]
	glGenVertexArrays(1, id)
	glCheckError("generating a buffer, array, or texture")
	id[]
end




# /////////////////////////////////////////////////////////////////////
function glCheckError(actionName="")
	message = glErrorMessage()
	if length(message) > 0
		if length(actionName) > 0
		error("Error ", actionName, ": ", message)
		else
		error("Error: ", message)
		end
	end
end

# /////////////////////////////////////////////////////////////////////
function glErrorMessage()
	err = glGetError()
	if err == GL_NO_ERROR return "" end
	if err == GL_INVALID_ENUM return "GL_INVALID_ENUM" end
	if err == GL_INVALID_VALUE return "GL_INVALID_VALUE"  end
	if err == GL_INVALID_OPERATION return "GL_INVALID_OPERATION"  end
	if err == GL_INVALID_FRAMEBUFFER_OPERATION return "GL_INVALID_FRAMEBUFFER_OPERATION"  end
	if err == GL_OUT_OF_MEMORY return "GL_OUT_OF_MEMORY" end
	return "Unknown OpenGL error with error code"
end


# /////////////////////////////////////////////////////////////////////
__release_gpu_resources__=[] 

function glDeleteLater(fun::Function)
	global __release_gpu_resources__
	append!(__release_gpu_resources__,[fun])
end

# /////////////////////////////////////////////////////////////////////
function glDeleteNow()
	global __release_gpu_resources__
	for fun in __release_gpu_resources__
		fun()
	end	
end



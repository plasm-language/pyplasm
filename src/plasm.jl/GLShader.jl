

# /////////////////////////////////////////////////////////////////////
mutable struct GLShader

	vertex_source
	frag_source

	program_id::Int32
	vertex_shader_id::Int32
	frag_shader_id::Int32

	# constructor
	function GLShader(vertex, fragment)
		ret=new(vertex,fragment,-1,-1,-1)
		finalizer(releaseGpuResources, ret)
		return ret
	end

end


# /////////////////////////////////////////////////////////////////////
function releaseGpuResources(shader::GLShader)

	global __release_gpu_resources__

	if shader.vertex_shader_id>=0
		id=shader.vertex_shader_id
		shader.vertex_shader_id=-1
		glDeleteLater(function()  glDeleteShader(id) end) 
	end	
	
	if shader.frag_shader_id>=0
		id=shader.frag_shader_id
		shader.frag_shader_id=-1
		glDeleteLater(function()  glDeleteShader(id) end) 
		
	end	

	if shader.program_id>=0
		id=shader.program_id
		shader.program_id=-1	
		glDeleteLater(function()  glDeleteProgram(id) end)
	end
	
end
	

# /////////////////////////////////////////////////////////////////////
function createShader(type,source)
	shader_id = glCreateShader(type)::GLuint
	glShaderSource(shader_id, 1, convert(Ptr{UInt8}, pointer([convert(Ptr{GLchar}, pointer(source))])), C_NULL)
	glCompileShader(shader_id)
	status = GLint[0]
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, status)	
	if status == GL_FALSE
		error("shader compilation failed",getGLInfo(shader_id))
	end
	return shader_id
end


# /////////////////////////////////////////////////////////////////////
function enableProgram(shader)

	if (shader.program_id<0)
	
		shader.program_id = glCreateProgram()
		if shader.program_id == 0
			error("Error creating GLShader program: ", glErrorMessage())
		end
		
		shader.vertex_shader_id=createShader(GL_VERTEX_SHADER  ,shader.vertex_source  )
		glAttachShader(shader.program_id, shader.vertex_shader_id)
		glCheckError("attaching vertex shader")
		
		shader.frag_shader_id=createShader(GL_FRAGMENT_SHADER,shader.frag_source)
		glAttachShader(shader.program_id, shader.frag_shader_id)
		glCheckError("attaching fragment shader")

		glLinkProgram(shader.program_id)
		status = GLint[0]
		glGetProgramiv(shader.program_id, GL_LINK_STATUS, status)		
		if status == GL_FALSE 
			error("Error linking program",getGLInfo(shader.shader_id))
		end
	end

	glUseProgram(shader.program_id)	
end

# /////////////////////////////////////////////////////////////////////
function disableProgram(shader)
	glUseProgram(0)	
end
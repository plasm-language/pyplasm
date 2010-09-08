#include <xge/xge.h>
#include <xge/engine.h>
#include <GL/glew.h>

#if ENGINE_ENABLE_SHADERS

/////////////////////////////////////////////////////////////////////////////////////
EngineShader::EngineShader(std::string vertex_shader_filename,std::string fragment_shader_filename)
{
	this->vertex_shader   = createShader(GL_VERTEX_SHADER  ,vertex_shader_filename  );
	this->fragment_shader = createShader(GL_FRAGMENT_SHADER,fragment_shader_filename);

	std::vector<SmartPointer<EngineResource> > shaders;
	shaders.push_back(vertex_shader);
	shaders.push_back(fragment_shader);
	this->program = createProgram(shaders);
}


/////////////////////////////////////////////////////////////////////////////////////
SmartPointer<EngineResource> EngineShader::createShader(int target,std::string filename)
{
	SmartPointer<EngineResource> ret;

	//i don't want to loose he context (if exists)
	int64 RC=(int64)wglGetCurrentContext();
	bool bValidContext=(RC!=0);
	if (!bValidContext) 
	{
		//cannot bind!
		if (!Engine::_shared_context->Bind())
			return SmartPointer<EngineResource>();

		RC=Engine::_shared_context->RC;
	}
	else 
	{
		Engine::_shared_context->lock.Lock();
	}
	int glew_context=Engine::WCs[RC];
	#define glewGetContext() ((GLEWContext*)glew_context)
	{
		unsigned int shader_id= glCreateShader( target);
		ReleaseAssert(shader_id);

		unsigned long filesize;
		char* source_code=(char*)FileSystem::ReadFile(filename,filesize,true);
		ReleaseAssert(source_code);

		glShaderSource( shader_id, 1, (const GLchar**)&source_code, NULL);
		glCompileShader(shader_id);

		// check if shader compiled
		int compiled = 0;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);

		if (!compiled)
		{
			char temp[256] = "";
			glGetShaderInfoLog(shader_id, 256, NULL, temp);
			Utils::Error(HERE,"Compile failed:\n%s\n", temp);
		}

		MemPool::getSingleton()->free(filesize,source_code);
		ret.reset(new EngineResource(EngineResource::RESOURCE_SHADER,shader_id,0));
	}
	#undef glewGetContext
	
	if (!bValidContext) 
		Engine::_shared_context->Unbind(); 
	else 
		Engine::_shared_context->lock.Unlock();

	return ret;
}



/////////////////////////////////////////////////////////////////////////////////////
SmartPointer<EngineResource> EngineShader::createProgram(std::vector< SmartPointer<EngineResource> > shaders)
{
	SmartPointer<EngineResource> ret;

	//i don't want to loose he context (if exists)
	int64 RC=(int64)wglGetCurrentContext();
	bool bValidContext=(RC!=0);
	if (!bValidContext) 
	{
		if (!Engine::_shared_context->Bind())
			return SmartPointer<EngineResource>();

		RC=Engine::_shared_context->RC;
	}
	else 
	{
		Engine::_shared_context->lock.Lock();
	}
	int glew_context=Engine::WCs[RC];
	#define glewGetContext() ((GLEWContext*)glew_context)
	{
		unsigned int program_id= glCreateProgram();
		ReleaseAssert(program_id);

		for (int i=0;i<(int)shaders.size();i++)
		{
			ReleaseAssert(shaders[i]->type==EngineResource::RESOURCE_SHADER);
			glAttachShader(program_id, shaders[i]->id);
			glAttachShader(program_id, shaders[i]->id);
		}

		glLinkProgram (program_id);

		//eventually errors
		int charsWritten, infoLogLength;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLogLength);
		char * infoLog = new char[infoLogLength];
		glGetProgramInfoLog(program_id, infoLogLength, &charsWritten, infoLog);
		Log::printf(infoLog);
		delete [] infoLog;

		// Test linker result.
		int linkSucceed = GL_FALSE;
		glGetProgramiv(program_id, GL_LINK_STATUS, &linkSucceed);
		ReleaseAssert(linkSucceed);

		ret.reset(new EngineResource(EngineResource::RESOURCE_PROGRAM,program_id,0));
	}
	#undef glewGetContext
	
	if (!bValidContext) 
		Engine::_shared_context->Unbind(); 
	else 
		Engine::_shared_context->lock.Unlock();

	return ret;
}


/////////////////////////////////////////////////////////////////////////////////////
void EngineShader::Bind(Engine* engine)
{
	//I should have the bind of the engine!
	#define glewGetContext() ((GLEWContext*)engine->WC)
	glUseProgram(this->program->id);
	#undef glewGetContext
}

/////////////////////////////////////////////////////////////////////////////////////
void EngineShader::Unbind(Engine* engine)
{	
	#define glewGetContext() ((GLEWContext*)engine->WC)
	glUseProgram(0);
	#undef glewGetContext
}


/////////////////////////////////////////////////////////////////////////////////////

#define glewGetContext() ((GLEWContext*)engine->WC)

void EngineShader::Set1b (Engine* engine,int program,const char* pname,bool bvalue                 ) {glUniform1i (glGetUniformLocation(program, pname), bvalue);}
void EngineShader::Set1i (Engine* engine,int program,const char* pname,int ivalue                  ) {glUniform1i (glGetUniformLocation(program, pname), ivalue);}
void EngineShader::Set1f (Engine* engine,int program,const char* pname,float fvalue                ) {glUniform1f (glGetUniformLocation(program, pname), fvalue);}
void EngineShader::Set2i (Engine* engine,int program,const char* pname,int value1  ,int value2     ) {glUniform2i (glGetUniformLocation(program, pname), value1, value2);}
void EngineShader::Set2f (Engine* engine,int program,const char* pname,float value1,float value2   ) {glUniform2f (glGetUniformLocation(program, pname), value1, value2);}
void EngineShader::Set4fv(Engine* engine,int program,const char* pname,int count,float* value      ) {glUniform4fv(glGetUniformLocation(program, pname), count, value);}

#undef glewGetContext



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
class EngineShaderViewer : public Viewer
{
public:

	SmartPointer<Hpc>                 mesh;
	std::vector<SmartPointer<Batch> > batches;
	EngineShader*                     shader;
	bool                              bUseShader;

	////////////////////////////////////////////////////////////////////
	EngineShaderViewer(std::vector<SmartPointer<Batch> > batches)
	{	
		this->bUseShader=true;
		this->batches=batches;
		shader   =new EngineShader(":shaders/glsl/phong.vertex.glsl",":shaders/glsl/phong.fragment.glsl");
		this->frustum.guessBestPosition(getModelBox());
		this->Redisplay();
	}

	////////////////////////////////////////////////////////////////////
	void displayModel()
	{
		for (int i=0;i<(int)batches.size();i++)
			this->engine->Render(batches[i]);
	}

	////////////////////////////////////////////////////////////////////
	virtual void Keyboard(int key,int x,int y)
	{
		if (key==' ')
		{
			bUseShader=!bUseShader;
			Redisplay();
		}
		else
		{
			Viewer::Keyboard(key,x,y);
		}
	}


	////////////////////////////////////////////////////////////////////
	Box3f getModelBox()
	{
		Box3f ret;
		for (int i=0;i<(int)batches.size();i++)
			ret.add(batches[i]->getBox());
		return ret;
	}

	////////////////////////////////////////////////////////////////////
	virtual void Render()
	{
		this->engine->ClearScreen();
		this->engine->SetViewport(frustum.x,frustum.y,frustum.width,frustum.height);
		this->engine->SetProjectionMatrix(frustum.projection_matrix);
		this->engine->SetModelviewMatrix(frustum.getModelviewMatrix());
		this->engine->SetDefaultLight(frustum.pos);

		if (bUseShader)
		{
			shader->Bind(this->engine);
			displayModel();
			shader->Unbind(this->engine);
		}
		else
		{
			displayModel();
		}
		this->engine->FlushScreen();

	}
};

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
void EngineShader::SelfTest()
{
	EngineShaderViewer v(Plasm::getBatches(Plasm::open(":models/temple.hpc.xml")));
	v.Run();
	v.Wait();
}



#endif //ENGINE_ENABLE_SHADERS


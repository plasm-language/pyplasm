
#include <xge/xge.h>
#include <xge/array.h>
#include <xge/glcanvas.h>
#include <xge/xge_gl.h>

#include <JUCE/AppConfig.h>
#include <JUCE/modules/juce_opengl/juce_opengl.h>

 
////////////////////////////////////////////////////
Array::~Array()
{
	gpu.reset();
	if (pointer) MemPool::getSingleton()->free(sizeof(float)*num,pointer);
}

/////////////////////////////////////////////////////////////////////////////////////
Array::Gpu::~Gpu()
{GLDestroyLater::push_back(GLDestroyLater::DestroyArrayBuffer,id);}


/////////////////////////////////////////////////////////////////////////////////////
void Array::uploadIfNeeded(GLCanvas& gl)
{
  if (this->gpu) return;

  juce::OpenGLContext* context=(juce::OpenGLContext*)gl.getGLContext();

  GLuint bufferid;	
  context->extensions.glGenBuffers(1,&bufferid);XgeReleaseAssert(bufferid);
  context->extensions.glBindBuffer(GL_ARRAY_BUFFER,bufferid);
  context->extensions.glBufferData(GL_ARRAY_BUFFER,this->memsize(),this->c_ptr(),GL_STATIC_DRAW);
  context->extensions.glBindBuffer(GL_ARRAY_BUFFER,0);

  this->gpu=SmartPointer<Array::Gpu>(new Array::Gpu(bufferid));
}

  
////////////////////////////////////////////////////
void Array::parse(int num,float* dest,const char* source,char* _format)
{
	char format[256];
	sprintf(format,"%s%%num",_format);
	const char* ptr=source;
	for (int i=0,howmuch;i<num;i++)
	{
		float aux;
		sscanf(ptr,format,&aux,&howmuch);
		dest[i]=aux;
		ptr+=howmuch;
	}
}



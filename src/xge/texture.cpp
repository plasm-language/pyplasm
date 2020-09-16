
#include <xge/xge.h>
#include <xge/texture.h>
#include <xge/mempool.h>
#include <xge/glcanvas.h>
#include <xge/xge_gl.h>

#include <JUCE/AppConfig.h>
#include <JUCE/modules/juce_core/juce_core.h>
#include <JUCE/modules/juce_graphics/juce_graphics.h>
#include <JUCE/modules/juce_opengl/juce_opengl.h>

//all loaded textures
static std::map<std::string,SmartPointer<Texture> > textures_in_cache;


///////////////////////////////////////////////
static SmartPointer<Texture> readTga(std::string filename)			
{    
	unsigned char	hdr[18];		

	printf("Opening file %s\n",filename.c_str());
	FILE *file = fopen(filename.c_str(), "rb");					

	if(	file==NULL || fread(hdr,1,sizeof(hdr),file)!=sizeof(hdr))		
	{
		if (file) fclose(file);	
		printf("error opening  TGA file %s\n",filename.c_str());
		return  SmartPointer<Texture>();		
	}

	int width  = hdr[12]+hdr[13]*256 ;				
	int height = hdr[14]+hdr[15]*256 ;		
	int bpp	   = hdr[16];
    
 	if(	width	<=0 ||	height	<=0 || !(bpp==8 || bpp==24 || bpp==32))
	{
		fclose(file);	
		printf("error reading  TGA file %s, width/height null, or wrong bpp\n",filename.c_str());
		return  SmartPointer<Texture>();		
	}

  SmartPointer<Texture> ret(new Texture(width,height,bpp));
  int tot=(width)*(height)*((bpp)/8);
  if (fread(ret->buffer,1,tot,file)!=tot)
	{
		printf("error reading texture file %s, cannot read pixels\n",filename.c_str());
		return  SmartPointer<Texture>();		
	}

  //swap red and blue
  if (bpp==24 || bpp==32)
    {for (int I=0;I<tot;I+=(bpp/8)) std::swap(ret->buffer[I],ret->buffer[I+2]);}

 
	return ret;
}


///////////////////////////////////////////////
static bool writeTga(std::string filename,Texture* tex)
{
   unsigned char uselessChar=0;
   short int     uselessInt =0;  

   // Open file for output.
   FILE *file = fopen(filename.c_str(), "wb");
   if(!file) 
    {fclose(file); return false;}

   unsigned char imageType=2; 
   unsigned char bpp      =tex->bpp;
   short int     width    =tex->width;
   short int     height   =tex->height;
   fwrite(&uselessChar, sizeof(unsigned char), 1, file);
   fwrite(&uselessChar, sizeof(unsigned char), 1, file);
   fwrite(&imageType  , sizeof(unsigned char), 1, file);
   fwrite(&uselessInt , sizeof(short int    ), 1, file);
   fwrite(&uselessInt , sizeof(short int    ), 1, file);
   fwrite(&uselessChar, sizeof(unsigned char), 1, file);
   fwrite(&uselessInt , sizeof(short int    ), 1, file);
   fwrite(&uselessInt , sizeof(short int    ), 1, file);
   fwrite(&width      , sizeof(short int    ), 1, file);
   fwrite(&height     , sizeof(short int    ), 1, file);
   fwrite(&bpp        , sizeof(unsigned char), 1, file);
   fwrite(&uselessChar, sizeof(unsigned char), 1, file);

   bool ret=true;
   int tot=tex->width*tex->height*(tex->bpp/8);

  //swap red and blue
   if (bpp==24 || bpp==32)
    {for (int I=0;I<tot;I+=(bpp/8)) std::swap(tex->buffer[I],tex->buffer[I+2]); }

   if (fwrite(tex->buffer,1,tot,file)!=tot)
   {
     printf("error writing texture file %s, cannot write pixels\n",filename.c_str());
     ret=false;
   }

  //swap red and blue
   if (bpp==24 || bpp==32)
    {for (int I=0;I<tot;I+=(bpp/8)) std::swap(tex->buffer[I],tex->buffer[I+2]); }

   fclose(file);
   return ret;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
std::vector<std::string> Texture::getCache()
{
	std::vector<std::string> ret;

	for (std::map<std::string,SmartPointer<Texture> >::iterator it=textures_in_cache.begin();it!=textures_in_cache.end();it++)
		ret.push_back(it->first);

	return ret;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Texture::flushCache()
{
	textures_in_cache.clear();
}



//------------------------------------------------------------------------
//------------------------------------------------------------------------
Texture::Texture() 
{
	this->filename="";
	this->width=0;
	this->height=0;
	this->bpp=0;
	this->buffer=0;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
Texture::Texture(int width,int height,int bpp) 
{
	//supported only these formats
	if (!(bpp==24 || bpp==32 || bpp==8))
	{
		Utils::Error(HERE,"Texture::Texture(int width,int height,int bpp)  bpp=%d is not supported",(int)bpp);
	}

	this->filename="";
	this->width=width;
	this->height=height;
	this->bpp=bpp;
	this->buffer=0;

	int imgsize=memsize();
	this->buffer=(unsigned char*)MemPool::getSingleton()->malloc(imgsize);
	memset(this->buffer,0,imgsize);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
Texture::Texture(int width,int height,int bpp,const std::vector<unsigned char>& buffer) 
{
	if (buffer.size()!=width*height*(bpp/8))
	{
		Utils::Error(HERE,"Texture::Texture(int width,int height,int bpp,const std::vector<unsigned char>& buffer)  invalid argument buffer, buffer.size()=%d but it's size must be %d",(int)buffer.size(),(int)width*height*(bpp/8));
	}

	if (!(bpp==24 || bpp==32 || bpp==8))
	{
		Utils::Error(HERE,"Texture::Texture(int width,int height,int bpp,const std::vector<unsigned char>& buffer)  bpp=%d is not supported",(int)bpp);
	}


	this->filename="";
	this->width=width;
	this->height=height;
	this->bpp=bpp;

	int imgsize=memsize();
	this->buffer=(unsigned char*)MemPool::getSingleton()->malloc(imgsize);
	memcpy(this->buffer,&buffer[0],imgsize);


}



//------------------------------------------------------------------------
//------------------------------------------------------------------------
Texture::Texture(int width,int height,int bpp,unsigned char* buffer) 
{
	//supported only these formats
	if (!(bpp==24 || bpp==32 || bpp==8))
		Utils::Error(HERE,"Texture::Texture(int width,int height,int bpp,unsigned char* buffer)  bpp=%d is not supported",(int)bpp);

	this->filename="";
	this->buffer=0;
	this->bpp=bpp;
	this->width=width;
	this->height=height;

	int imgsize=memsize();
	this->buffer=(unsigned char*)MemPool::getSingleton()->malloc(imgsize);

	if (buffer)
		memcpy(this->buffer,buffer,imgsize);
	else
		memset(this->buffer,0,imgsize);
}



/////////////////////////////////////////////////////////////////////////////////////
Texture::~Texture()
{
	deallocBuffer();
}

/////////////////////////////////////////////////////////////////////////////////////
Texture::Gpu::~Gpu()
{GLDestroyLater::push_back(GLDestroyLater::DestroyTexture,id);}


/////////////////////////////////////////////////////////////////////////////////////
void Texture::uploadIfNeeded(GLCanvas& gl)
{
  if (this->gpu)
    return;

  juce::OpenGLContext* context=(juce::OpenGLContext*)gl.getGLContext();

  unsigned int texid;
  glGenTextures(1,&texid);XgeReleaseAssert(texid); 
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
  glBindTexture (GL_TEXTURE_2D, texid);
  float maxsize;
  glGetFloatv(GL_MAX_TEXTURE_SIZE,&maxsize); 
  XgeDebugAssert (this->width<=maxsize && this->height<=maxsize);

  unsigned int format=(this->bpp==24)?GL_RGB:(this->bpp==32?GL_RGBA:GL_LUMINANCE);
  unsigned int type=GL_UNSIGNED_BYTE;
  gluBuild2DMipmaps(GL_TEXTURE_2D,this->bpp/8,this->width, this->height,format, type, this->buffer);

  this->gpu=SmartPointer<Texture::Gpu>(new Texture::Gpu(texid));	
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Texture::deallocBuffer()
{
	if (buffer) 
		MemPool::getSingleton()->free(memsize(),buffer);
	
	buffer=0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
int Texture::memsize() const
{
	return (this->width)*(this->height)*(bpp/8);
}



// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Texture::change(int width,int height,int bpp)
{
	//supported only these formats
	if (!(bpp==24 || bpp==32 || bpp==8))
		Utils::Error(HERE,"Texture::Texture(int width,int height,int bpp,unsigned char* buffer)  bpp=%d is not supported",(int)bpp);

	this->gpu.reset();
	int oldsize=this->memsize();
	this->bpp   = bpp;
	this->width = width;
	this->height= height;
	int imgsize=memsize();
	this->buffer=(unsigned char*)MemPool::getSingleton()->realloc(oldsize,this->buffer,imgsize);
	memset(this->buffer,0,imgsize);
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Texture::change(int width,int height,int bpp,unsigned char* buffer)
{
	//supported only these formats
	if (!(bpp==24 || bpp==32 || bpp==8))
		Utils::Error(HERE,"Texture::Texture(int width,int height,int bpp,unsigned char* buffer)  bpp=%d is not supported",(int)bpp);

	this->gpu.reset();
	int oldsize=this->memsize();
	this->bpp   = bpp;
	this->width = width;
	this->height= height;
	int imgsize=memsize();
	this->buffer=(unsigned char*)MemPool::getSingleton()->realloc(oldsize,this->buffer,imgsize);

	if (buffer)
	{
		memcpy(this->buffer,buffer,imgsize);
	}
	else
	{
		memset(this->buffer,0,imgsize);
	}
}






// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Texture::change(int width,int height,int bpp,const std::vector<unsigned char>& buffer)
{
	if (buffer.size()!=width*height*(bpp/8))
		Utils::Error(HERE,"void Texture::change(int width,int height,int bpp,const std::vector<unsigned char>& buffer)  invalid argument buffer, buffer.size()=%d but it's size must be %d",(int)buffer.size(),(int)width*height*(bpp/8));

	//supported only these formats
	if (!(bpp==24 || bpp==32 || bpp==8))
		Utils::Error(HERE,"Texture::Texture(int width,int height,int bpp,unsigned char* buffer)  bpp=%d is not supported",(int)bpp);

	this->gpu.reset();
	int oldsize=this->memsize();
	this->bpp   = bpp;
	this->width = width;
	this->height= height;
	int imgsize=memsize();
	this->buffer=(unsigned char*)MemPool::getSingleton()->realloc(oldsize,this->buffer,imgsize);
	memcpy(this->buffer,&buffer[0],imgsize);
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
SmartPointer<Texture> Texture::open(std::string filename,bool bUseCacheIfPossible,bool bCacheInMemory)
{
	if (!filename.length()) 
	{
		Log::printf("Texture::open cannot open texture because filename is empty\n");
		return SmartPointer<Texture>();
	}

	//use cache?
	if (bUseCacheIfPossible)
	{
		if (textures_in_cache.find(Utils::ToLower(filename))!=textures_in_cache.end())
		{
			//Log::printf("Opened texture file %s from cache (already in memory)\n",Filename.c_str());
			return textures_in_cache[Utils::ToLower(filename)];
		}
	}


	//opening from disk
  juce::File   jfile(juce::File::getCurrentWorkingDirectory().getChildFile(filename.c_str()));
  juce::String jext=jfile.getFileExtension().toLowerCase();

  SmartPointer<Texture> ret;
  if (jext==".tga")
  {
    ret=readTga(filename);

    if (!ret)
      return ret; //failed
  }
  else
  {
    juce::Image jimg=juce::ImageFileFormat::loadFrom(jfile);

    if (!jimg.isValid())
	  {
		  Log::printf("Texture::open cannot open texture file %s \n",filename.c_str());
		  return SmartPointer<Texture>();
	  }

	  int width  = jimg.getWidth();
	  int height = jimg.getHeight();
	  int bpp    = jimg.getFormat()==juce::Image::ARGB? 32 : (jimg.getFormat()==juce::Image::RGB? 24 : (jimg.getFormat()==juce::Image::SingleChannel? 8:0));

	  if((width == 0) || (height == 0) || !(bpp==24 || bpp==32 || bpp==8))
	  {
		  Log::printf("Texture::open failed to load the texture file %s (reason  unsupported type bpp=%d width=%d height=%d\n",filename.c_str(),bpp,width,height);
		  return SmartPointer<Texture>();
	  }

	  ret=SmartPointer<Texture>(new Texture(width,height,bpp,0));
    unsigned char* dst=ret->buffer;

	  //Alpha
	  if (bpp==8)
	  {
		  for (int Y=0;Y<height;Y++) {
		  for (int X=0;X<width ;X++) {
        juce::Colour colour=jimg.getPixelAt(X,height-Y-1); //mirror y
			  *dst++=colour.getAlpha();
		  }}
	  }
	  //RGB
	  else if (bpp==24)
	  {	
		  for (int Y=0;Y<height;Y++) {
		  for (int X=0;X<width ;X++) {
        juce::Colour colour=jimg.getPixelAt(X,height-Y-1); //mirror y
			  *dst++=colour.getRed  ();
			  *dst++=colour.getGreen();
			  *dst++=colour.getBlue ();
		  }}
	  }
	  //RGBA
	  else if (bpp==32)
	  {
		  for (int Y=0;Y<height;Y++) {
		  for (int X=0;X<width ;X++) {
        juce::Colour colour=jimg.getPixelAt(X,height-Y-1); //mirror y
			  *dst++=colour.getRed  ();
			  *dst++=colour.getGreen();
			  *dst++=colour.getBlue ();
        *dst++=colour.getAlpha();
		  }}
	  }
  }

	Log::printf("image file %s loaded from disk width(%d) height(%d) bpp(%d)\n",filename.c_str(),ret->width,ret->height,ret->bpp);

	if (bCacheInMemory)
		textures_in_cache[Utils::ToLower(filename)]=ret;

	ret->filename=filename;
	return ret;

}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
bool Texture::save()
{
	return save(this->filename);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
bool Texture::save(std::string filename)
{
	if (!filename.length())
	{
		Log::printf("Texture::save failed to save the texture file %s (reason:  filename empty)\n");
		return false;
	}
  
  juce::File   jfile(juce::File::getCurrentWorkingDirectory().getChildFile(filename.c_str()));
  juce::String jext=jfile.getFileExtension().toLowerCase();

  if (jext==".tga")
  {
    if (!writeTga(filename,this))
      return false;
  }
  else
  {
    juce::ScopedPointer<juce::ImageFileFormat> jiff;

    if      (jext==".jpg" || jext==".jpeg") jiff=new juce::JPEGImageFormat();
    else if (jext==".png"                 ) jiff=new juce::PNGImageFormat();
    else if (jext==".git"                 ) jiff=new juce::GIFImageFormat();


    juce::Image jimg(bpp==8? (juce::Image::SingleChannel) : (bpp==24? juce::Image::RGB : juce::Image::ARGB),width,height,false);
    if (!jimg.isValid())
	  {
		  Log::printf("Texture::save failed to save the texture file %s \n",filename.c_str());
		  return false;
	  }

     if (!jiff)
     {
       Log::printf("Texture::save failed to save the texture file %s (reason: unsupported extension)\n",filename.c_str());
       return false;
     }

	  unsigned char* src=this->buffer;

    //Alpha
	  if (bpp==8)
	  {
		  for (unsigned int Y=0;Y<height;Y++) {
		  for (unsigned int X=0;X<width ;X++) {
        juce::Colour colour(src[0],src[0],src[0],src[0]);
        jimg.setPixelAt(X,height-Y-1,colour); //mirror y
			  src+=1;
		  }}
	  }
	  //RGB
	  else if (bpp==24)
	  {	
		  for (unsigned int Y=0;Y<height;Y++) {
		  for (unsigned int X=0;X<width ;X++) {
        juce::Colour colour(src[0],src[1],src[2]);
        jimg.setPixelAt(X,height-Y-1,colour); //mirror y
        src+=3;
		  }}
	  }
	  //RGBA
	  else if (bpp==32)
	  {
		  for (unsigned int Y=0;Y<height;Y++) {
		  for (unsigned int X=0;X<width ;X++) {
        juce::Colour colour(src[0],src[1],src[2],src[3]);
        jimg.setPixelAt(X,height-Y-1,colour); //mirror y
        src+=4;
		  }}
	  }

    if (jfile.existsAsFile()) 
      jfile.deleteFile();

    juce::FileOutputStream jstream(jfile);
    if (jstream.failedToOpen() || !jiff->writeImageToStream(jimg,jstream))
    {
		  Log::printf("Texture::save failed to save the texture file %s (reason: failedToOpen or writeImageToStream failed)\n",filename.c_str());
		  return false;
    }
  }

	this->filename=filename;	
	return true;
}


/////////////////////////////////////////////////
void Texture::flipVertical()
{
	for (int y1=0,y2=this->height-1;y1<y2;y1++,y2--)
	{
		for (int x=0;x<(int)this->width ;x++)
		{		
			for (int B=0;B<(this->bpp/8);B++)
			{
				unsigned char& Src=buffer[(y1*this->width+x)*(this->bpp/8)+B];
				unsigned char& Dst=buffer[(y2*this->width+x)*(this->bpp/8)+B];
				unsigned char  Tmp=Src;
				Src=Dst;
				Dst=Tmp;
			}
		}
	}
}


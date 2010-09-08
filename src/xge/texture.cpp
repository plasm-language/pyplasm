
#include <xge/xge.h>
#include <xge/Texture.h>
#include <xge/mempool.h>
#include <xge/Viewer.h>


#include <freeimage.h>

//all loaded textures
static std::map<std::string,SmartPointer<Texture> > textures_in_cache;




//------------------------------------------------------------------------
//------------------------------------------------------------------------
static bool InitializeFreeImage()
{
	Log::printf("Initializing FreeImage library\n");
	FreeImage_Initialise();
	return true;
}
static bool Free_Image_Init=InitializeFreeImage();



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



// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Texture::~Texture()
{
	deallocBuffer();
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
	//kind of normalization of path
	filename=FileSystem::ShortPath(FileSystem::FullPath(filename));

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

	//now i need the full path
	filename=FileSystem::FullPath(filename);

	//opening from disk
	//Log::printf("Opening texture file %s from disk (not using cached textures)\n",filename.c_str());

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename.c_str(), 0);

	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(filename.c_str());

	if(fif == FIF_UNKNOWN)
	{
		Log::printf("Texture::open cannot open texture file %s (reason: fif == FIF_UNKNOWN)\n",filename.c_str());
		return SmartPointer<Texture>();
	}

	FIBITMAP* bitmap=FreeImage_Load(fif, filename.c_str());

	if(!bitmap)
	{
		Log::printf("Texture::open failed to open texture file %s (reason: !bitmap)\n",filename.c_str());
		return SmartPointer<Texture>();
	}

	BITMAPINFO* info=FreeImage_GetInfo(bitmap);

	BYTE* bits = FreeImage_GetBits(bitmap);
	int width  = FreeImage_GetWidth(bitmap);
	int height = FreeImage_GetHeight(bitmap);
	int bpp    = FreeImage_GetBPP(bitmap);
	int pitch=FreeImage_GetPitch(bitmap);
	FREE_IMAGE_TYPE format = FreeImage_GetImageType(bitmap);

	if((bits == 0) || (width == 0) || (height == 0) || !(bpp==24 || bpp==32 || bpp==8) || format!=FIT_BITMAP)
	{
		FreeImage_Unload(bitmap);
		Log::printf("Texture::open failed to load the texture file %s (reason  unsupported type bpp=%d width=%d height=%d format=%d\n",filename.c_str(),bpp,width,height,(int)format);
		return SmartPointer<Texture>();
	}

	//only lighting or gray level
	SmartPointer<Texture> ret;

	//AA
	if (bpp==8)
	{
		ret.reset(new Texture(width,height,8,0));
		unsigned char* dst=ret->buffer;

		for (int Y=0;Y<height;Y++)
		{
			unsigned char* src=bits;

			for (int X=0;X<width;X++)
			{
				dst[0]=src[0];
				dst++;
				src++;
			}
			bits+=pitch;
		}
	}
	//BBGGRR
	else if (bpp==24)
	{	
		ret.reset(new Texture(width,height,24,0));
		unsigned char* dst=ret->buffer;

		for (int Y=0;Y<height;Y++)
		{
			unsigned char* src=bits;

			for (int X=0;X<width;X++)
			{
			#ifdef Darwin
				dst[0]=src[FI_RGBA_BLUE  ];
				dst[1]=src[FI_RGBA_GREEN];
				dst[2]=src[FI_RGBA_RED];
			#else
				dst[0]=src[FI_RGBA_RED  ];
				dst[1]=src[FI_RGBA_GREEN];
				dst[2]=src[FI_RGBA_BLUE ];
			#endif
				dst+=3;
				src+=3;
			}
			bits+=pitch;
		}
	}
	//BBGGRRaa
	else if (bpp==32)
	{
		ret.reset(new Texture(width,height,32,0));
		unsigned char* dst=ret->buffer;

		for (int Y=0;Y<height;Y++)
		{
			unsigned char* src=bits;

			for (int X=0;X<width;X++)
			{
			#ifdef Darwin
				dst[0]=src[FI_RGBA_RED  ];
				dst[1]=src[FI_RGBA_GREEN];
				dst[2]=src[FI_RGBA_BLUE ];
			#else
				dst[0]=src[FI_RGBA_BLUE  ];
				dst[1]=src[FI_RGBA_GREEN ];
				dst[2]=src[FI_RGBA_RED   ];
			#endif
				dst[3]=src[FI_RGBA_ALPHA];
				dst+=4;
				src+=4;
			}
			bits+=pitch;
		}
	}

	FreeImage_Unload(bitmap);
	Log::printf("image file %s loaded from disk width(%d) height(%d) bpp(%d)\n",filename.c_str(),ret->width,ret->height,ret->bpp);

	//set back to short path
	filename=FileSystem::ShortPath(filename);

	if (bCacheInMemory)
	{
		textures_in_cache[Utils::ToLower(filename)]=ret;
	}

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
	//empty name
	if (!filename.length())
	{
		Log::printf("Texture::save failed to save the texture file %s (reason:  filename empty)\n");
		return false;
	}

	filename=FileSystem::FullPath(filename);
	
	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename.c_str());

	if(fif == FIF_UNKNOWN ) 
	{
		Log::printf("Texture::save failed to save the texture file %s (reason: fif == FIF_UNKNOWN)\n",filename.c_str());
		return false;
	}

	FIBITMAP* bitmap=FreeImage_Allocate(this->width, this->height, this->bpp);

	unsigned char* src=this->buffer;
	unsigned char* dst=FreeImage_GetBits(bitmap);
	int tot=this->width*this->height;

	ReleaseAssert(FreeImage_GetPitch(bitmap)==this->width*(this->bpp/8));

	//AA
	if (this->bpp==8)
	{
		// Build a greyscale palette
		RGBQUAD *pal = FreeImage_GetPalette(bitmap);
		for (int i = 0; i < 256; i++) 
		{
			pal[i].rgbRed = i;
			pal[i].rgbGreen = i;
			pal[i].rgbBlue = i;
		}

		memcpy(dst,src,tot);
	}
	//BBGGRRff
	else if (this->bpp==24)
	{
		
		for (int i=0;i<tot;i++,dst+=3,src+=3) 
		{
		//bug in the inversion
		#ifdef Darwin
			dst[FI_RGBA_BLUE  ]=src[0]; 
			dst[FI_RGBA_GREEN ]=src[1]; 
			dst[FI_RGBA_RED   ]=src[2]; 
		#else
			dst[FI_RGBA_RED  ]=src[0]; 
			dst[FI_RGBA_GREEN]=src[1]; 
			dst[FI_RGBA_BLUE ]=src[2];  
		#endif
		}
	}
	//BBGGRRaa
	else if (this->bpp==32)
	{
		for (int i=0;i<tot;i++,src+=4,dst+=4) 
		{
		#ifdef Darwin
			dst[FI_RGBA_BLUE  ]=src[0];
			dst[FI_RGBA_GREEN ]=src[1];
			dst[FI_RGBA_RED    ]=src[2];
		#else
			dst[FI_RGBA_RED  ]=src[0];
			dst[FI_RGBA_GREEN]=src[1];
			dst[FI_RGBA_BLUE ]=src[2];
		#endif
			dst[FI_RGBA_ALPHA]=src[3];
		}
	}
	else
	{
		ReleaseAssert(false);
	}

	bool bSaved=FreeImage_Save(fif, bitmap, filename.c_str())>0;
	
	if (!bSaved)
	{
		FreeImage_Unload(bitmap);
		Log::printf("Texture::save failed to save the texture file %s (reason: !bSaved)\n",filename.c_str());
		return false;
	}

	FreeImage_Unload(bitmap);

	filename=FileSystem::ShortPath(filename);
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



/////////////////////////////////////////////////////////////////
class TextureViewer:public Viewer
{
	SmartPointer<Texture> texture0;
	SmartPointer<Texture> texture1;

public:

	//constructor
	TextureViewer(SmartPointer<Texture> texture0,SmartPointer<Texture> texture1)
	{
		this->texture0=texture0;
		this->texture1=texture1;
	}

	//display
	virtual void Render()
	{
		int W=this->frustum.width;
		int H=this->frustum.height;
		SmartPointer<Batch> batch(new Batch);
		batch->primitive=Batch::QUADS;
		batch->setColor(Color4f(1,1,1));
		float _vertices[]       = {0,0,0,  W,0,0,  W,H,0, 0,H,0};batch->vertices.reset(new Vector(12,_vertices));
		batch->texture0=texture0;
		batch->texture1=texture1;
		float _texture0coords[] ={0,0, 1,0, 1,1, 0,1};batch->texture0coords.reset(new Vector(8,_texture0coords));
		float _texture1coords[] ={0,0, 1,0, 1,1, 0,1};batch->texture1coords.reset(new Vector(8,_texture1coords));

		engine->ClearScreen();
		engine->SetViewport(0,0,W,H);
		engine->SetProjectionMatrix(Mat4f::ortho(0,W,0,H,-1,+1));
		engine->SetModelviewMatrix(Mat4f());
		engine->Render(batch);
		engine->FlushScreen();
	}
};

int Texture::SelfTest()
{
	Log::printf("Testing Texture...\n");
	
	SmartPointer<Texture> gioconda=Texture::open(":images/gioconda.tga",false,false);

	//try to see if the shared context is working
	if (true)
	{
		SmartPointer<Texture> back    =Texture::open(":images/gioconda.tga",false,false);
		SmartPointer<Texture> texture1=Texture::open(":images/gioconda.texture1.tga",false,false);
		TextureViewer v1(back,texture1);v1.Run();v1.Wait();
		TextureViewer v2(back,texture1);v2.Run();v2.Wait();
	}

	//TGA open/save
	if (true)
	{
		bool ret=gioconda->save(":temp/gioconda.copy.tga");
		ReleaseAssert(gioconda->filename==":temp/gioconda.copy.tga");

		ReleaseAssert(ret);
		SmartPointer<Texture> back=Texture::open(":temp/gioconda.copy.tga",false,false);
		SmartPointer<Texture> texture1=Texture::open(":images/gioconda.texture1.tga",false,false);
		ReleaseAssert(back && texture1);
		TextureViewer v(back,texture1);v.Run();v.Wait();
	}

	//test PNG load/save
	if (true)
	{
		bool ret=gioconda->save(":temp/gioconda.copy.png");
		ReleaseAssert(gioconda->filename==":temp/gioconda.copy.png");

		ReleaseAssert(ret);
		SmartPointer<Texture> back=Texture::open(":temp/gioconda.copy.png",false,false);
		SmartPointer<Texture> texture1=Texture::open(":images/gioconda.texture1.png",false,false);
		ReleaseAssert(back && texture1);
		TextureViewer v(back,texture1);v.Run();v.Wait();
	}

	//test jpeg load/save
	if (true)
	{
		bool ret=gioconda->save(":temp/gioconda.copy.jpg");
		ReleaseAssert(gioconda->filename==":temp/gioconda.copy.jpg");

		ReleaseAssert(ret);
		SmartPointer<Texture> back=Texture::open(":temp/gioconda.copy.jpg",false,false);
		SmartPointer<Texture> texture1=Texture::open(":images/gioconda.texture1.jpg",false,false);
		ReleaseAssert(back && texture1);
		TextureViewer v(back,texture1);v.Run();v.Wait();
	}
	
	//test PPM load/save  (broken in macosx)
	#ifndef Darwin
	if (true)
	{
		bool ret=gioconda->save(":temp/gioconda.copy.ppm");
		ReleaseAssert(gioconda->filename==":temp/gioconda.copy.ppm");

		ReleaseAssert(ret);
		SmartPointer<Texture> back=Texture::open(":temp/gioconda.copy.ppm",false,false);
		SmartPointer<Texture> texture1=Texture::open(":images/gioconda.texture1.ppm",false,false);
		ReleaseAssert(back && texture1);
		TextureViewer v(back,texture1);v.Run();v.Wait();
	}
	#endif

	//test TIFF load/save (broken in macosx)
	#ifndef Darwin
	if (true)
	{
		bool ret=gioconda->save(":temp/gioconda.copy.tif");
		ReleaseAssert(gioconda->filename==":temp/gioconda.copy.tif");
		ReleaseAssert(ret);
		SmartPointer<Texture> back=Texture::open(":temp/gioconda.copy.tif",false,false);
		SmartPointer<Texture> texture1=Texture::open(":images/gioconda.texture1.tif",false,false);
		ReleaseAssert(back && texture1);
		TextureViewer v(back,texture1);v.Run();v.Wait();
	}
	#endif


	//test BMP load/save
	if (true)
	{
		bool ret=gioconda->save(":temp/gioconda.copy.bmp");
		ReleaseAssert(gioconda->filename==":temp/gioconda.copy.bmp");

		ReleaseAssert(ret);
		SmartPointer<Texture> back=Texture::open(":temp/gioconda.copy.bmp",false,false);
		SmartPointer<Texture> texture1=Texture::open(":images/gioconda.texture1.bmp",false,false);
		ReleaseAssert(back && texture1);
		TextureViewer v(back,texture1);v.Run();v.Wait();
	}

	Texture::flushCache();

	return 0;
}

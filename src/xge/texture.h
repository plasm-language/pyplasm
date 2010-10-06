#ifndef _TEXTURE_H__
#define _TEXTURE_H__

#include <xge/xge.h>


//predeclaration
class EngineResource;



//================================================
//! class which stores texture maps
//================================================
class XGE_API Texture 
{
public:

	//! filename
	std::string filename;

	//!  id texture map, if 0 no texture map has been created
	SmartPointer<EngineResource> gpu;

	//! bit per pixel (8=LUMINANCE,24=RGB,32=RGBA)
	int bpp;

	//! dimension of the texture
	unsigned int    width ;
	unsigned int    height;

	//! internal buffer where the pixels are
	unsigned char* buffer;

	//! default contructor
	/*
		@py
		t=Texture()
		assert t.width==0 and t.height==0 and t.bpp==0 and t.texid==0 and t.filename==""
		@endpy
	*/
	Texture();

	//! constructor
	/*! 
		@py 
		t=Texture(100,100,24)
		assert t.width==100 and t.height==100 and t.bpp==24 and t.texid==0 and t.memsize()==100*100*3
		@endpy
	*/
	explicit Texture(int width,int height,int bpp);

	//! constructor from buffer of C data
	explicit Texture(int width,int height,int bpp,unsigned char* buffer);

	//! constructor from STD vector<unsigned char> of data
	/*! 
		@py 
		t=Texture(2,2,24,[1,2,3, 4,5,6, 7,8,9, 10,11,12])
		assert t.width==2 and t.height==2 and t.bpp==24 and [t.get(i) for i in range(0,4*3)]==range(1,13)
		@endpy
	*/
	explicit Texture(int width,int height,int bpp,const std::vector<unsigned char>& buffer);

	//!destructor
	~Texture();

	//! open function, all the supported format from Qt plus TGA
	/*! 
		\param filename name of the file to open
		\param bUseCacheIfPossible if the texture has already been loaded (see param bCacheInMemory) then it is possible
		                           to reuse the same data 
	   \param bCacheInMemory after the loading from disk, if this param is True, the texture map will be put in a special
	                         hashtable, kind of cache that can be used later by setting bUseCacheIfPossible=true

	*/
	static SmartPointer<Texture> open(std::string filename,bool bUseCacheIfPossible=true,bool bCacheInMemory=true);

	//! flush all textures in cache (i.e. the next open will be forced to read the texture from disk)
	//! note: if some textures have not been deallocated (and are kept alive somewhere) there can be 
	//! a problem about texture memory for Gpu
	static void flushCache();


	//! get list of cached textures
	static std::vector<std::string> getCache();

	//!save function
	bool save(std::string filename);

	//! save function
	bool save();
	
	//! if you want to dealloc the C buffer in RAM
	/*!
		use this functin only if the texture map in on the GPU RAM (so after a call to generate) and
		you don't need to modify it at all
	*/
	void deallocBuffer();

	//! calculate memory occupancy
	/*! 
		@py
		t=Texture(2,2,8)
		assert t.memsize()==2*2*1
		@endpy
	*/
	int memsize() const;

	//! resize the texture
	/*! 
		@py
		t=Texture(2,2,8)
		t.change(4,4,24)
		assert t.memsize()==4*4*3
		@endpy
	*/
	void change(int width,int height,int bpp);

	//! modify the texture
	void change(int width,int height,int bpp,unsigned char* buffer);

	//!modify the texture
	/*! 
		@py 
		t=Texture(4,4,24)
		t.change(2,2,8,[1,2,3,4])
		assert t.memsize()==2*2*1 and t.get(0)==1 and t.get(1)==2 and t.get(2)==3 and t.get(3)==4
		@endpy
	*/
	void change(int width,int height,int bpp,const std::vector<unsigned char>& buffer);


	//! flip vertically
	//void flipVertical();


	//! access the single pixel
	inline unsigned char* getPixel(int x,int y) 
		{return buffer+((y*width+x)*(bpp/8));}


	inline bool goodPixel(int x,int y)
	{
		return x>=0 && x<(int)this->width && y>=0 && y<(int)this->height;
	}

	//! low-level access
	/*! 
		@py 
		t=Texture(2,2,8,[0,1,2,3])
		assert [t.get(i) for i in range(0,4)]==range(0,4)
		@endpy
	*/
	inline unsigned char get(int i) const
	{
		if (i<0 || i>=memsize())
			Utils::Error(HERE,"unsigned char get(int i) const argument i=%d ouside valid range [0..%d]",memsize()-1);

		return buffer[i];
	}

	//! low-level access
	/*! 
		@py 
		t=Texture(2,2,8)
		t.set(0,0);t.set(1,1)
		t.set(2,2);t.set(3,3)
		assert [t.get(i) for i in range(0,4)]==range(0,4)
		@endpy
	*/
	inline void set(int i,unsigned char value) 
	{
		if (i<0 || i>=memsize())
			Utils::Error(HERE,"void set(int i,unsigned char value)  argument i=%d ouside valid range [0..%d]",memsize()-1);

		buffer[i]=value;
	}

	//! flip
	void flipVertical();


	//! python repr()
	/*! 
		@py 
		repr(Texture())
		@endpy
	*/
	inline std::string repr() const
	{
		return Utils::Format("Texture width(%d) height(%d) bpp(%d) filename(%s) buffer(%s)",(int)width,(int)height,(int)bpp,filename.c_str(),buffer?"...":"NULL");
	}

	//! python str()
	/*! 
		@py 
		str(Texture())
		@endpy
	*/
	inline std::string str() const
	{
		return repr();
	}

	//! internal self test
	static int SelfTest();
	
}; //end class






#endif  //_TEXTURE_H__




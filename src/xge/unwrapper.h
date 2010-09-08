#ifndef _UNWRAPPER_H__
#define _UNWRAPPER_H__

#include <xge/xge.h>
#include <xge/texture.h>
#include <xge/triangle.h>
#include <xge/box.h>
#include <xge/vec.h>



class XGE_API Unwrapper
{
protected:
	
	//estimated time
	int ntriangles_done;

	//textures stuff
	typedef struct
	{
		SmartPointer<Texture> rgb;
		SmartPointer<Texture> alpha;
		std::set< Box2i >       queue;
	}
	t_working;

	t_working prv,cur;

	//all the texture1 textures to create
	std::vector< SmartPointer<Texture> > new_light_textures;

public:

	//where to save textures (default ":texture1.%02d.tif")
	//MUST be with .tif extension!
	std::string texture_template;

	//user configuration (default 1024)
	unsigned int texturedim;

	//user configuration (default 1.0)
	float factor;

	//constructor
	Unwrapper();

	//run the unwrapping algorithm, batch is not touched but produce in return one or more batch unwrapped
	std::vector< SmartPointer<Batch> > Unwrap(Mat4f T,SmartPointer<Batch> batch);

	//get createt texture1 textures
	std::vector< SmartPointer<Texture> > getNewLightTextures()
	{
		return new_light_textures;
	}

}; //end class


#endif //Unwrapper



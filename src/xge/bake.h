#ifndef _RIB_BAKE_H__
#define _RIB_BAKE_H__

#include <xge/xge.h>
#include <xge/Batch.h>

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class XGE_API Bake
{
protected:

	//list of texture1 textures to process
	std::map< Texture* , std::vector<SmartPointer<Batch> > > texture1_map;


	Box3f scene_box;

public:

	//user configuration
	bool  DebugMode;
	bool  PointOcclusion;

	//constructor
	Bake();

	//eventually here the unwrapping
	std::vector< SmartPointer<Batch> > Unwrap(std::vector< SmartPointer<Batch> > batches,float factor,std::string texture_template,int texturedim=1024);

	//add a batch
	void Add(Mat4f T,SmartPointer<Batch> batch);

	//add some batches
	void Add(std::vector<SmartPointer<Batch> > batches)
	{
		for (int i=0;i<(int)batches.size();i++) 
			this->Add(Mat4f(),batches[i]);
	}

	//to the export (return false on error)
	bool Export();

	//Run the rib
	bool Run();

	//final post processing
	bool PostProcess();

	//self test
	static int SelfTest();

};


#endif //_RIB_BAKE_H__


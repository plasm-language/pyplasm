#include <xge/xge.h>
#include <xge/batch.h>

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
std::vector<SmartPointer<Batch> > Batch::openPtx(std::string filename,bool bUseReflectance)
{

	filename=FileSystem::FullPath(filename);

	FILE* file=fopen(filename.c_str(),"rt");

	if (!file)
		return std::vector<SmartPointer<Batch> >();

	float mat0[12],mat1[16];
	int h0,h1;
	fscanf(file,"%d %d",&h0,&h1);
	for (int i=0;i<12;i++) fscanf(file,"%f",&mat0[i]);
	for (int i=0;i<16;i++) fscanf(file,"%f",&mat1[i]);

	//number of points
	int npoints=h0*h1;

	SmartPointer<Batch> batch(new Batch);

	batch->primitive     = Batch::POINTS;
	batch->matrix   =Mat4f(mat1).transpose();
	batch->vertices .reset(new Vector(npoints*3));
	batch->colors  .reset(new Vector(npoints*3));

	float* V=batch->vertices->mem();
	float* C=batch->colors  ->mem();

	for (int i=0;i<npoints;i++)		
	{
		float reflactance;

		Vec3f v;
		Vec3f c;

		//note: the color should be int by I prefer using float
		int nfound=fscanf(file,"%f %f %f %f %f %f %f",&v.x,&v.y,&v.z, &reflactance, &c.x,&c.y,&c.z );
		ReleaseAssert(nfound==7); 

		if (bUseReflectance)
			c=Vec3f(reflactance,reflactance,reflactance);
		else
			c/=255.0f;
	
		ReleaseAssert(c.x>=0 && c.x<=1); 
		ReleaseAssert(c.y>=0 && c.y<=1);
		ReleaseAssert(c.z>=0 && c.z<=1);

		*V++=v.x;*V++=v.y;*V++=v.z;
		*C++=c.x;*C++=c.y;*C++=c.z;
	}

	std::vector<SmartPointer<Batch> > ret;
	ret.push_back(batch);
	return ret;
}

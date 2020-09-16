#include <xge/xge.h>

#include <iostream>

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
static inline void skipLine(FILE* file)
{
	int ch;
	bool crlf=false;
	while (!feof(file))
	{
		ch=fgetc(file);
		if (ch==10 || ch==13)
		{
			crlf=true;
		}
		else
		{
			//if I come from a CRLF then I skipped the line
			if (crlf) 
			{
				ungetc(ch,file);
				break;
			}
		}
	}
}


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
void Batch::saveObj(std::string filename,std::vector<SmartPointer<Batch> > batches)
{
	FILE* file=fopen(filename.c_str(),"wt");
	XgeReleaseAssert(file);

  Mat4f prepend_T;

  if (bool bToUnitBox=false)
  {
	  Box3f box;
	  for (int I=0;I<(int)batches.size();I++)
	  {
		  SmartPointer<Batch> batch=batches[I];
		  box.add(batch->getBox());
	  }

	  prepend_T= Mat4f::scale(1.0f/box.size().x,1.0f/box.size().y,1.0f/box.size().z) * Mat4f::translate(-box.p1.x,-box.p1.y,-box.p1.z);
  }

	int Cont=1;

	for (int I=0;I<(int)batches.size();I++)
	{
		SmartPointer<Batch> batch=batches[I];

		//TODO: all other cases
		XgeReleaseAssert(batch->primitive==Batch::TRIANGLES);

		Mat4f matrix=batch->matrix;

		matrix=prepend_T * matrix;

		Mat4f inv=matrix.invert();

		/*XgeReleaseAssert(batch->texture1);
		XgeReleaseAssert(batch->texture1->width==batch->texture1->height);
		int texturedim=batch->texture1->width;*/

		int nv=batch->vertices->size()/3;
		int nt=nv/3;

		XgeReleaseAssert(batch->vertices);
		XgeReleaseAssert(batch->normals);
		//XgeReleaseAssert(batch->texture1coords);

		float* vertex     = (float*)batch->vertices->c_ptr();
		float* normal     = (float*)batch->normals ->c_ptr();
		//float* lightcoord = (float*)batch->texture1coords->c_ptr();

		for (int i=0;i<nt;i++,vertex+=9,normal+=9)
		{
			Vec3f v0(vertex[0],vertex[1],vertex[2]) ; v0=matrix * v0; 
			Vec3f v1(vertex[3],vertex[4],vertex[5]) ; v1=matrix * v1; 
			Vec3f v2(vertex[6],vertex[7],vertex[8]) ; v2=matrix * v2; 

			Vec4f _n0(normal[0],normal[1],normal[2],0.0);  _n0=_n0 * inv;  Vec3f n0=Vec3f(_n0.x,_n0.y,_n0.z).normalize();
			Vec4f _n1(normal[3],normal[4],normal[5],0.0);  _n1=_n1 * inv;  Vec3f n1=Vec3f(_n1.x,_n1.y,_n1.z).normalize();
			Vec4f _n2(normal[6],normal[7],normal[8],0.0);  _n2=_n2 * inv;  Vec3f n2=Vec3f(_n2.x,_n2.y,_n2.z).normalize();

			/*float s0=lightcoord[i*6+0],t0=lightcoord[i*6+1];
			float s1=lightcoord[i*6+2],t1=lightcoord[i*6+3];
			float s2=lightcoord[i*6+4],t2=lightcoord[i*6+5];*/

			// force the regeneration of float values (seems perfect  for opengl/ renderman scan line conversion!)
			/*s0=(0.5f/(float)texturedim)+(((int)(s0*(float)texturedim))/(float)texturedim);t0=(0.5f/(float)texturedim)+(((int)(t0*(float)texturedim))/(float)texturedim);
			s1=(0.5f/(float)texturedim)+(((int)(s1*(float)texturedim))/(float)texturedim);t1=(0.5f/(float)texturedim)+(((int)(t1*(float)texturedim))/(float)texturedim);
			s2=(0.5f/(float)texturedim)+(((int)(s2*(float)texturedim))/(float)texturedim);t2=(0.5f/(float)texturedim)+(((int)(t2*(float)texturedim))/(float)texturedim);
			*/

			fprintf(file,Utils::Format("v %e %e %e\n",v0.x,v0.y,v0.z).c_str());
			fprintf(file,Utils::Format("v %e %e %e\n",v1.x,v1.y,v1.z).c_str());
			fprintf(file,Utils::Format("v %e %e %e\n",v2.x,v2.y,v2.z).c_str());

			//fprintf(file,Utils::Format("vt %e %e\n",s0,t0).c_str());
			//fprintf(file,Utils::Format("vt %e %e\n",s1,t1).c_str());
			//fprintf(file,Utils::Format("vt %e %e\n",s2,t2).c_str());

			fprintf(file,Utils::Format("vn %e %e %e\n",n0.x,n0.y,n0.z).c_str());
			fprintf(file,Utils::Format("vn %e %e %e\n",n1.x,n1.y,n1.z).c_str());
			fprintf(file,Utils::Format("vn %e %e %e\n",n2.x,n2.y,n2.z).c_str());

			fprintf(file,Utils::Format("f %d//%d %d//%d %d//%d\n",
				Cont+0,Cont+0,
				Cont+1,Cont+1,
				Cont+2,Cont+2).c_str());

			/*fprintf(file,Utils::Format("f %d/%d/%d %d/%d/%d %d/%d/%d\n",
				Cont+0,Cont+0,Cont+0,
				Cont+1,Cont+1,Cont+1,
				Cont+2,Cont+2,Cont+2).c_str());*/

			Cont+=3;
		}
	}

	fclose(file);
}


#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
std::vector<SmartPointer<Batch> >  Batch::openObj(std::string filename)
{
        std::vector<float> vertices ; 
	std::vector<float> normals  ; 
	std::vector<float> texcoords; 

	std::vector<tinyobj::shape_t>    shapes;
	std::vector<tinyobj::material_t> materials;
	std::string                      err;
	bool bOk = tinyobj::LoadObj(shapes, materials, err, filename.c_str());

	if (!err.empty()) 
	  std::cout << err << std::endl;

	if (!bOk) 
	  return std::vector<SmartPointer<Batch> >();

	for (size_t S = 0; S < shapes.size(); S++) 
        {
	      const tinyobj::shape_t& shape=shapes[S];
	      const tinyobj::mesh_t&  mesh=shape.mesh;
	      bool bValidM =mesh.material_ids.size()>0;
	      bool bValidN =mesh.normals     .size()>0;
	      bool bValidT =mesh.texcoords   .size()>0;

	      assert((mesh.indices.size() % 3) == 0);
	      for (size_t f = 0; f < mesh.indices.size() / 3; f++) 
	      {
		for (int I=0;I<3;I++)
		{
		  int idx=mesh.indices[3*f+I];
                  vertices.push_back(mesh.positions[3*idx+0]);
                  vertices.push_back(mesh.positions[3*idx+1]);
                  vertices.push_back(mesh.positions[3*idx+2]);
                  
		  // if (bValidM) gl.color(mesh.material_ids[f]>0? ccolor(materials[mesh.material_ids[f]]) : Colors::White);
		  if (bValidN) 
                  {
                     normals.push_back(mesh.normals[3*idx+0]);
                     normals.push_back(mesh.normals[3*idx+1]);
                     normals.push_back(mesh.normals[3*idx+2]);
                  }

		  if (bValidT) 
                  {
                    texcoords.push_back(mesh.texcoords[2*idx+0]);
                    texcoords.push_back(mesh.texcoords[2*idx+1]);
                  }
		}
	      }
	}

	if (vertices.empty())
	    return std::vector<SmartPointer<Batch> >();

	SmartPointer<Batch> batch(new Batch());
	batch->primitive=Batch::TRIANGLES;
	batch->vertices.reset(new Array(vertices));

	if (!normals.empty())  
	{
	  batch->normals.reset(new Array(normals));
	}
	else
	{
		//automatic calculation of normals
		normals.clear();

		int nt=batch->vertices->size()/9;
		float* t=(float*)batch->vertices->c_ptr();
		for (int i=0;i<nt;i++,t+=9)
		{
			Vec3f p0(t[0],t[1],t[2]);
			Vec3f p1(t[3],t[4],t[5]);
			Vec3f p2(t[6],t[7],t[8]);

			Vec3f n=((p1-p0).cross(p2-p0)).normalize();
			normals.push_back(n.x);normals.push_back(n.y);normals.push_back(n.z);
			normals.push_back(n.x);normals.push_back(n.y);normals.push_back(n.z);
			normals.push_back(n.x);normals.push_back(n.y);normals.push_back(n.z);
		}
		batch->normals.reset(new Array(normals));
	}


	if (!texcoords.empty())  
		batch->texture0coords.reset(new Array(texcoords));

	std::vector< SmartPointer<Batch> > ret;
	ret.push_back(batch);
	return ret;
}


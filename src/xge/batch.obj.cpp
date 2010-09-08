#include <xge/xge.h>

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
	FILE* file=fopen(FileSystem::FullPath(filename).c_str(),"wt");
	ReleaseAssert(file);

	Box3f box;
	for (int I=0;I<(int)batches.size();I++)
	{
		SmartPointer<Batch> batch=batches[I];
		box.add(batch->getBox());
	}

	Mat4f ToUnitBox= Mat4f::scale(1.0f/box.size().x,1.0f/box.size().y,1.0f/box.size().z) * Mat4f::translate(-box.p1.x,-box.p1.y,-box.p1.z);


	int Cont=1;

	for (int I=0;I<(int)batches.size();I++)
	{
		SmartPointer<Batch> batch=batches[I];

		//TODO: all other cases
		ReleaseAssert(batch->primitive==Batch::TRIANGLES);

		Mat4f matrix=batch->matrix;

		matrix=ToUnitBox * matrix;

		Mat4f inv=matrix.invert();

		/*ReleaseAssert(batch->texture1);
		ReleaseAssert(batch->texture1->width==batch->texture1->height);
		int texturedim=batch->texture1->width;*/

		int nv=batch->vertices->size()/3;
		int nt=nv/3;

		ReleaseAssert(batch->vertices);
		ReleaseAssert(batch->normals);
		//ReleaseAssert(batch->texture1coords);

		float* vertex     = batch->vertices->mem();
		float* normal     = batch->normals ->mem();
		//float* lightcoord = batch->texture1coords->mem();

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


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
std::vector<SmartPointer<Batch> >  Batch::openObj(std::string filename)
{
	char buf[2048];
	
	filename=FileSystem::FullPath(filename);

	//open the Text obj file
	FILE* file=fopen(filename.c_str(),"rt");

	if (!file)  
		return std::vector<SmartPointer<Batch> >();

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texcoords;
	
	std::vector<int> vertices_indices;
	std::vector<int> normals_indices;
	std::vector<int> texcoords_indices;

	int nline=0;
	while (fscanf(file, "%s", buf) != EOF) 
	{
		switch (buf[0]) 
		{		
		case 'v': /* vertex! */
			{
				if (!buf[1])
				{
					float X,Y,Z;
					fscanf(file, "%e %e %e", &X, &Y, &Z);
					vertices.push_back(X);
					vertices.push_back(Y);
					vertices.push_back(Z);

				}
				else if (buf[1]=='n')
				{
					float NX,NY,NZ;
					fscanf(file, "%e %e %e", &NX, &NY, &NZ);
					normals.push_back(NX);
					normals.push_back(NY);
					normals.push_back(NZ);
				}
				else if (buf[1]=='t')
				{
					float TX, TY;
					fscanf(file, "%e %e", &TX, &TY);
					texcoords.push_back(TX);
					texcoords.push_back(TY);
				}
				nline++;
				break;
			}
			
		case 'f': /* face */
			{	
				int c;
				int V,N,T;

				for (int i=0;i<3;i++)
				{
					fscanf(file,"%d",&V);

					if (V<0) V=((int)vertices.size()/3)+V; else V--;

					ReleaseAssert(V>=0 && V<((int)vertices.size()/3));
				
					vertices_indices.push_back(V*3+0); 
					vertices_indices.push_back(V*3+1);
					vertices_indices.push_back(V*3+2); 

					 /* v/t/n */
					if (normals.size() && texcoords.size())
					{
						c=fgetc(file);
						ReleaseAssert(c=='/');
						fscanf(file,"%d",&T);
						if (T<0) V=((int)texcoords.size()/2)+T; else T--;
						ReleaseAssert(T>=0 && T<((int)texcoords.size()/2));

						texcoords_indices.push_back(T*2+0);
						texcoords_indices.push_back(T*2+1);

						c=fgetc(file);
						ReleaseAssert(c=='/');
						fscanf(file,"%d",&N);
						if (N<0) N=((int)normals.size()/3)+N; else N--;
						ReleaseAssert(N>=0 && N<((int)normals.size()/3));

						normals_indices.push_back(N*3+0);
						normals_indices.push_back(N*3+1);
						normals_indices.push_back(N*3+2);
					}
					else if (normals.size()) /* v//n */
					{
						c=fgetc(file);
						ReleaseAssert(c=='/');
						c=fgetc(file);
						ReleaseAssert(c=='/');
						fscanf(file,"%d",&N);
						if (N<0) N=((int)normals.size()/3)+N; else N--;
						ReleaseAssert(N>=0 && N<((int)normals.size()/3));

						normals_indices.push_back(N*3+0);
						normals_indices.push_back(N*3+1);
						normals_indices.push_back(N*3+2);
					}
					else if (texcoords.size())
					{
						c=fgetc(file);
						ReleaseAssert(c=='/');
						fscanf(file,"%d",&T);
						if (T<0) V=((int)texcoords.size()/2)+T; else T--;
						ReleaseAssert(T>=0 && T<((int)texcoords.size()/2));

						texcoords_indices.push_back(T*2+0);
						texcoords_indices.push_back(T*2+1);

						c=fgetc(file);
						if (c!='/') ungetc(c,file);
					}

					if (i<2) 
					{
						c=fgetc(file);
						ReleaseAssert(c==' ');
					}
				}
				
				nline++;
				break;
			}

		default: /* skip the line */
			{
				skipLine(file);
				nline++;
				break;
			}
		}
	}

	fclose(file);
	
	SmartPointer<Batch> batch(new Batch);
	batch->primitive=Batch::TRIANGLES;

	if (!vertices.size() || !vertices_indices.size())
		return std::vector<SmartPointer<Batch> >();

	batch->vertices.reset(new Vector(vertices_indices,vertices));

	if (normals_indices .size() && normals.size())  
	{
		batch->normals.reset(new Vector(normals_indices,normals));
	}
	else
	{
		//automatic calculation of normals
		normals.clear();

		int nt=batch->vertices->size()/9;
		float* t=batch->vertices->mem();
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
		batch->normals.reset(new Vector(normals));
	}


	if (texcoords_indices.size() && texcoords.size())  
		batch->texture0coords.reset(new Vector(texcoords_indices,texcoords));

	std::vector<SmartPointer<Batch> > ret;
	ret.push_back(batch);
	return ret;
}


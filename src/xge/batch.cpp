
#include <xge/xge.h>
#include <xge/Batch.h>
#include <xge/Pick.h>

Color4f Batch::DefaultAmbientColor   =  Color4f(0.2f,0.2f,0.2f,1);
Color4f Batch::DefaultDiffuseColor   =  Color4f(0.8f,0.8f,0.8f,1);
Color4f Batch::DefaultSpecularColor  =  Color4f(0.1f,0.1f,0.1f,1);
Color4f Batch::DefaultEmissionColor  =  Color4f::Black();
float   Batch::DefaultShininess      =  100.f;

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
SmartPointer<Batch> Batch::Merge(SmartPointer<Batch> _A,SmartPointer<Batch> _B)
{
	if (!_A) return _B;
	if (!_B) return _A;

	Batch* A=_A.get();
	Batch* B=_B.get();

	if (A->primitive!=B->primitive) 
		return SmartPointer<Batch>();

	if (A->ambient  !=B->ambient  ) 
		return SmartPointer<Batch>();

	if (A->diffuse  !=B->diffuse  ) 
		return SmartPointer<Batch>();

	if (A->specular !=B->specular ) 
		return SmartPointer<Batch>();

	if (A->emission !=B->emission ) 
		return SmartPointer<Batch>();

	if (A->shininess!=B->shininess) 
		return SmartPointer<Batch>();

	if ((A->vertices && !B->vertices) || (!A->vertices && B->vertices)) 
		return SmartPointer<Batch>();

	if ((A->normals  && !B->normals ) || (!A->normals  && B->normals )) 
		return SmartPointer<Batch>();

	if ((A->colors   && !B->colors  ) || (!A->colors   && B->colors  )) 
		return SmartPointer<Batch>();

	bool ATex0=(A->texture0) && (A->texture0coords);
	bool BTex0=(B->texture0) && (B->texture0coords);
	if ((ATex0 && !BTex0) || (!ATex0 && BTex0) || (ATex0 && BTex0 && A->texture0.get()!=B->texture0.get())) 
		return SmartPointer<Batch>();

	bool ATex1=(A->texture1) && (A->texture1coords);
	bool BTex1=(B->texture1) && (B->texture1coords);
	if ((ATex1 && !BTex1) || (!ATex1 && BTex1) || (ATex1 && BTex1 && A->texture1.get()!=B->texture1.get())) 
		return SmartPointer<Batch>();

	SmartPointer<Batch> ret(new Batch());
	ret->matrix=Mat4f();
	ret->primitive=A->primitive;
	ret->ambient  =A->ambient;
	ret->diffuse  =A->diffuse;
	ret->specular =A->specular;
	ret->emission =A->emission;
	ret->shininess=A->shininess;

	//vertices
	if (A->vertices)
	{
		Vector VA(*(A->vertices));
		{
			Mat4f T=A->matrix;
			float* p=VA.mem();
			for (int i=0;i<VA.size();i+=3,p+=3)
			{
				Vec3f V=T * Vec3f(p[0],p[1],p[2]);
				p[0]=V.x;p[1]=V.y;p[2]=V.z;
			}
		}

		Vector VB(*(B->vertices));
		{
			Mat4f T=B->matrix;
			float* p=VB.mem();
			for (int i=0;i<VB.size();i+=3,p+=3)
			{
				Vec3f V=T * Vec3f(p[0],p[1],p[2]);
				p[0]=V.x;p[1]=V.y;p[2]=V.z;
			}
		}

		ret->vertices.reset(new Vector(VA));
		ret->vertices->append(VB);
	}

	//normals
	if (A->normals)
	{
		Vector NA(*(A->normals));
		{
			Mat4f T=A->matrix.invert();
			float* p=NA.mem();
			for (int i=0;i<NA.size();i+=3,p+=3)
			{
				Vec4f _N=Vec4f(p[0],p[1],p[2],0.0) *  T;
				Vec3f  N=Vec3f(_N.x,_N.y,_N.z).normalize();
				p[0]=N.x;p[1]=N.y;p[2]=N.z;
			}
		}

		Vector NB(*(B->normals));
		{
			Mat4f T=B->matrix.invert();
			float* p=NB.mem();
			for (int i=0;i<NB.size();i+=3,p+=3)
			{
				Vec4f _N=Vec4f(p[0],p[1],p[2],0.0) *  T;
				Vec3f  N=Vec3f(_N.x,_N.y,_N.z).normalize();
				p[0]=N.x;p[1]=N.y;p[2]=N.z;
			}
		}

		ret->normals.reset(new Vector(NA));
		ret->normals->append(NB);
	}

	//colors
	if (A->colors)
	{
		ret->colors.reset(new Vector(*(A->colors)));
		ret->colors->append(*(B->colors));
	}

	//texture 0
	if (ATex0)
	{
		ret->texture0=A->texture0;
		ret->texture0coords.reset(new Vector(*(A->texture0coords)));
		ret->texture0coords->append(*(B->texture0coords));
	}
	
	//texture 1
	if (ATex1)
	{
		ret->texture1=A->texture1;
		ret->texture1coords.reset(new Vector(*(A->texture1coords)));
		ret->texture1coords->append(*(B->texture1coords));
	}

	return ret;
}


///////////////////////////////////////////////////////////////////////////////////////
std::vector<SmartPointer<Batch> > Batch::Optimize(std::vector<SmartPointer<Batch> > batches,int max_vertices_per_batch,int max_depth,float LOOSE_K) 
{
	std::vector<SmartPointer<Batch> > ret;

	Clock t1;
	Log::printf("Optimizing the octree....\n");
	Log::printf("   Number of input batches %d\n",(int)batches.size());

	//calculate world box
	Box3f world_box;
	for (int i=0;i<(int)batches.size();i++)
		world_box.add(batches[i]->getBox());

	//build the octree for doing optimization
	Octree octree(world_box,max_depth,LOOSE_K);
	for (int i=0;i<(int)batches.size();i++)
		octree.getNode(batches[i]->getBox())->batches.push_back(batches[i]);

	std::stack<OctreeNode*> stack;
	stack.push(octree.root);

	//count number of vertices
	int tot_number_of_vertices=0;
	while (stack.size())
	{
		OctreeNode* node=stack.top();
		stack.pop();

		for (int i=0;i<8;i++)
			{if (node->childs[i]) stack.push(node->childs[i]);}

		//rebuild the batches
		std::set<int> merged;

		int N=(int)node->batches.size();
		for (int I=0;I<N;I++)
		{
			if (merged.find(I)!=merged.end()) continue;

			SmartPointer<Batch> A=node->batches[I];
			if (A->vertices)
			{
				int nva=A->vertices->size()/3;
				tot_number_of_vertices+=nva;

				for (int J=I+1;nva < max_vertices_per_batch && J<N;J++)
				{	
					//already merged, skip
					if (merged.find(J)!=merged.end()) continue;
					SmartPointer<Batch> B=node->batches[J];
					if (B->vertices)
					{
						int nvb=B->vertices->size()/3;
						if ((nva+nvb)<=max_vertices_per_batch) //vertex limits
						{
							SmartPointer<Batch> C=Batch::Merge(A,B);
							if (!C) 
								continue;
							A=C;
							merged.insert(J);
							nva+=nvb;
						}
					}
				}
				//Log::printf("   num vertices %d\n",A->vertices->size()/3);
			}
			ret.push_back(A);
		}
	}

	Log::printf("   total number vertices    %d\n",(int)tot_number_of_vertices);
	Log::printf("   Number of output batches %d\n",(int)ret.size());
	Log::printf("   Batch vertex media       %d\n",(int)(tot_number_of_vertices/(float)ret.size()));
	Log::printf("...done in %d msec\n",(int)t1.msec());
	return ret;
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void Batch::Write(Archive& ar)
{
	static int BATCH_VERSION=1;

	ar.WriteInt("version",BATCH_VERSION);
	ar.WriteInt("primitive",primitive);    	

	ar.Push("ambient" );ambient.Write (ar);ar.Pop("ambient");
	ar.Push("diffuse" );diffuse.Write (ar);ar.Pop("diffuse");
	ar.Push("specular");specular.Write(ar);ar.Pop("specular");
	ar.Push("emission");emission.Write(ar);ar.Pop("emission");
	ar.WriteFloat("shininess",shininess);

	ar.Push("matrix"   );matrix.Write(ar);ar.Pop("matrix");

	ar.Push("vertices" );ar.WriteSmartPointer(vertices);ar.Pop("vertices");
	ar.Push("normals"  );ar.WriteSmartPointer(normals );ar.Pop("normals");
	ar.Push("colors"   );ar.WriteSmartPointer(colors  );ar.Pop("colors");
	ar.WriteString("texture0",this->texture0?texture0->filename:"");
	ar.Push("texture0coords");ar.WriteSmartPointer(texture0coords);ar.Pop("texture0coords");
	ar.WriteString("texture1",this->texture1?texture1->filename:"");
	ar.Push("texture1coords");ar.WriteSmartPointer(texture1coords);ar.Pop("texture1coords");

	//important: I do not write the bounding box getBox();
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void Batch::Read(Archive& ar)
{
	int version=ar.ReadInt("version");
	this->primitive=ar.ReadInt("primitive"); 
	ar.Push("ambient" );ambient.Read (ar);ar.Pop("ambient");
	ar.Push("diffuse" );diffuse.Read (ar);ar.Pop("diffuse");
	ar.Push("specular");specular.Read(ar);ar.Pop("specular");
	ar.Push("emission");emission.Read(ar);ar.Pop("emission");
	this->shininess=ar.ReadFloat("shininess");
	ar.Push("matrix"  );matrix.Read(ar);ar.Pop("matrix");
	ar.Push("vertices");vertices=ar.ReadSmartPointer<Vector>();ar.Pop("vertices");
	ar.Push("normals" );normals=ar.ReadSmartPointer<Vector>();ar.Pop("normals");
	ar.Push("colors" ) ;colors=ar.ReadSmartPointer<Vector>();ar.Pop("colors");
	this->texture0.reset();std::string skin_filename=ar.ReadString("texture0");
	if (skin_filename.length()) this->texture0 =Texture::open(skin_filename);
	ar.Push("texture0coords");texture0coords=ar.ReadSmartPointer<Vector>();ar.Pop("texture0coords");
	this->texture1.reset();std::string light_filename=ar.ReadString("texture1");
	if (light_filename.length()) this->texture1 =Texture::open(light_filename);
	ar.Push("texture1coords");texture1coords=ar.ReadSmartPointer<Vector>();ar.Pop("texture1coords");

	//I force a recalculation of bounding box
	this->invalidateBox();
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
SmartPointer<Batch> Batch::Quad(int x1,int y1,int x2,int y2,int z)
{
	SmartPointer<Batch> batch(new Batch);
	batch->primitive=Batch::QUADS;
	batch->setColor(Color4f(1,1,1));
	
	float _vertices[] = {
		x1,y1,z,  
		x2,y1,z,  
		x2,y2,z, 
		x1,y2,z
	};

	batch->vertices.reset(new Vector(12,_vertices));

	float _texture0coords[] =
	{
		0,0, 
		1,0, 
		1,1, 
		0,1
	};
	
	batch->texture0coords.reset(new Vector(8,_texture0coords));

	return batch;
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
SmartPointer<Batch> Batch::Cube(const Box3f& box)
{
	SmartPointer<Vector> vertices;
	vertices.reset(new Vector(6*4*3));float* V=vertices->mem();
	
	SmartPointer<Vector> normals;
	normals.reset (new Vector(6*4*3));float* N=normals->mem();

	static float n[6][3]   = {{-1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{1.0, 0.0, 0.0},{0.0, -1.0, 0.0},{0.0, 0.0, 1.0},{0.0, 0.0, -1.0}};
	static int faces[6][4] = {{0, 1, 2, 3},{3, 2, 6, 7},{7, 6, 5, 4},{4, 5, 1, 0},{5, 6, 2, 1},{7, 4, 0, 3}};

	float v[8][3];
	v[0][0] = v[1][0] = v[2][0] = v[3][0] = v[0][1] = v[1][1] = v[4][1] = v[5][1] = v[0][2] = v[3][2] = v[4][2] = v[7][2] = 0;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = v[2][1] = v[3][1] = v[6][1] = v[7][1] = v[1][2] = v[2][2] = v[5][2] = v[6][2] = 1;

	for (int i = 5; i >= 0; i--) 
	{
		float* v0=&v[faces[i][0]][0];* V++=v0[0]; *V++=v0[1]; *V++=v0[2];
		float* v1=&v[faces[i][1]][0];* V++=v1[0]; *V++=v1[1]; *V++=v1[2];
		float* v2=&v[faces[i][2]][0];* V++=v2[0]; *V++=v2[1]; *V++=v2[2];
		float* v3=&v[faces[i][3]][0];* V++=v3[0]; *V++=v3[1]; *V++=v3[2];

		float* normal=&n[i][0];
		
		*N++=normal[0];*N++=normal[1];*N++=normal[2];
		*N++=normal[0];*N++=normal[1];*N++=normal[2];
		*N++=normal[0];*N++=normal[1];*N++=normal[2];
		*N++=normal[0];*N++=normal[1];*N++=normal[2];
	}

	SmartPointer<Batch> batch(new Batch);
	batch->primitive=Batch::QUADS;
	batch->matrix=Mat4f::translate(box.p1)*Mat4f::scale(box.size());
	batch->vertices = vertices;
	batch->normals  = normals;
	return batch;
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
std::vector<SmartPointer<Batch> > Batch::Sky(const Box3f& box,std::string sky_name)
{	
	Vec3f p1=box.p1;
	Vec3f p2=box.p2;

	float Zer=0.001f;
	float One=0.999f;

	std::vector<SmartPointer<Batch> > batches;

	{
		float v[]={p1.x,p1.y, p2.z, p2.x,p1.y, p2.z,p2.x,p2.y, p2.z,p1.x,p2.y, p2.z};
		float t[]={One,One,One,Zer,Zer,Zer,Zer,One};
		SmartPointer<Batch> batch(new Batch);
		batch->primitive=Batch::QUADS;
		batch->ambient  =Color4f(1,1,1,1);
		batch->diffuse  =Color4f(0,0,0,1);
		batch->vertices.reset(new Vector(4*3,v));
		batch->texture0=Texture::open(Utils::Format(sky_name.c_str(),"up"));
		batch->texture0coords.reset(new Vector(4*2,t));
		batches.push_back(batch);
	}

	{
		float v[]={p1.x, p1.y,p2.z,p1.x, p2.y,p2.z,p1.x, p2.y,p1.z,p1.x, p1.y,p1.z};
		float t[]={Zer,One,One,One,One,Zer,Zer,Zer};
		SmartPointer<Batch> batch(new Batch);
		batch->primitive=Batch::QUADS;
		batch->ambient  =Color4f(1,1,1,1);
		batch->diffuse  =Color4f(0,0,0,1);
		batch->vertices.reset(new Vector(4*3,v));
		batch->texture0=Texture::open(Utils::Format(sky_name.c_str(),"back"));
		batch->texture0coords.reset(new Vector(4*2,t));
		batches.push_back(batch);
	}

	{
		float v[]={p1.x,p1.y,p1.z,p1.x,p1.y,p2.z,p2.x,p1.y,p2.z,p2.x,p1.y,p1.z};
		float t[]={One,Zer,One,One,Zer,One,Zer,Zer};
		SmartPointer<Batch> batch(new Batch);
		batch->primitive=Batch::QUADS;
		batch->ambient  =Color4f(1,1,1,1);
		batch->diffuse  =Color4f(0,0,0,1);
		batch->vertices.reset(new Vector(4*3,v));
		batch->texture0=Texture::open(Utils::Format(sky_name.c_str(),"right"));
		batch->texture0coords.reset(new Vector(4*2,t));
		batches.push_back(batch);
	}

	{
		float v[]={p1.x,p2.y,p1.z,p1.x,p2.y,p2.z,p2.x,p2.y,p2.z,p2.x,p2.y,p1.z};
		float t[]={Zer,Zer,Zer,One,One,One,One,Zer};
		SmartPointer<Batch> batch(new Batch);
		batch->primitive=Batch::QUADS;
		batch->ambient  =Color4f(1,1,1,1);
		batch->diffuse  =Color4f(0,0,0,1);
		batch->vertices.reset(new Vector(4*3,v));
		batch->texture0=Texture::open(Utils::Format(sky_name.c_str(),"left"));
		batch->texture0coords.reset(new Vector(4*2,t));
		batches.push_back(batch);
	}

	{
		float v[]={p2.x,p1.y,p2.z,p2.x,p2.y,p2.z,p2.x,p2.y,p1.z,p2.x,p1.y,p1.z};
		float t[]={One,One,Zer,One,Zer,Zer,One,Zer};
		SmartPointer<Batch> batch(new Batch);
		batch->primitive=Batch::QUADS;
		batch->ambient  =Color4f(1,1,1,1);
		batch->diffuse  =Color4f(0,0,0,1);
		batch->vertices.reset(new Vector(4*3,v));
		batch->texture0=Texture::open(Utils::Format(sky_name.c_str(),"front"));
		batch->texture0coords.reset(new Vector(4*2,t));
		batches.push_back(batch);
	}

	/*
	{
		float v[]={p2.x,p1.y,p1.z,p2.x,p2.y,p1.z,p1.x,p2.y,p1.z,p1.x,p1.y,p1.z};
		float t[]={Zer,Zer,One,Zer,One,One,Zer,One};
		SmartPointer<Batch> batch(new Batch);
		batch->primitive=Batch::QUADS;
		batch->ambient  =Color4f(1,1,1,1);
		batch->diffuse  =Color4f(0,0,0,1);
		batch->vertices.reset(new Vector(4*3,v));
		batch->texture0=Texture::open(Utils::Format(sky_name.c_str(),"down"));
		Engine::transferToGraphicCard(batch->texture0,false);
		batch->texture0coords.reset(new Vector(4*2,t));
		batches.push_back(batch);
	}*/

	return batches;
};


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
SmartPointer<Batch> Batch::Circle(float angle_delta)
{
	SmartPointer<Batch> batch(new Batch);
	batch->primitive=Batch::POLYGON;

	std::vector<float> v;
	std::vector<float> n;

    float angle=0;
    while (angle<2*M_PI)
	{
        float X=cos(angle),Y=sin(angle);
		v.push_back(X);
		v.push_back(Y);
		v.push_back(0);

		n.push_back(0);
		n.push_back(0);
		n.push_back(1);

        angle+=angle_delta;
	}

	batch->vertices.reset(new Vector(v));
	batch->normals.reset (new Vector(n));
	return batch;
};


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
SmartPointer<Batch> Batch::getTriangles(const std::vector<int>& triangle_indices)
{
	DebugAssert(this->primitive==Batch::TRIANGLES);

	SmartPointer<Batch> ret(new Batch(*this));
	
	//i need to convert indices to vertices -> to indices to elements in the arrays
	std::vector<int> i2(triangle_indices.size()*2);
	std::vector<int> i3(triangle_indices.size()*3);
	for (int i=0;i<(int)triangle_indices.size();i++)
	{
		i2[i*2+0]=triangle_indices[i]*2+0;
		i2[i*2+1]=triangle_indices[i]*2+1;

		i3[i*3+0]=triangle_indices[i]*3+0;
		i3[i*3+1]=triangle_indices[i]*3+1;
		i3[i*3+2]=triangle_indices[i]*3+2;
	}

	if (this->vertices   ) ret->vertices   .reset(new Vector(i3,this->vertices   ->mem()));
	if (this->normals    ) ret->normals    .reset(new Vector(i3,this->normals    ->mem()));
	if (this->colors     ) ret->colors     .reset(new Vector(i3,this->colors     ->mem()));
	if (this->texture0coords ) ret->texture0coords .reset(new Vector(i2,this->texture0coords ->mem())); //2 because each texcoord has 2 components
	if (this->texture1coords) ret->texture1coords.reset(new Vector(i2,this->texture1coords->mem()));

	return ret;
}



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
Box3f Batch::getBox()
{
	//calculate the box for the first time
	if (!this->box.isValid() && this->vertices)
	{
		float* p=this->vertices->mem();
		for (int I=0;I<(int)this->vertices->size();I+=3,p+=3)
		{
			Vec3f V(p);
			V=this->matrix * V;
			this->box.add(V);
		}
	}

	return this->box;
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
Pick Batch::getPick(const Ray3f& ray,Mat4f global_matrix)
{
	//first test if the box hits the ray
	#if 1
	{
		Box3f box=this->getBox();

		if (!box.isValid())
			return Pick();//invalid pick

		//if a transformation is applyied to this batch...
		Box3f transformed_box;
		for (int i=0;i<8;i++)
			transformed_box.add(global_matrix * box.getPoint(i));
		box=transformed_box;

		float tmin,tmax;
		if (!box.isValid() || !ray.intersectBox(tmin,tmax,box))
			return Pick(); //invalid pick
	}
	#endif

	//invalud pick
	Pick pick;

	if (!this->vertices)
		return pick;

	//invalid pick
	if (!this->vertices)
		return pick;

	int tot=this->vertices->size();
	const float* pv=this->vertices->mem();

	//TRIANGLES
	if (this->primitive==Batch::TRIANGLES)
	{
		for (int i=0;i<tot;i+=9,pv+=9)
		{
			Vec3f v0(pv+0); 
			Vec3f v1(pv+3); 
			Vec3f v2(pv+6);

			v0=global_matrix * (this->matrix * v0);
			v1=global_matrix * (this->matrix * v1);
			v2=global_matrix * (this->matrix * v2);
			

			float distance=ray.intersectTriangle(v0,v1,v2);

			if (distance>=0 && (!pick.valid || pick.distance>distance))
			{
				//fill up useful information
				pick.valid=true;
				pick.distance=distance;
				pick.position=ray.getPoint(distance);
				pick.normal=(v1-v0).cross(v2-v0).normalize();
				pick.path.clear();
				pick.path.push_back(i);
			}
		}
	}

	//QUADS
	if (this->primitive==Batch::QUADS)
	{
		for (int i=0;i<tot;i+=12,pv+=12)
		{
			Vec3f v0(pv+0); 
			Vec3f v1(pv+3); 
			Vec3f v2(pv+6);
			Vec3f v3(pv+9);

			v0=global_matrix * (this->matrix * v0);
			v1=global_matrix * (this->matrix * v1);
			v2=global_matrix * (this->matrix * v2);
			v3=global_matrix * (this->matrix * v3);

			float distance=ray.intersectTriangle(v0,v1,v2);

			//first triangle
			if (distance>=0 && (!pick.valid || pick.distance>distance))
			{
				pick.valid=true;
				pick.distance=distance;
				pick.position=ray.getPoint(distance);
				pick.normal=(v1-v0).cross(v2-v0).normalize();
				pick.path.clear();
				pick.path.push_back(i);
			}

			distance=ray.intersectTriangle(v0,v2,v3);

			//second triangle
			if (distance>=0 && (!pick.valid || pick.distance>distance))
			{
				pick.valid=true;
				pick.distance=distance;
				pick.position=ray.getPoint(distance);
				pick.normal=(v1-v0).cross(v2-v0).normalize();
				pick.path.clear();
				pick.path.push_back(i);
			}
		}
	}

	return pick;
}




///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
SmartPointer<Batch> Batch::getNormals()
{
	float* v=this->vertices->mem();
	float* n=this->normals ->mem();

	Mat4f D=this->matrix;

	//this is how opengl works
	//see http://books.google.it/books?id=H4eYq7-2YhYC&pg=PA23&lpg=PA23&dq=opengl+normal+transform&source=bl&ots=q9ZGuKZ2S3&sig=_vc67-fLbPl4CZC-oEgX9eUxIsA&hl=it&ei=tv3qSv71KYLwmwOo4KSaDw&sa=X&oi=book_result&ct=result&resnum=4&ved=0CBgQ6AEwAw#v=onepage&q=opengl%20normal%20transform&f=false
	Mat4f Dit=Mat4f(
		D[ 0],D[ 1],D[ 2],0,
		D[ 4],D[ 5],D[ 6],0,
		D[ 8],D[ 9],D[10],0,
		    0,    0,    0,1).invert().transpose();

	SmartPointer<Batch> batch(new Batch);
	batch->primitive=Batch::LINES;
	batch->setColor(Color4f::White());
	batch->vertices.reset(new Vector(6*vertices->size()));

	float* p=batch->vertices->mem();
	for (int i=0;i<vertices->size();i+=3,v+=3,n+=3)
	{
		Vec3f p1=    (D   * Vec3f(v[0],v[1],v[2]));
		Vec3f p2=p1+ (Dit * Vec3f(n[0],n[1],n[2])).normalize();

		*v++=p1.x;*v++=p1.y;*v++=p1.z;
		*v++=p2.x;*v++=p2.y;*v++=p2.z;
	}

	return batch;
}


///////////////////////////////////////////////////////////////////////////
void Batch::Save(std::string filename,std::vector< SmartPointer<Batch> > batches)
{
	Clock t1;
	printf("Saving file %s\n",filename.c_str());
	std::set<std::string> light_textures;
	Archive ar;
	ar.Open(filename,true);
	ar.Push("batches");
	int num=(int)batches.size();
	ar.WriteInt("num",num);
	for (int i=0;i<num;i++) 
	{
		ar.Push("batch");
		ar.WriteSmartPointer(batches[i]);
		ar.Pop("batch");

		//save the light maps
		if (batches[i]->texture1 && light_textures.find(batches[i]->texture1->filename)==light_textures.end())
		{
			light_textures.insert(batches[i]->texture1->filename);
			batches[i]->texture1->save();
		}
	}
	ar.Pop("batches");
	ar.Close();
	printf("done in %d msec\n",t1.msec());
}


///////////////////////////////////////////////////////////////////////////
std::vector< SmartPointer<Batch> > Batch::Open(std::string filename)
{
	Clock t1;
	printf("Opening file %s\n",filename.c_str());
	Archive ar;
	ar.Open(filename,false);
	ar.Push("batches");
	int num=ar.ReadInt("num");
	std::vector< SmartPointer<Batch> > batches(num);
	for (int i=0;i<num;i++) 
	{
		ar.Push("batch");
		batches[i]=ar.ReadSmartPointer<Batch>();
		ar.Pop("batch");
	}
	ar.Pop("batches");
	ar.Close();
	printf("done in %d msec\n",t1.msec());
	return batches;
}



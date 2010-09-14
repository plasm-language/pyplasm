#include <xge/xge.h>
#include <scheme.h>

#define OBJSCHEME_GET_CPP_OBJ(obj) scheme_struct_ref(obj, 0)
#define OBJSCHEME_SET_CPP_OBJ(obj, v) scheme_struct_set(obj, 0, v)

void objscheme_init(); 
void objscheme_add_procedures(Scheme_Env *);
Scheme_Object *objscheme_make_class(const char *name, Scheme_Object *sup, Scheme_Prim *initf, int num_methods);
Scheme_Object *objscheme_add_method_w_arity(Scheme_Object *c, const char *name, Scheme_Prim *f, int mina, int maxa);
Scheme_Object *objscheme_make_uninited_object(Scheme_Object *sclass);
Scheme_Object *objscheme_find_method(Scheme_Object *obj, char *name, void **cache);
int objscheme_is_a(Scheme_Object *o, Scheme_Object *c);
Scheme_Object *objscheme_add_closed_method_w_arity(Scheme_Object *c, void* data,const char *name,Scheme_Closed_Prim *f, int mina, int maxa);
extern "C" Scheme_Object *scheme_Plasm_initialize(Scheme_Env *env);
static Scheme_Object* PolComplexSchemeClass=0;
static void mzscheme_pol_complex_finilize(void *p, void *data);


//////////////////////////////////////////////////////////////////////////////////////////
//parser of mzscheme arguments
//////////////////////////////////////////////////////////////////////////////////////////
class Args
{
	
	const char* name;
	int narg;
	int argc;
	Scheme_Object** argv;

public:

	//constructor
	inline Args(const char* _name,int _argc,Scheme_Object** _argv,int minc,int maxc)
		:name(_name),narg(0),argc(_argc),argv(0)
	{
		if (argc<0)
			scheme_signal_error("c++::empty list in %s function",name);

		checkNumArgs(minc,maxc);

		if (argc)
		{
			argv=new Scheme_Object*[argc];
			for (int i=0;i<argc;i++) argv[i]=_argv[i];
		}
	}

	//constructor
	inline Args(const Args& src)
		:name(src.name),narg(0),argc(src.argc),argv(0)
	{
		if (argc)
		{
			this->argv =new Scheme_Object*[argc];
			for (int i=0;i<argc;i++) this->argv[i]=src.argv[i];
		}
	}

	//constructor
	inline Args(const char* _name,Scheme_Object* list)
		:name(_name),narg(0),argv(0)
	{
		if (!SCHEME_LISTP(list))
			scheme_signal_error("c++::invalid list %V in %s function",list,name);

		this->argc=scheme_proper_list_length(list);

		if (this->argc<0)
			scheme_signal_error("c++::empty list in %s function",name);

		if (argc)
		{
			this->argv=new Scheme_Object*[this->argc];
			for (int i=0;i<this->argc;i++,list = scheme_cdr(list)) this->argv[i]=scheme_car(list);
		}
	}

	//destructor
	inline ~Args()
	{
		if (this->argv) delete [] this->argv;
	}

	//check number of arguments
	inline void checkNumArgs(int minc,int maxc)
	{
		if (minc!=-1 && !(argc>=minc)) scheme_wrong_count(name,minc,maxc,argc,argv);
		if (maxc!=-1 && !(argc<=maxc)) scheme_wrong_count(name,minc,maxc,argc,argv);
	}

	//remaining number of args 
	inline int remaining()
		{return (argc-narg);}

	//float parser (static)
	static inline float Float(Scheme_Object* obj)
	{
		if (SCHEME_INTP(obj))		return (float)SCHEME_INT_VAL(obj);
		if (SCHEME_DBLP(obj))		return (float)SCHEME_DBL_VAL(obj);
		if (SCHEME_RATIONALP(obj))  return (float)scheme_rational_to_double(obj);
		return 0;
	}

	//float from arguments
	inline float Float()
	{
		if (narg>=argc)  scheme_signal_error("c++::too few paramenters for %s function",name);
		     if (SCHEME_INTP(argv[narg]))      return (float)SCHEME_INT_VAL(argv[narg++]);
		else if (SCHEME_DBLP(argv[narg]))      return (float)SCHEME_DBL_VAL(argv[narg++]);
		else if (SCHEME_RATIONALP(argv[narg])) return (float)scheme_rational_to_double(argv[narg++]);
		scheme_wrong_type(name,"Float",narg,argc,argv);
		return 0;
	}

	//int parser
	inline int Int()
	{
		if (narg>=argc) scheme_signal_error("c++::too few paramenters for %s function",name);
		     if (SCHEME_BOOLP(argv[narg])) return SCHEME_TRUEP(argv[narg++])?1:0;
		else if (SCHEME_INTP( argv[narg])) return SCHEME_INT_VAL(argv[narg++]);
		scheme_wrong_type(name,"Int",narg,argc,argv);
		return 0;
	}
	
	//string parser
	inline char* String()
	{
		if (narg>=argc) scheme_signal_error("c++::too few paramenters for %s function",name);
		     if (SCHEME_BYTE_STRINGP(argv[narg])) return SCHEME_BYTE_STR_VAL(argv[narg++]);
		else if (SCHEME_CHAR_STRINGP(argv[narg])) return SCHEME_BYTE_STR_VAL(scheme_char_string_to_byte_string(argv[narg++]));
		else if (SCHEME_SYMBOLP(argv[narg])     ) return SCHEME_SYM_VAL(argv[narg++]);
		scheme_wrong_type(name,"String",narg,argc,argv);
		return 0;
	}

	//pol parser
	inline SmartPointer<Hpc> Pol()
	{
		if (narg>=argc)  scheme_signal_error("c++::too few paramenters for %s function",name);
		if (objscheme_is_a(argv[narg],PolComplexSchemeClass))  return *((SmartPointer<Hpc>*)OBJSCHEME_GET_CPP_OBJ(argv[narg++]));
		scheme_wrong_type(name,"pol-complex",narg,argc,argv);
		return  SmartPointer<Hpc>();
	}

	//scheme parser
	inline Scheme_Object* Obj()
	{
		if (narg>=argc)  scheme_signal_error("c++::too few paramenters for %s function",name);
		return argv[narg++];
	}

	//scheme function
	inline Scheme_Object* Function()
	{
		if (narg>=argc) scheme_signal_error("c++::too few paramenters for %s function",name);
		return argv[narg++];
	}

	//get list
	Args getList()
	{
		if (narg>=argc) scheme_signal_error("c++::too few paramenters for %s function",name);
		if (SCHEME_LISTP(argv[narg])) return Args(name,argv[narg++]);
		scheme_signal_error("c++::invalid list %V in %s function",argv[narg],name);
		return Args(*this);
	}

	//return a wrapped integer value
	static inline Scheme_Object* Wrap(int value)
	{
		return scheme_make_integer_value(value);
	}

	//return a wrapped float value
	static inline Scheme_Object* Wrap(float value)
	{
		#if 1
		value = (float)(floor( 1e6l * value + 0.5l ) / 1e6l); //don't let the numbers to be too precise
		#endif
		return scheme_make_double(value);
	}

	//return a wrappend string value
	static inline Scheme_Object* Wrap(const char* value)
	{
		return scheme_byte_string_to_char_string(scheme_make_byte_string(value));
	}

	//return a wrapped polcomplex value
	inline Scheme_Object* Wrap(SmartPointer<Hpc> hpc)
	{
		//I create a new pointer!
		SmartPointer<Hpc>* __pointer=new SmartPointer<Hpc>(hpc);
		Scheme_Object* obj= objscheme_make_uninited_object(PolComplexSchemeClass);
		OBJSCHEME_SET_CPP_OBJ(obj,(Scheme_Object*)__pointer);
		scheme_add_finalizer(obj, mzscheme_pol_complex_finilize, (void*)NULL);//for deallocation
		return obj;
	}
};


////////////////////////////////////////////////////////////////////////////
static void mzscheme_pol_complex_finilize(void *p, void *data) 
{
	SmartPointer<Hpc>* pc=(SmartPointer<Hpc>*)OBJSCHEME_GET_CPP_OBJ((Scheme_Object*)p);		

	if (pc)
	{
		//in this way I can get benchmarks for destroy too!
		Plasm::dereference(pc);
		delete pc;
	}
}

////////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_build_pol_complex_cube(void* data,int argc, Scheme_Object **argv)
{
	Args p("build_pol_complex_cube",argc,argv,1,1);
	int dim =p.Int();
	SmartPointer<Hpc> ret=(((Plasm*)data))->cube(dim);
	return p.Wrap(ret);
}


////////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_build_pol_complex_simplex (void* data,int argc, Scheme_Object **argv)
{
	Args p("build_pol_complex_simplex",argc,argv,1,1);
	int dim =p.Int();
	SmartPointer<Hpc> ret=(((Plasm*)data))->simplex(dim);
	return p.Wrap(ret);
}


////////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_get_npol_dim (void* data,int argc, Scheme_Object **argv)
{
	Args p("get_npol_dim",argc,argv,1,1);
	SmartPointer<Hpc> src =p.Pol();
	int ret=(((Plasm*)data))->getSpaceDim(src);
	return Args::Wrap(ret);	
}


//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_get_dpol_dim(void* data,int argc, Scheme_Object **argv)
{
	Args p("get_dpol_dim",argc,argv,1,1);
	SmartPointer<Hpc> src=p.Pol();
	int ret=(((Plasm*)data))->getPointDim(src);
	return Args::Wrap(ret);	
}


//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_scale_pol_complex (void* data,int argc, Scheme_Object **argv)
{
	Args p("scale_pol_complex",argc,argv,3,3);
	SmartPointer<Hpc>    src   =p.Pol();
	int             ref   =p.Int()+1;
	float         factor=p.Float();
	SmartPointer<Hpc> ret=(((Plasm*)data))->scale(src,ref,ref,factor);
	return p.Wrap(ret);
}


//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_translate_pol_complex (void* data,int argc, Scheme_Object **argv)
{
	Args p("translate_pol_complex",argc,argv,3,3);
	SmartPointer<Hpc>    src   =p.Pol   ();
	int				ref   =p.Int   ()+1;
	float			factor=p.Float  ();
	SmartPointer<Hpc> ret=(((Plasm*)data))->translate(src,ref,ref,factor);
	return p.Wrap(ret);
}

//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_rotate_pol_complex (void* data,int argc, Scheme_Object **argv)
{
	Args p("rotate_pol_complex",argc,argv,4,4);
	SmartPointer<Hpc> src   =p.Pol   ();
	int          ref1  =p.Int   ()+1;
	int          ref2  =p.Int   ()+1;
	float      factor=p.Float();
	SmartPointer<Hpc> ret=(((Plasm*)data))->rotate(src,max2(ref1,ref2),ref1,ref2,factor);
	return p.Wrap(ret);
}

//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_embed_pol_complex(void* data,int argc, Scheme_Object **argv)
{
	Args p("embed_pol_complex",argc,argv,2,2);
	SmartPointer<Hpc> src=p.Pol();
	int  dim=p.Int();
	SmartPointer<Hpc> ret=(((Plasm*)data))->embed(src,(((Plasm*)data))->getSpaceDim(src)+dim);
	return p.Wrap(ret);
}

//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_struct_pol_complex(void* data,int argc, Scheme_Object **argv)
{
	Args p("struct_pol_complex",argc,argv,1,1);
	Args pstruct=p.getList();
	int n=pstruct.remaining();

	std::vector<SmartPointer<Hpc> > pols;
	for (int i=0;i<n;i++) pols.push_back(pstruct.Pol());
	SmartPointer<Hpc> ret=(((Plasm*)data))->Struct(pols);
	return p.Wrap(ret);
}

//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_join(void* data,int argc, Scheme_Object **argv)
{
	Args p("join",argc,argv,2,2);
	SmartPointer<Hpc> src=p.Pol();
	Args psub=p.getList();
	std::vector<SmartPointer<Hpc> > pols;
	pols.push_back(src);
	while (psub.remaining())  pols.push_back(psub.Pol());
	SmartPointer<Hpc> ret=(((Plasm*)data))->join(pols);
	return p.Wrap(ret);
}


//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_power(void* data,int argc, Scheme_Object **argv)
{
	Args p("power",argc,argv,2,2);
	SmartPointer<Hpc> arg1=p.Pol();
	SmartPointer<Hpc> arg2=p.Pol();
	SmartPointer<Hpc> ret=(((Plasm*)data))->power(arg1,arg2);
	return p.Wrap(ret);
}


//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_skeleton(void* data,int argc, Scheme_Object **argv)
{
	Args p("skeleton",argc,argv,2,2);
	SmartPointer<Hpc> src=p.Pol();
	int dim  =p.Int();
	SmartPointer<Hpc> ret=(((Plasm*)data))->skeleton(src,dim);
	return p.Wrap(ret);
}


//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_pol_complex_mat (void* data,int argc, Scheme_Object **argv)
{
	Args p("pol_complex_mat",argc,argv,2,2);
	SmartPointer<Hpc> src=p.Pol();

	//read matrix
	Args prows=p.getList();

	int nrows=prows.remaining();
	SmartPointer<Matf> mat(new Matf(nrows-1));

	int n=0;
	int ncols=-1;
	while (prows.remaining())
	{
		Args pcols=prows.getList();
		if (ncols==-1) ncols=pcols.remaining();
		if (ncols!=nrows || pcols.remaining()!=ncols)  
			scheme_signal_error("c++::not square matrix in function pol_complex_mat");

		while (pcols.remaining())  (*mat).mem[n++]=(float)pcols.Float();
	}

	SmartPointer<Matf> mat_inv(new Matf(mat->invert()));
	SmartPointer<Hpc> ret=(((Plasm*)data))->transform(src,mat,mat_inv);
	return p.Wrap(ret);
}


//////////////////////////////////////////////////////////////////////
static Scheme_Object *mzscheme_boolop(void* data,int argc, Scheme_Object **argv,Plasm::BoolOpCode operation)
{
	Args p("boolop",argc,argv,2,2);
	SmartPointer<Hpc>   src =p.Pol();
	Args psub=p.getList();
	int n=0;
	std::vector<SmartPointer<Hpc> > pols;
	pols.push_back(src);
	while (psub.remaining()) pols.push_back(psub.Pol());
	SmartPointer<Hpc> ret=(((Plasm*)data))->boolop(operation,pols);
	return p.Wrap(ret);
}

Scheme_Object *mzscheme_or  (void* data,int argc, Scheme_Object **argv)		{return mzscheme_boolop(data,argc,argv,Plasm::BOOL_CODE_OR);}
Scheme_Object *mzscheme_and (void* data,int argc, Scheme_Object **argv)		{return mzscheme_boolop(data,argc,argv,Plasm::BOOL_CODE_AND);}
Scheme_Object *mzscheme_diff(void* data,int argc, Scheme_Object **argv)		{return mzscheme_boolop(data,argc,argv,Plasm::BOOL_CODE_DIFF);}
Scheme_Object *mzscheme_xor (void* data,int argc, Scheme_Object **argv)		{return mzscheme_boolop(data,argc,argv,Plasm::BOOL_CODE_XOR);}


//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_pdiff(void* data,int argc, Scheme_Object **argv)	
{
	Args p("pdiff",argc,argv,2,2);

	SmartPointer<Hpc> src =p.Pol();
	Args psub=p.getList();

	std::vector<SmartPointer<Hpc> > pols;
	pols.push_back(src);
	while (psub.remaining()) pols.push_back(psub.Pol());

	Scheme_Object** ret_list=new Scheme_Object*[(int)pols.size()];

	for (int i=0;i<(int)pols.size();i++)
	{
		std::vector<SmartPointer<Hpc> > args;
		args.push_back(pols[i]);
		for (int j=0;j<i;j++) args.push_back(pols[j]);
		SmartPointer<Hpc> ret=(((Plasm*)data))->boolop(Plasm::BOOL_CODE_DIFF,args);
		ret_list[i]=p.Wrap(ret);
	}

	Scheme_Object* ret=scheme_build_list((int)pols.size(),ret_list);

	delete [] ret_list;
	return ret;
}



//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_make_pol(void* data,int argc, Scheme_Object **argv)
{
	Args p("make_pol",argc,argv,3,3);

	int u;
	Args pverts =p.getList();
	Args pcells =p.getList();
	Args ppols  =p.getList();

	//parse points
	int npoints=pverts.remaining();

	std::vector<float> points;
	int pointdim=-1;
	
	for (u=0;u<npoints;u++)
	{
		Args ppoint=pverts.getList();

		if (pointdim==-1) 
		{
			pointdim=ppoint.remaining();
		}
		else
		{
			if (pointdim!=ppoint.remaining())
				scheme_signal_error("c++::error in make_pol. vertices don't have the same dimensions %V",argv[0]);
		}

		for (int i=0;i<pointdim;i++) points.push_back(ppoint.Float());
	}
	DebugAssert((int)points.size()==(npoints*pointdim));

	//parse hulls
	int nhulls=pcells.remaining();
	std::vector<std::vector<int> >  hulls;

	int midx=-1,Midx=-1;
	while (pcells.remaining())
	{
		Args phull=pcells.getList();

		std::vector<int> hull;

		while (phull.remaining()) 
		{
			int idx=phull.Int();
			midx=(midx==-1)?(idx):(min2(midx,idx));
			Midx=(Midx==-1)?(idx):(max2(Midx,idx));
			hull.push_back(idx);
		}

		hulls.push_back(hull);

	}
	DebugAssert((int)hulls.size()==nhulls);

	if (midx<1 || midx>npoints || Midx<1 || Midx>npoints)
		scheme_signal_error("c++::error in make_pol. hulls reference a point outside the valid range %V",argv[1]);

	//correct, i want to start from 0
	for (u=0;u<nhulls;u++)
	{
		for (int j=0;j<(int)hulls[u].size();j++) 
			--hulls[u][j];
	}

	//parse pols
	int npols=ppols.remaining();

	std::vector<std::vector<int> > pols;
	midx=-1,Midx=-1;
	while (ppols.remaining())
	{
		Args ppol=ppols.getList();

		std::vector<int> pol;

		while (ppol.remaining()) 
		{
			int idx=ppol.Int();
			midx=(midx==-1)?(idx):(min2(midx,idx));
			Midx=(Midx==-1)?(idx):(max2(Midx,idx));
			pol.push_back(idx);
		}

		pols.push_back(pol);
	}
	DebugAssert((int)pols.size()==npols);

	if (midx<1 || midx>nhulls || Midx<1 || Midx>nhulls)
		scheme_signal_error("c++::error in make_pol. Pols reference a hull outside the valid range %V",argv[2]);
	
	//correct indices
	for (u=0;u<npols;u++) 
	{
		for (int j=0;j<(int)pols[u].size();j++)  
			--pols[u][j];
	}

	//simplify hulls not referenced
	bool* bHull=new bool[nhulls];
	memset(bHull,0,sizeof(bool)*nhulls);

	for (u=0;u<(int)pols.size();u++) 
	{
		for (int j=0;j<(int)pols[u].size();j++) 
			bHull[pols[u][j]]=true;
	}

	for (u=0;u<nhulls;u++) 
	{
		if (!bHull[u]) 
			hulls[u]=std::vector<int>();
	}

	SmartPointer<Hpc> ret=(((Plasm*)data))->mkpol(pointdim,points,hulls);


	delete [] bHull;

	return p.Wrap(ret);
}

//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_add_prop_pol_complex (void* data,int argc, Scheme_Object **argv)
{
	Args p("add_prop_pol_complex",argc,argv,2,2);
	SmartPointer<Hpc> src = p.Pol();
	Args subp=p.getList();subp.checkNumArgs(2,2);
	char* pname  = subp.String();

	char s[1024];

	Scheme_Object* pvalue = subp.Obj();

	if (!strcmpi(pname,HPC_PROP_RGB_COLOR))
	{
		Args mylist(0,pvalue);
		float R=mylist.Float();
		float G=mylist.Float();
		float B=mylist.Float();
		sprintf(s,"%f %f %f",R,G,B);
		SmartPointer<Hpc> ret=(((Plasm*)data))->addProperty(src,HPC_PROP_RGB_COLOR,s);
		return p.Wrap(ret);
	}
	
	if (!strcmpi(pname,HPC_PROP_VRML_MATERIAL))
	{
		Args mylist(0,pvalue);

		float Diffuse [4];Args diffuse=mylist.getList();Diffuse[0]=diffuse.Float();Diffuse[1]=diffuse.Float();Diffuse[2]=diffuse.Float();
		float Specular[4];Args specular=mylist.getList();Specular[0]=specular.Float();Specular[1]=specular.Float();Specular[2]=specular.Float();
		float Ambient [4];Ambient[0]=Ambient[1]=Ambient[2]=mylist.Float();
		float Emissive[4];Args emissive=mylist.getList();Emissive[0]=emissive.Float();Emissive[1]=emissive.Float();Emissive[2]=emissive.Float();
		float Shininess=128*mylist.Float(); //map from [0,1] to [0,128]
		float Transparency=mylist.Float();
		Diffuse[3]=Specular[3]=Ambient[3]=Emissive[3]=(1-Transparency);//alpha channel 1==opaque, 0==totally transparent
		sprintf(s,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
			Ambient[0],Ambient[1],Ambient[2],Ambient[3],
			Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3],
			Specular[0],Specular[1],Specular[2],Specular[3],
			Emissive[0],Emissive[1],Emissive[2],Emissive[3],
			Shininess);
		SmartPointer<Hpc> ret=(((Plasm*)data))->addProperty(src,HPC_PROP_VRML_MATERIAL,s);
		return p.Wrap(ret);
	}

	if (!strcmpi(pname,HPC_PROP_VRML_TEXTURE))
	{
		Args mylist(0,pvalue);

		const char* url=mylist.String();
		int repeatS=mylist.Int();
		int repeatT=mylist.Int();

		Args texturecenter=mylist.getList();
		float cx=texturecenter.Float();
		float cy=texturecenter.Float();

		float rot=mylist.Float();

		Args texturescale=mylist.getList();
		float sx=texturescale.Float();
		float sy=texturescale.Float();

		Args texturetranslation=mylist.getList();
		float tx=texturetranslation.Float();
		float ty=texturetranslation.Float();
	
		//set to zero
		SmartPointer<Hpc> ret;

#if 1
		{
			Boxf box=Plasm::limits(src);

			if (box.dim()==2 || box.dim()==3)
			{
				int ref0=box.maxsizeidx();
				int ref1=box.minsizeidx();
				if (box.dim()==3) ref1=(ref0!=1 && ref1!=1)?1:((ref0!=2 && ref1!=2)?2:3); //middle dimension!
				DebugAssert(ref0!=ref1);

				//one of the dimension is not full
				if (box.size()[ref0] && box.size()[ref1])
				{
					Vecf vt(0.0f,
						box.dim()>=1?-box.p1[1]:0.0f,
						box.dim()>=2?-box.p1[2]:0.0f,
						box.dim()>=3?-box.p1[3]:0.0f);

					Vecf vs(0.0f,
						box.dim()>=1 && box.size()[1]?1.0f/(box.size()[1]):1.0f,
						box.dim()>=2 && box.size()[2]?1.0f/(box.size()[2]):1.0f,
						box.dim()>=3 && box.size()[3]?1.0f/(box.size()[3]):1.0f);

					int refm;
						 if (ref0!=1 && ref1!=1) refm=1;
					else if (ref0!=2 && ref1!=2) refm=2;
					else                         refm=3;
					std::vector<int> perm(4);perm[0]=0;perm[ref0]=1;perm[ref1]=2; perm[refm]=3; 
					DebugAssert(ref0!=ref1 && ref1!=refm && ref0!=refm);
					SmartPointer<Matf> project_uv
					(
						new Matf(   
							  Matf::translateV(Vecf(0.0,+cx,+cy,0))
							* Matf::scaleV(Vecf(0.0f,sx,sy,1))
							* Matf::rotateV(3,1,2,-rot) //- because  want this way 
							* Matf::translateV(Vecf(0.0f,-cx,-cy,0))
							* Matf::translateV(Vecf(0.0f,tx,ty,0)) 
							* Matf(3).swapCols(perm) 
							* Matf::scaleV(vs)
							* Matf::translateV(vt)
						)
					);
					ret=Plasm::Skin(src,url,project_uv);
				}
			}
		}

#endif

		if (!ret) ret=Plasm::copy(src);
		return p.Wrap(ret);
	}

	if (!strcmpi(pname,HPC_PROP_VRML_CREASE))
	{
		float angle=Args::Float(pvalue);
		sprintf(s,"%f",angle);
		SmartPointer<Hpc> ret=(((Plasm*)data))->addProperty(src,HPC_PROP_VRML_CREASE,s);
		return p.Wrap(ret);
	}

	if (!strcmpi(pname,HPC_PROP_VRML_CAMERA))
	{
		Args mylist(0,pvalue);

		Args position=mylist.getList();
			float position_X=position.Float();
			float position_Y=position.Float();
			float position_Z=position.Float();

		// this is a quaternion!
		Args orientation=mylist.getList();
			float orientation_X=orientation.Float();
			float orientation_Y=orientation.Float();
			float orientation_Z=orientation.Float();
			float orientation_W=orientation.Float();

		float field_of_view=mylist.Float();
		char* description=mylist.String();
		sprintf(s,"[%s] %f %f %f %f %f %f %f %f ",description,position_X,position_Y,position_Z,orientation_X,orientation_Y,orientation_Z,orientation_W,field_of_view);
		SmartPointer<Hpc> ret=(((Plasm*)data))->addProperty(src,HPC_PROP_VRML_CAMERA,s);
		return p.Wrap(ret);
	}

	if (!strcmpi(pname,HPC_PROP_VRML_LIGHT))
	{
		//todo parsing
		SmartPointer<Hpc> ret=(((Plasm*)data))->addProperty(src,HPC_PROP_VRML_LIGHT,"light");
		return p.Wrap(ret);
	}
	

	scheme_signal_error("c++::unknown property name %s",pname);
	return scheme_void;
}


//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_set_name_pol_complex (void* data,int argc, Scheme_Object **argv)
{
	Args p("set_name_pol_complex",argc,argv,2,2);
	SmartPointer<Hpc> src=p.Pol();
	char* name=p.String();
	(((Plasm*)data))->addProperty(src,HPC_PROP_NAME,name);
	return p.Wrap(src);
}


//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_get_prop_pol_complex (void* data,int argc, Scheme_Object **argv)
{
	Args p("get_prop_pol_complex",argc,argv,2,2);
	SmartPointer<Hpc>  src  = p.Pol();
	std::string pvalue=(((Plasm*)data))->getProperty(src,std::string(p.String()));
	return pvalue.length()?p.Wrap(pvalue.c_str()):(Scheme_Object*)scheme_void;
}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_viewopengl(void* data,int argc, Scheme_Object **argv)
{
	Args p("viewopengl",argc,argv,1,1); //do not count time!
	SmartPointer<Hpc> src=p.Pol();
	(((Plasm*)data))->View(src);
	return scheme_void;
}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_dump_pol_complex(void* data,int argc, Scheme_Object **argv)
{
	Args p("dump_pol_complex",argc,argv,1,1);
	SmartPointer<Hpc> src=p.Pol();
	(((Plasm*)data))->Print(src);
	return scheme_void;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_copy_pol_complex(void* data,int argc, Scheme_Object **argv)
{
	Args p("copy_pol_complex",argc,argv,1,1);
	SmartPointer<Hpc> src=p.Pol();
	SmartPointer<Hpc> ret=(((Plasm*)data))->copy(src);
	return p.Wrap(ret);
}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_hpcdestruct(void* data,int argc, Scheme_Object **argv)
{
	Args p("hpcdestruct",argc,argv,1,1);
	SmartPointer<Hpc> src=p.Pol();
	SmartPointer<Hpc> tmp=(((Plasm*)data))->shrink(src,false); //Clone geometry

	int num=0;
	Scheme_Object** v=new Scheme_Object*[tmp->getNumberOfChilds()];

	for (Hpc::const_iterator it=tmp->childs.begin();it!=tmp->childs.end();it++)
		v[num++]=p.Wrap((((Plasm*)data))->copy(*it));

	Scheme_Object* ret=scheme_build_list(num,v);
	delete [] v;
	return ret;
}

/////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_xml_pol_import (void* data,int argc, Scheme_Object **argv)
{
	Args p("xml_pol_import",argc,argv,1,1);
	char* filename=p.String();
	return p.Wrap((((Plasm*)data))->open(filename));
}


/////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_xml_pol_export (void* data,int argc, Scheme_Object **argv)
{
	Args p("xml_pol_export",argc,argv,2,2);
	SmartPointer<Hpc> src=p.Pol();
	char* filename=p.String();
	(((Plasm*)data))->save(src,filename);
	return scheme_true;
}

/////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_hpc_statistics (void* data,int argc, Scheme_Object **argv)
{
	Args p("hpc_statistics",argc,argv,0,0);
	(((Plasm*)data))->statistics(true);
	return scheme_true;
}

/////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_get_filename(void* data,int argc, Scheme_Object **argv)
{
	Args p("mzscheme_get_filename",argc,argv,1,1);
	std::string Filename=p.String();
	std::string ret=FileSystem::FullPath(Filename);
	return p.Wrap(ret.c_str());
}


//////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_get_limits (void* data,int argc, Scheme_Object **argv)
{
	Args p("get_limits",argc,argv,3,3);

	SmartPointer<Hpc> src=p.Pol();
	Boxf box=(((Plasm*)data))->limits(src);

	Args psub=p.getList();
	std::vector<int> selection;
	while (psub.remaining()) selection.push_back(psub.Int());
	char* symbol=p.String();

	Scheme_Object* vmax[GRAPH_MAX_GEOMETRIC_DIMENSION+1];
	Scheme_Object* vmin[GRAPH_MAX_GEOMETRIC_DIMENSION+1];
	Scheme_Object* vmed[GRAPH_MAX_GEOMETRIC_DIMENSION+1];
	Scheme_Object* vsiz[GRAPH_MAX_GEOMETRIC_DIMENSION+1];

	if (!strcmp(symbol,"max"))	
	{
		for (int i=0;i<(int)selection.size();i++) vmax[i]=Args::Wrap(box.p2[selection[i]]);
		Scheme_Object* aux=scheme_build_list((int)selection.size(),vmax);
		return scheme_build_list(1,&aux);	
	}
	
	else if (!strcmp(symbol,"min"))
	{
		for (int i=0;i<(int)selection.size();i++) vmin[i]=Args::Wrap(box.p1[selection[i]]);
		Scheme_Object* aux=scheme_build_list((int)selection.size(),vmin);
		return scheme_build_list(1,&aux);	
	}
	else if (!strcmp(symbol,"med"))
	{
		for (int i=0;i<(int)selection.size();i++) vmed[i]=Args::Wrap(0.5f*box.p1[selection[i]]+0.5f*box.p2[selection[i]]);
		Scheme_Object* aux=scheme_build_list((int)selection.size(),vmed);
		return scheme_build_list(1,&aux);	
	}
	else if (!strcmp(symbol,"size"))	
	{
		for (int i=0;i<(int)selection.size();i++) vsiz[i]=Args::Wrap(box.p2[selection[i]]-box.p1[selection[i]]);
		Scheme_Object* aux=scheme_build_list((int)selection.size(),vsiz);
		return scheme_build_list(1,&aux);	
	}

	else if (!strcmp(symbol,"min-max"))	
	{
		Scheme_Object* vmax[GRAPH_MAX_GEOMETRIC_DIMENSION+1];
		Scheme_Object* vmin[GRAPH_MAX_GEOMETRIC_DIMENSION+1];
		for (int i=0;i<(int)selection.size();i++) {vmin[i]=Args::Wrap(box.p1[selection[i]]);vmax[i]=Args::Wrap(box.p2[selection[i]]);}
		Scheme_Object* aux[2]={scheme_build_list((int)selection.size(),vmin),scheme_build_list((int)selection.size(),vmax)};
		return scheme_build_list(2,aux);
	}
	
	scheme_wrong_type("get_limits","wrong-symbol",2,argc,argv);
	return scheme_void;
}


//////////////////////////////////////////////////////////////////////
static Scheme_Object *mzscheme_unmake_pol (void* data,int argc, Scheme_Object **argv,bool bCovector)
{
	Args p("unmake_pol",argc,argv,1,1);
	SmartPointer<Hpc> src=p.Pol();

	std::vector<float> points;
	std::vector<std::vector<int> > hulls;
	int dim,npoints;

	if (!bCovector)
	{
		dim=(((Plasm*)data))->ukpol(src,points,hulls);
		npoints=(int)points.size()/dim;
		DebugAssert(npoints*dim==(int)points.size());
	}
	else
	{
		dim=(((Plasm*)data))->ukpolf(src,points,hulls);
		npoints=(int)points.size()/(dim+1);
		DebugAssert(npoints*(dim+1)==(int)points.size());
	}

	int nhulls=(int)hulls.size();

	Scheme_Object **In[3],*Out[3];
	In[0]=new Scheme_Object*[npoints];
	In[1]=new Scheme_Object*[nhulls];
	In[2]=new Scheme_Object*[nhulls];

	//store vertices
	{
		float* V=&points[0];
		Scheme_Object* v[GRAPH_MAX_GEOMETRIC_DIMENSION+1];
		for (int i=0;i<npoints;i++)
		{
			Scheme_Object** pv=v;
			if (bCovector) *pv++=Args::Wrap(-1*(*V++));  //only if per-face get/store the first homogeneous coordinate
			for (int u=0;u<dim;u++) *pv++=Args::Wrap((bCovector?-1:1)*(*V++));
			if (bCovector) Swap(Scheme_Object*,v[0],v[dim]); //store homog coordinate as last
			In[0][i]=scheme_build_list(dim+(bCovector?1:0),v);
		}
		DebugAssert((V-&points[0])==npoints*(dim+(bCovector?1:0)));
	}
	
	//store hulls
	for (int i=0;i<nhulls;i++)
	{
		const std::vector<int>& hull=hulls[i];
		int num=(int)hull.size();
		Scheme_Object** aux=new Scheme_Object*[num];
		for (int j=0;j<num;j++) aux[j]=Args::Wrap(hull[j]+1); //increment by one!
		In[1][i]=scheme_build_list(num,aux);
		delete [] aux;
	}

	//store pols
	for (int i=0;i<nhulls;i++)
	{
		Scheme_Object* aux=Args::Wrap(i+1); 
		In[2][i]=scheme_build_list(1,&aux);
	}

	Out[0] = scheme_build_list(npoints,In[0]);delete [] In[0] ;
	Out[1] = scheme_build_list(nhulls ,In[1]);delete [] In[1];
	Out[2] = scheme_build_list(nhulls ,In[2]);delete [] In[2];
	Scheme_Object *ret=scheme_build_list(3,Out);
	return ret;
}

Scheme_Object *mzscheme_unmake_pol   (void* data,int argc, Scheme_Object **argv) {return mzscheme_unmake_pol(data,argc,argv,false);}
Scheme_Object *mzscheme_unmake_pol_f (void* data,int argc, Scheme_Object **argv) {return mzscheme_unmake_pol(data,argc,argv,true);}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
static inline void mzscheme_apply_map(int& pointdim,std::vector<float>& points,Scheme_Object* fun)
{
	int npoints=(int)points.size()/pointdim;
	DebugAssert(npoints*pointdim==(int)points.size());
	Scheme_Object* scheme_coords[GRAPH_MAX_GEOMETRIC_DIMENSION+1];

	for(int j=0;j<pointdim;j++) 
		scheme_coords[j]=scheme_make_vector(npoints, scheme_make_double(0.0));

	float* V=&points[0];
	for(int i=0;i<npoints;i++) 
	for(int j=0;j<pointdim;j++) 
		SCHEME_VEC_ELS(scheme_coords[j])[i]=Args::Wrap(*(V++)); 
	
	Scheme_Object* scheme_coords_list=scheme_build_list(pointdim,scheme_coords);

	scheme_coords_list=_scheme_apply(fun,1,&scheme_coords_list);

	int new_pointdim=scheme_proper_list_length(scheme_coords_list);

	if (new_pointdim!=pointdim)
	{
		pointdim=new_pointdim;
		points.resize(npoints*pointdim);
		DebugAssert((int)points.size()==npoints*pointdim);
	}

	Scheme_Object* tmp=scheme_coords_list;
	for (int j=0;j<pointdim;j++,tmp=scheme_cdr(tmp)) scheme_coords[j]=scheme_car(tmp);

	for(int i=0;i<npoints ;i++) 
	for(int j=0;j<pointdim;j++) 
	{
		Scheme_Object* tmp=scheme_coords[j];
		points[i*pointdim+j]=(SCHEME_VECTORP(tmp))?(Args::Float(SCHEME_VEC_ELS(tmp)[i])):(Args::Float(tmp)); 
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
/*
special case: operates only in full dimension
and when cells do not have a MAP which overlaps cells (this condition
cannot be verified here, the coder must be sure about it!)
*/
///////////////////////////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_map_pol_complex_full(void* data,int argc, Scheme_Object **argv)
{
	Args p("map_pol_complex_full",argc,argv,2,2);

	SmartPointer<Hpc> src=(((Plasm*)data))->shrink(p.Pol(),true); //clone geometry since I need triangulation
	Scheme_Object* schemefun=p.Function();
	int Dim=src->spacedim;
	SmartPointer<Hpc> ret(new Hpc());

	for (Hpc::const_iterator it=src->childs.begin();it!=src->childs.end();it++)
	{
		SmartPointer<Hpc> child=*it;
		DebugAssert(child->spacedim==Dim && child->g && child->g->getPointDim()==child->pointdim && child->vmat && child->hmat); //safety check
		
		//operate only in full dimension
		if (child->pointdim!=Dim) 
			scheme_signal_error("c++::map_pol_complex_full pointdim of hpc complex <> dimension of the map function");
		
		//remove matrices and transform
		if(child->vmat)
		{
			child->g->transform(child->vmat,child->hmat);
			child->vmat.reset();
			child->hmat.reset();
		}

		//triangulation is needed
		child->g->triangulate(0); 

		//load vertices
		int npoints=child->g->getNCells(0);
		std::vector<float> points(npoints*Dim);
		float* V=&points[0];
		for (GraphListIterator gt=child->g->each(0);!gt.end();gt++,V+=Dim)
			memcpy(V,child->g->getGeometry(*gt)+1,sizeof(float)*Dim);

		int RetDim=Dim;
		mzscheme_apply_map(RetDim,points,schemefun);

		//the returned result is not in full dimension
		if (RetDim!=Dim) 
			{scheme_signal_error("c++::map_pol_complex_full spacedim of hpc complex <> dimension of the map function");}

		DebugAssert(Dim==child->g->getPointDim()); //should not change the dimension since it is a full map

		V=&points[0];
		for (GraphListIterator gt=child->g->each(0);!gt.end();gt++,V+=Dim)
			memcpy(child->g->getGeometry(*gt)+1,V,sizeof(float)*Dim);

		//find planes which have changed
		if (Dim>=2)
		{
			//remove bounding ball infos
			for (GraphListIterator gt=child->g->each(Dim);!gt.end();gt++)
				child->g->releaseGeom(*gt);

			//recalculate plane information
			for (GraphListIterator gt=child->g->each(Dim-1);!gt.end();gt++)
			{
				child->g->releaseGeom(*gt);
				child->g->getFittingPlane(*gt);
				if (child->g->getNUp(*gt)==1) child->g->fixBoundaryFaceOrientation(*gt);
			}
		}

		SmartPointer<Hpc> ret_child(new Hpc(child->g));
		ret->add(ret_child);
	}

	return p.Wrap(ret);
}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
Scheme_Object *mzscheme_map_pol_complex (void* data,int argc, Scheme_Object **argv)
{
	Args p("map_pol_complex",argc,argv,2,2);

	SmartPointer<Hpc> src=(((Plasm*)data))->shrink(p.Pol(),false);//do not clone geometry
	Scheme_Object* schemefun=p.Function();

	SmartPointer<Hpc> ret(new Hpc());

	//iterate
	for (Hpc::const_iterator it=src->childs.begin();it!=src->childs.end();it++)
	{
		SmartPointer<Hpc> child=*it;
		DebugAssert(child->spacedim==src->spacedim && child->g && child->g->getPointDim()==child->pointdim && child->vmat && child->hmat); //safety check

		//works better with some models (es palatino.psm::DomusDwFloor), worst on other (example pisa.psm)
		#if 0
		child->g=child->g->clone();
		child->g->triangulate(0); 
		#endif

		std::vector<float> points;
		std::vector<std::vector<int> > hulls;
		int pointdim=(((Plasm*)data))->ukpol(child,points,hulls);
		int npoints=(int)points.size()/pointdim;

		if (npoints>0 && pointdim>0)
		{
			mzscheme_apply_map(pointdim,points,schemefun);
			if (npoints>0 && pointdim>0) 
				ret->add(((Plasm*)data)->mkpol(pointdim,points,hulls));	
		}
	}

	return p.Wrap(ret);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern "C" Scheme_Object *Plasm_Parser (int argc,Scheme_Object **argv);

static Scheme_Object* mzscheme_eval_code(Scheme_Env* env,const char* code)
{
	mz_jmp_buf * volatile save, fresh;
	save = scheme_current_thread->error_buf;
	scheme_current_thread->error_buf = &fresh;

	Scheme_Object* ret=0;

	if (scheme_setjmp(scheme_error_buf)) 
		;
	else 
		ret=scheme_eval_string_all(code, env,1) ;

	scheme_current_thread->error_buf = save;

	return ret;
}



/////////////////////////////////////////////////////////////
typedef struct 
{
	char* class_name;
	char* ffi_name;
	Scheme_Object* (*fn)(void* ,int argc, Scheme_Object **argv);
} 
MzPlasmFn;


static MzPlasmFn scmfun[]=
{
	//global reports
	{0              , "xml-pol-import"                ,mzscheme_xml_pol_import            },
	{0              , "xml-pol-export"                ,mzscheme_xml_pol_export            },
	{0              , "build-pol-complex-cube"        ,mzscheme_build_pol_complex_cube    },
	{0              , "build-pol-complex-simplex"     ,mzscheme_build_pol_complex_simplex },
	{0              , "struct-pol-complex"            ,mzscheme_struct_pol_complex        },
	{0              , "make-pol"                      ,mzscheme_make_pol                  },
	{0              , "hpc-statistics"                ,mzscheme_hpc_statistics            },
	{0              , "get-filename"                  ,mzscheme_get_filename              },
	//pol complex methods
	{"pol-complex%" , "get-npol-dim"                  ,mzscheme_get_npol_dim              },
	{"pol-complex%" , "get-dpol-dim"                  ,mzscheme_get_dpol_dim              },
	{"pol-complex%" , "power"                         ,mzscheme_power                     },
	{"pol-complex%" , "scale-pol-complex"             ,mzscheme_scale_pol_complex         },
	{"pol-complex%" , "translate-pol-complex"         ,mzscheme_translate_pol_complex     },
	{"pol-complex%" , "rotate-pol-complex"            ,mzscheme_rotate_pol_complex        },
	{"pol-complex%" , "embed-pol-complex"             ,mzscheme_embed_pol_complex         },
	{"pol-complex%" , "skeleton"                      ,mzscheme_skeleton                  },
	{"pol-complex%" , "or"                            ,mzscheme_or                        },
	{"pol-complex%" , "and"                           ,mzscheme_and                       },
	{"pol-complex%" , "diff"                          ,mzscheme_diff                      },
	{"pol-complex%" , "xor"                           ,mzscheme_xor                       },
	{"pol-complex%" , "pdiff"                         ,mzscheme_pdiff                     },
	{"pol-complex%" , "join"                          ,mzscheme_join                      },
	{"pol-complex%" , "get-limits"                    ,mzscheme_get_limits                },
	{"pol-complex%" , "unmake-pol"                    ,mzscheme_unmake_pol                },
	{"pol-complex%" , "unmake-pol-f"                  ,mzscheme_unmake_pol_f              },
	{"pol-complex%" , "map-pol-complex"               ,mzscheme_map_pol_complex           },
	{"pol-complex%" , "map-pol-complex-full"          ,mzscheme_map_pol_complex_full      },
	{"pol-complex%" , "dump-pol-complex"              ,mzscheme_dump_pol_complex          },
	{"pol-complex%" , "pol-complex-mat"               ,mzscheme_pol_complex_mat           },
	{"pol-complex%" , "set-name-pol-complex"          ,mzscheme_set_name_pol_complex      },
	{"pol-complex%" , "copy-pol-complex"              ,mzscheme_copy_pol_complex          },
	{"pol-complex%" , "add-prop-pol-complex"          ,mzscheme_add_prop_pol_complex      },
	{"pol-complex%" , "get-prop-pol-complex"          ,mzscheme_get_prop_pol_complex      },
	{"pol-complex%" , "viewopengl"                    ,mzscheme_viewopengl                },
	{"pol-complex%" , "hpcdestruct"                   ,mzscheme_hpcdestruct               }
};

/////////////////////////////////////////////////////////////
int main(int argn,char** argv)
{
	#if defined(PLATFORM_Darwin)
	// required for threads safety
	int dummy;
	scheme_set_stack_base(&dummy, 1); 
	#endif

	//important, for case insensitive symbols
	scheme_set_case_sensitive(0);

	printf("MzPlasm initializing...\n");
	Scheme_Env *env = scheme_basic_env();
	fflush(stdout);

	Scheme_Config *config = scheme_current_config();
	Scheme_Object* output_port=scheme_get_param(config,MZCONFIG_OUTPUT_PORT);
	Scheme_Object* error_port =scheme_get_param(config,MZCONFIG_ERROR_PORT );

 
	#if !defined(_DEBUG)
	if (scheme_setjmp(scheme_error_buf)) 
	{	
		scheme_signal_error("plm::an error occurred in MzPlasm. \n");
		return 0;
	}
	#endif
	 
	Scheme_Object *curout = scheme_get_param(config, MZCONFIG_OUTPUT_PORT);
	scheme_eval_string("(display \"Starting MzPlasm X. Please wait....\n\")",env);
	char cmd[1024];
	sprintf(cmd,"(current-library-collection-paths (list (build-path \"%s\")))",FileSystem::FullPath(":mzplasm/mz-360/collects").c_str());
	scheme_eval_string(cmd, env);
	objscheme_init();
	scheme_Plasm_initialize(env);//parser
	scheme_register_extension_global(&PolComplexSchemeClass,sizeof(PolComplexSchemeClass));

	//count number of functions for pol comples class
	int num_methos_pol_complex=0;
	int tot_functions=sizeof(scmfun)/sizeof(MzPlasmFn);
	for (int i=0;i<tot_functions;i++)
	{
		if (scmfun[i].class_name && !strcmpi(scmfun[i].class_name,"pol-complex%")) 
			++num_methos_pol_complex;
	}
	PolComplexSchemeClass = objscheme_make_class("pol-complex%" ,NULL,NULL,num_methos_pol_complex);

	//add scheme primitives
	for (int i=0;i<tot_functions;i++)
	{
		MzPlasmFn* p=&scmfun[i];
		char* class_name=p->class_name;
		char* ffi_name  =p->ffi_name;
		Scheme_Object* (*fn)(void* ffi,int argc, Scheme_Object **argv)=p->fn;
		if (!class_name)
			scheme_add_global(ffi_name,scheme_make_closed_prim_w_arity(fn,NULL,ffi_name,0,-1),env);

		else if (!strcmp(class_name,"pol-complex%")) 
			objscheme_add_closed_method_w_arity(PolComplexSchemeClass,NULL,ffi_name,fn,0,-1);
	}

	scheme_add_global("pol-complex-primitive-class",PolComplexSchemeClass,env);
	objscheme_add_procedures(env);

	//set  lib dir (very important!)
	char _cwd[2048];
	getcwd(_cwd,sizeof(_cwd));
	std::string cwd=std::string(_cwd);
	
	for (int i=0;i<(int)cwd.size();i++)
		if (cwd[i]=='\\') cwd[i]='/';

	std::string PLASMLIBDIR=FileSystem::FullPath(":mzplasm/psm");
	sprintf(cmd,"(define *PLASMLIBSDIR* \"%s\")",PLASMLIBDIR.c_str());
	printf("%s\n",cmd);fflush(stdout);
	scheme_eval_string(cmd,env); 

	std::string init_scm=FileSystem::FullPath(":mzplasm/scm/init.scm");
	sprintf(cmd,"(time (load (build-path \"%s\")))",init_scm.c_str());
	printf("%s\n",cmd);fflush(stdout);
	scheme_eval_string(cmd, env);
	scheme_eval_string("(welcome (current-output-port))",env);
	scheme_eval_string("(read-eval-print-loop)",env);
	return 0;
}





#include <xge/xge.h>
#include <xge/unwrapper.h>



/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
static Triangle2i triangleProject(int texturedim,float factor,const Vec3f& v0,const Vec3f& v1,const Vec3f& v2)
{
	Vec3f verts[3]={v0,v1,v2};

	//establish order
	double v01=(verts[1]-verts[0]).module();
	double v12=(verts[2]-verts[1]).module();
	double v20=(verts[0]-verts[2]).module();

	//not valid,invalid triangle!
	if (!v01 || !v12 || !v20)
		return Triangle2i(); //fill with all zeros

	int idx[3];
		 if (v01>=v12 && v01>=v20) {if (v12>=v20) {idx[0]=0;idx[1]=1;idx[2]=2;} else {idx[0]=1;idx[1]=0;idx[2]=2;}}
	else if (v12>=v01 && v12>=v20) {if (v01>=v20) {idx[0]=2;idx[1]=1;idx[2]=0;} else {idx[0]=1;idx[1]=2;idx[2]=0;}}
	else if (v20>=v01 && v20>=v12) {if (v01>=v12) {idx[0]=2;idx[1]=0;idx[2]=1;} else {idx[0]=0;idx[1]=2;idx[2]=1;}}
	else                           {DebugAssert(0);}

	Vec3f vdiff[3]=
	{	
		(verts[idx[1]]-verts[idx[0]]),
		(verts[idx[2]]-verts[idx[1]]),
		(verts[idx[0]]-verts[idx[2]])
	};

	double len[3]={vdiff[0].module(),vdiff[1].module(),vdiff[2].module()};

	//safety check
	DebugAssert(len[0]>=0 && len[1]>=0 && len[2]>=0 && len[0]>=len[1] && len[1]>=len[2]);

	int width=0,middle=0,height=0;
	for (float actual_factor=factor;actual_factor;actual_factor*=0.9f)
	{
		width =(int)(actual_factor*len[0]);
		middle=(int)(actual_factor*fabs(vdiff[0]*vdiff[2]) / len[0]);
		height=(int)(actual_factor*(vdiff[0].cross(vdiff[2]).module()) / len[0]);

		if (width<=0.9f*texturedim && height<=0.9f*texturedim) 
			break;
	}

	//correct (it has to have minimum dimension)
	if (width <2     ) width =2; 
	if (height<2     ) height=2; 
	if (middle==0    ) middle=1; 
	if (middle==width) middle--; 

	//safety check
	DebugAssert(width>=2 && height>=2 && middle>0 && middle<width);

	Triangle2i ret;
	ret.getRefPoint(idx[0]).x=0;
	ret.getRefPoint(idx[0]).y=0;

	ret.getRefPoint(idx[1]).x=width;
	ret.getRefPoint(idx[1]).y=0;

	ret.getRefPoint(idx[2]).x=middle;
	ret.getRefPoint(idx[2]).y=height;

	return ret;
}



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
static bool triangleRender(const Triangle2i& triangle,SmartPointer<Texture> _rgb,SmartPointer<Texture> _alpha,Color4f color,const Box2i& box, bool bWrite)
{
	DebugAssert(_rgb->bpp==24 && _alpha->bpp==8 && _rgb->width==_alpha->width && _rgb->height==_alpha->height);

	int x1=triangle.p0.x,y1=triangle.p0.y;
	int x2=triangle.p1.x,y2=triangle.p1.y;
	int x3=triangle.p2.x,y3=triangle.p2.y;

	unsigned char* rgb     =_rgb  ->buffer;
	unsigned char* alpha   =_alpha->buffer;
	int            W       =_rgb  ->width;

	int minx = min3(x1, x2, x3) , maxx = max3(x1, x2, x3);
	int miny = min3(y1, y2, y3) , maxy = max3(y1, y2, y3);

	//must be completely be contained in the current area
	if (!(minx>=box.left() && maxx<=box.right() && miny>=box.bottom() && maxy<=box.top()))
	{
		DebugAssert(!bWrite);
		return false;
	}

	int Sign=(((x2-x1)*(y3-y1)-(x3-x1)*(y2-y1))>=0)?(+1):(-1);

	for(int y = miny; y <= maxy; y++)
	for(int x = minx; x <= maxx; x++)
	{
		// if the pixel is inside....
		if
		(
			(Sign*((x2 - x1) * (y - y1) - (y2 - y1) * (x - x1)) >= 0) &&
			(Sign*((x3 - x2) * (y - y2) - (y3 - y2) * (x - x2)) >= 0) &&
			(Sign*((x1 - x3) * (y - y3) - (y1 - y3) * (x - x3)) >= 0)
		)
		{
			int idx=y*W+x;

			//test if it has already been written
			//note: for each pixel consider also the pixel (-1,0) (+1,0) (0,-1) (0,+1)
			if (!bWrite && (alpha[idx] || alpha[idx-1] || alpha[idx+1] ||alpha[idx-W] || alpha[idx+W]))
				return false;
			
			//if write....
			if (bWrite)
			{
				unsigned char R=(unsigned char)(255.0f*color.r);
				unsigned char G=(unsigned char)(255.0f*color.g);
				unsigned char B=(unsigned char)(255.0f*color.b);

				rgb[idx*3+0] = R;
				rgb[idx*3+1] = G;
				rgb[idx*3+2] = B;

				rgb[(idx-1)*3+0] = rgb[(idx+1)*3+0] = rgb[(idx-W)*3+0] = rgb[(idx+W)*3+0] =R ;
				rgb[(idx-1)*3+1] = rgb[(idx+1)*3+1] = rgb[(idx-W)*3+1] = rgb[(idx+W)*3+1] =G ;
				rgb[(idx-1)*3+2] = rgb[(idx+1)*3+2] = rgb[(idx-W)*3+2]  =rgb[(idx+W)*3+2] =B ;

				//sign as drawn
				alpha [idx  ] = 0x01;
				alpha [idx-1] = 0x01;
				alpha [idx+1] = 0x01;
				alpha [idx-W] = 0x01;
				alpha [idx+W] = 0x01;
			}
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool triangleFits(Triangle2i& triangle,SmartPointer<Texture> rgb,SmartPointer<Texture> alpha,std::set<Box2i>& queue,Color4f color)
{
	DebugAssert(rgb->width==rgb->height);

	int texturedim=rgb->width;

	int w=triangle.right(); //width of the triangle (after the projection)
	int h=triangle.top();   //height of the triangle (after the projection)

	Box2i allowed_area(Vec2i(1,1),Vec2i(texturedim-2,texturedim-2),0);


	//try to fitTriangle2i
	for (std::set<Box2i>::iterator jt=queue.begin();jt!=queue.end() ;jt++)
	{
		Box2i box=*jt;



		Triangle2i candidates[4]=
		{
			triangle.scale(+1,+1).translate(  box.left(),  box.bottom()), //no mirroring
			triangle.scale(-1,+1).translate(w+box.left(),  box.bottom()), //mirror x
			triangle.scale(+1,-1).translate(  box.left(),h+box.bottom()), //mirror y
			triangle.scale(-1,-1).translate(w+box.left(),h+box.bottom())  //mirror x And mirrory
		};

		if (box.align)
		{
			//must start from the top of the area
			candidates[0]=candidates[0].translate(0,box.top()-candidates[0].top());
			candidates[1]=candidates[1].translate(0,box.top()-candidates[1].top());
			candidates[2]=candidates[2].translate(0,box.top()-candidates[2].top());
			candidates[3]=candidates[3].translate(0,box.top()-candidates[3].top());
		}

		while (candidates[0].right()<box.right())
		{
			//safety check
			DebugAssert(
				   candidates[0].right()==candidates[1].right() 
				&& candidates[1].right()==candidates[2].right() 
				&& candidates[2].right()==candidates[3].right());

			for (int n=0;n<4;n++)
			{
				//first try to triangleRender
				if (triangleRender(candidates[n],rgb,alpha,color,box,false)) 
				{
					//store finally in the texture
					triangleRender(candidates[n],rgb,alpha,color,box,true);
					
					//these are the 2 other areas to find for fitting
					if (box.align)
					{
						Box2i box_right(Vec2i(candidates[n].centerx(),candidates[n].bottom()),Vec2i(box.right(),box.top()             ),1);
						Box2i box_down (Vec2i(box.left(),box.bottom())                       ,Vec2i(box.right(),candidates[n].bottom()-3),1);

						if (box_right.isValid() && allowed_area.contains(box_right)) queue.insert(box_right);
						if (box_down .isValid() && allowed_area.contains(box_down )) queue.insert(box_down );
					}
					else
					{
						Box2i box_right(Vec2i(candidates[n].centerx(),box.bottom() ) , Vec2i(box.right(),candidates[n].top()),0);
						Box2i box_up   (Vec2i(box.left(),candidates[n].top()+3   ) , Vec2i(box.right(),box.top()          ),0);

						if (box_right.isValid() && allowed_area.contains(box_right)) queue.insert(box_right);
						if (box_up   .isValid() && allowed_area.contains(box_up   )) queue.insert(box_up   );

						//special case when I do not want to waste space for the first triangle in row
						if (box.left()==1)
						{
							Box2i box_left (Vec2i(box.left(),box.bottom()),Vec2i(candidates[n].centerx(),candidates[n].top()),1);

							if (box_left.isValid() && allowed_area.contains(box_left)) queue.insert(box_left);
						}
					}

					queue.erase(jt);

					//modify the triangle and return ok
					triangle=candidates[n];
					return true;
				}
				
				//try a little more to the right (1 pixel)
				
				candidates[n]=candidates[n].translate(1,0);
			}
		}	
	} 

	//cannot fitTriangle2i in the texture, return false (the triangle has not been modified)
	return false;

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class TriangleSort
{
  float* buffer;

public:
  
	//constructor
	inline TriangleSort(SmartPointer<Vector > texture1coords)
		{this->buffer=texture1coords->mem();}

	//comparison
	inline bool operator()(int A, int B) const
	{
		float* ta0=buffer+A*6;
		float* ta1=ta0+2;
		float* ta2=ta0+4;

		float* tb0=buffer+B*6;
		float* tb1=tb0+2;
		float* tb2=tb0+4;

		//measure height
		int h0=(int)max3(ta0[1],ta1[1],ta2[1]);
		int h1=(int)max3(tb0[1],tb1[1],tb2[1]);
		return (h0>h1) || (h0==h1 && A<B);
	}
};


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
Unwrapper::Unwrapper()
{
	this->texturedim=1024;
	this->texture_template=":texture1.%02d.tif";
	this->factor=1;
	this->ntriangles_done=0;
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
std::vector< SmartPointer<Batch> > Unwrapper::Unwrap(Mat4f _T1,SmartPointer<Batch> source_batch)
{
	std::vector< SmartPointer<Batch> > ret;

	//return the batch I cannot handle untouched
	if (source_batch->primitive!=Batch::TRIANGLES || !source_batch->vertices || !source_batch->vertices->size())
	{
		ret.push_back(source_batch);
		return ret;
	}

	//second transformation matrix
	Mat4f Matrix= _T1 * source_batch->matrix;

	//create a perfect copy and leave the source batch untouched
	SmartPointer<Batch> batch(new Batch(*source_batch));

	//overwrite texture1 coordinates
	int ntriangles=batch->vertices->size()/9;
	batch->texture1coords.reset(new Vector(ntriangles*6));
	batch->texture1.reset();

	float* vertices   = batch -> vertices    -> mem();
	float* lightcoord = batch -> texture1coords -> mem();



	//*****************************************************************************
	// step 1: triangleProject and rotate GL_TRIANGLES so the longest edge is horizontal and the top vertex is above the horizontal line
	//******************************************************************************

	//LOG("projecting %d triangles\n",ntriangles);

	for (int T=0;T<ntriangles;T++,vertices+=9)
	{			
		//apply transformation matrix
		Vec3f v0 = Matrix * Vec3f(vertices+0);
		Vec3f v1 = Matrix * Vec3f(vertices+3);
		Vec3f v2 = Matrix * Vec3f(vertices+6);

		Triangle2i triangle=triangleProject(texturedim,factor,v0,v1,v2); //NOTE: for now on I have integer coordinates stored!

		(*lightcoord++)=triangle.p0.x;
		(*lightcoord++)=triangle.p0.y;
		(*lightcoord++)=triangle.p1.x;
		(*lightcoord++)=triangle.p1.y;
		(*lightcoord++)=triangle.p2.x;
		(*lightcoord++)=triangle.p2.y;
	}




	//******************************************************************************
	// step 2: sort the triangle by decreasing height (this is for good placement of GL_TRIANGLES)
	//******************************************************************************

	std::vector<int> order(ntriangles);

	for (int T=0;T<ntriangles;T++) order[T]=T;
		std::sort(order.begin(),order.end(),TriangleSort(batch->texture1coords));

	//******************************************************************************
	// step3: pack continous segment
	//******************************************************************************

	std::vector<int>  cur_indices;
	std::vector<int>  prv_indices;

	lightcoord = batch -> texture1coords -> mem();


	for (int Torder=0;Torder<ntriangles;Torder++)
	{			
		int  T=order[Torder];

		// get the integer texture coordinates as Vec2i
		Triangle2i triangle
		(
			Vec2i(lightcoord[T*6+0],lightcoord[T*6+1]),
			Vec2i(lightcoord[T*6+2],lightcoord[T*6+3]),
			Vec2i(lightcoord[T*6+4],lightcoord[T*6+5])
		);

		//not valid!= since the projection failed
		if (!triangle.p0.x && !triangle.p0.y && !triangle.p1.x && !triangle.p1.y && !triangle.p2.x && !triangle.p2.y)
			continue;

		//give a random color
		Color4f color=Color4f::randomRGB();

		while (true)
		{
			//try with the current texture
			if (cur.rgb && triangleFits(triangle,cur.rgb,cur.alpha,cur.queue,color))	
			{
				lightcoord[T*6+0]=(float)triangle.p0.x;lightcoord[T*6+1]=(float)triangle.p0.y;
				lightcoord[T*6+2]=(float)triangle.p1.x;lightcoord[T*6+3]=(float)triangle.p1.y;
				lightcoord[T*6+4]=(float)triangle.p2.x;lightcoord[T*6+5]=(float)triangle.p2.y;

				cur_indices.push_back(T*3+0); //here I put indices of vertices (each triangle has 3 vertices)
				cur_indices.push_back(T*3+1);
				cur_indices.push_back(T*3+2);
				break;
			}

			//try to fitTriangle2i with the previous texture (seems to be better to do here!)
			if (prv.rgb && triangleFits(triangle,prv.rgb,prv.alpha,prv.queue,color))
			{
				lightcoord[T*6+0]=(float)triangle.p0.x;lightcoord[T*6+1]=(float)triangle.p0.y;
				lightcoord[T*6+2]=(float)triangle.p1.x;lightcoord[T*6+3]=(float)triangle.p1.y;
				lightcoord[T*6+4]=(float)triangle.p2.x;lightcoord[T*6+5]=(float)triangle.p2.y;

				prv_indices.push_back(T*3+0);
				prv_indices.push_back(T*3+1);
				prv_indices.push_back(T*3+2);
				break;
			}

			//cannot fitTriangle2i in prev or current. New texture needed, drop previous one
			if (prv_indices.size()) 
			{
				ret.push_back(batch->getTriangles(prv_indices));
				ret[ret.size()-1]->texture1=prv.rgb;
			}
			
			//the previous becames the current one
			prv=cur;
			prv_indices=cur_indices;

			//allocate new texture
			
			cur.rgb  .reset(new Texture(texturedim,texturedim,24,0));
			cur.alpha.reset(new Texture(texturedim,texturedim, 8,0));
			cur.rgb->filename="";

			if (texture_template.length())
				cur.rgb->filename=Utils::Format(texture_template.c_str(),(int)new_light_textures.size()).c_str();

			new_light_textures.push_back(cur.rgb);

			//LOG("New texture %d\n",(int)new_light_textures.size());
			
			cur.queue.clear();
			cur.queue.insert(Box2i(Vec2i(1,1),Vec2i(texturedim-2,texturedim-2),0)); //this is the working area
			cur_indices.clear();

		} //while true

	} //for each triangle
	
	//******************************************************************************
	// step4: flush residual
	//******************************************************************************

	if (prv_indices.size()) 
	{
		ret.push_back(batch->getTriangles(prv_indices));
		ret[ret.size()-1]->texture1=prv.rgb;
	}
	
	if (cur_indices .size()) 
	{
		ret.push_back(batch->getTriangles(cur_indices));
		ret[ret.size()-1]->texture1=cur.rgb;
	}

	//******************************************************************************
	//convert back to integer coordinates
	//******************************************************************************

	for (std::vector<SmartPointer<Batch> >::iterator ct=ret.begin();ct!=ret.end();ct++)
	{
		DebugAssert((*ct)->primitive==Batch::TRIANGLES);

		float* t= (*ct)->texture1coords->mem();

		for (int i=0;i<(int)(*ct)->texture1coords->size();i++)
		{
			int ival=(int)(*t);
			float fval=(float)(0.5f/texturedim+ival/(float)texturedim); //robust conversion from int to float 
			*t++=fval;
		}
	}

	ntriangles_done+=ntriangles;

	//#ifdef _DEBUG
	//LOG("RibBake:: GL_TRIANGLES done %2d (ntextures %d)\n",ntriangles_done,(int)this->new_light_textures.size());
	//#endif 
	return ret;
}



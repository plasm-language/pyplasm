
#include <xge/xge.h>
#include <xge/Manipulator.h>


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
static Mat4f getTransformationToBox(Box3f box)
{
	// T_to_box to transform the unit circle in the bounding box
    Vec3f mid=box.center();
    Vec3f size=box.size();


	float max_size=box.maxsize();
	size=Vec3f(max_size,max_size,max_size);

    float cos_pi_4= cos((float)M_PI/4.0);

	float _mat[16]={
	   size.x?(0.5*size.x/cos_pi_4):1.0f , 0                                , 0                               , mid.x,  
	   0                                 , size.y?(0.5*size.y/cos_pi_4):1.0f, 0                               , mid.y, 
	   0                                 , 0                                , size.z?0.5*size.z/cos_pi_4:1.0f , mid.z, 
	   0                                 , 0                                , 0                               , 1    
	};

    Mat4f T_to_box=Mat4f(_mat);
	return T_to_box;
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
Manipulator::Manipulator(float epsilon)
{

	this->EPSILON=epsilon;
	this->ref=INVALID;
	this->bRunning=false;
	this->operation=ROTATE;

	this->freeze_x=false;
	this->freeze_y=false;
	this->freeze_z=false;

	circle_x=Batch::Circle((float)M_PI/32.0f);
	circle_y=Batch::Circle((float)M_PI/32.0f);
	circle_z=Batch::Circle((float)M_PI/32.0f);

	float _linex[]={-1,0,0,1,0,0};
	line_x.reset(new Batch);
	line_x->primitive=Batch::LINES;
	line_x->vertices.reset(new Vector(6,_linex));

	float _liney[]={0,-1,0,0,1,0};
	line_y.reset(new Batch);
	line_y->primitive=Batch::LINES;
	line_y->vertices.reset(new Vector(6,_liney));

	float _linez[]={0,0,-1,0,0,1};
	line_z.reset(new Batch);
	line_z->primitive=Batch::LINES;
	line_z->vertices.reset(new Vector(6,_linez));

}



//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void Manipulator::setOperation(Operation operation)
{
	this->operation=operation;
	this->ref=INVALID;
	this->bRunning=false;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void Manipulator::setObject(Box3f box,Mat4f* T)
{
	this->T=T;
    this->box=box;
	this->ref=INVALID;
	this->bRunning=false;

	if (box.isValid())
		this->T_to_box=getTransformationToBox(box);
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
bool Manipulator::canManipulate(Ray3f ray,Box3f box,Mat4f* T)
{
	//nothing to do
	if (!box.isValid())
		return false;

	Vec3f size=box.size();

	Mat4f Direct=(*T) * getTransformationToBox(box);
    Mat4f Inverse=Direct.invert();

    // the ray is in world coordinate
    Vec3f P1=Inverse * (ray.origin        );
    Vec3f P2=Inverse * (ray.origin+ray.dir);
    
    // should be the unit bounding ball not the bounding box, but seems good enough (probably better)
    // is objects does not overlap too much!
	float epsilon=1e-4f;
	Box3f unit_box(
		Vec3f(
			size[0]?-1:-epsilon,
			size[1]?-1:-epsilon,
			size[2]?-1:-epsilon),
		Vec3f(
			size[0]?+1:+epsilon,
			size[1]?+1:+epsilon,
			size[2]?+1:+epsilon));


	float tmin,tmax;
    return (Ray3f(P1,P2-P1).intersectBox(tmin,tmax,unit_box) && tmin>0);
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void Manipulator::Mouse(MouseEvent args,Ray3f ray)
{           
	if (!box.isValid() || !T)
		return;

	if (!ray.origin.isValid() || !ray.dir.isValid())
		return;

	//request to find a new coordinate axis
	bool bFindNewRef= (args.type==MouseEvent::MousePressed) || (args.type==MouseEvent::MouseMoved && !this->bRunning);
    
    // change local coordinate system
    if (bFindNewRef)
	{
        // assign new reference (start with invalid one)
        this->ref=INVALID;
        
        // go to the local coordinate planes
        this->Direct=(*T) * this->T_to_box;
        this->Inverse=this->Direct.invert();

        // local coordinate system!
        this->X=Vec3f(Direct.a11(),Direct.a21(),Direct.a31()).normalize();
        this->Y=Vec3f(Direct.a12(),Direct.a22(),Direct.a32()).normalize();
        this->Z=Vec3f(Direct.a13(),Direct.a23(),Direct.a33()).normalize();
        this->C=Vec3f(Direct.a14(),Direct.a24(),Direct.a34());
	}


    // find the intersection in the local coordinate axes
	//NOTE: the can be invalid, test before using
	Vec3f px=Inverse * ray.intersection(Plane4f(this->X,this->X*this->C));
	Vec3f py=Inverse * ray.intersection(Plane4f(this->Y,this->Y*this->C));
	Vec3f pz=Inverse * ray.intersection(Plane4f(this->Z,this->Z*this->C));

	bool px_good=px.isValid();
	bool py_good=py.isValid();
	bool pz_good=pz.isValid();

    // rotate operation
    if (this->operation==ROTATE)
	{
        // find the nearest ellipse to the current mouse cur_position  (x*x+y*y)=R*R 
        if (bFindNewRef)
		{
			float mx=px_good?px.module():1e18f;
			float my=py_good?py.module():1e18f;
			float mz=pz_good?pz.module():1e18f;
			bool bx=(!freeze_y && !freeze_z) && mx>(1.0-EPSILON) && mx<(1.0+EPSILON);float dx=bx?(px-ray.origin).module():1e18f;
			bool by=(!freeze_x && !freeze_z) && my>(1.0-EPSILON) && my<(1.0+EPSILON);float dy=by?(py-ray.origin).module():1e18f;
			bool bz=(!freeze_x && !freeze_y) && mz>(1.0-EPSILON) && mz<(1.0+EPSILON);float dz=bz?(pz-ray.origin).module():1e18f;
            if (bx && dx<=dy && dx<=dz) this->ref=REF_X;
            if (by && dy<=dx && dy<=dz) this->ref=REF_Y;
            if (bz && dz<=dx && dz<=dy) this->ref=REF_Z;
		}

        // good reference, store the first cur_angle
        if (this->ref!=INVALID)
		{
			
			float new_angle;

                 if (this->ref==REF_X) new_angle=atan2(px.z,px.y) ;
            else if (this->ref==REF_Y) new_angle=atan2(py.x,py.z);
            else                       new_angle=atan2(pz.y,pz.x);
                
            // apply transformation 
            if (this->bRunning)
			{
				Mat4f Ta=Mat4f::translate( this->C.x, this->C.y, this->C.z);
				Mat4f Tb=Mat4f::translate(-this->C.x,-this->C.y,-this->C.z);

                float  delta=(new_angle-this->cur_angle); 

				Mat4f Rot;
				     if (ref==REF_X) Rot=Mat4f::rotate(X.normalize(),delta);
				else if (ref==REF_Y) Rot=Mat4f::rotate(Y.normalize(),delta);
				else                 Rot=Mat4f::rotate(Z.normalize(),delta);

				
                (*T) = (Ta*Rot*Tb) * (*T);
			}
                 
            this->cur_angle=new_angle;
		}
	}
    else if (this->operation==TRANSLATE || this->operation==SCALE)
	{
        // find the nearest line
        if (bFindNewRef)
		{
			//X 
			float dx=1e18f;int sign_x;
			if (!freeze_x && py_good && py.x>=-1 && py.x<=+1 && fabs(py.z)<dx) {dx=fabs(py.z);sign_x=(py.x>=0)?+1:-1;} //Y
			if (!freeze_x && pz_good && pz.x>=-1 && pz.x<=+1 && fabs(pz.y)<dx) {dx=fabs(pz.y);sign_x=(pz.x>=0)?+1:-1;} //Z

			//Y
			float dy=1e18f;int sign_y;
			if (!freeze_y && px_good && px.y>=-1 && px.y<=+1 && fabs(px.z)<dy) {dy=fabs(px.z);sign_y=(px.y>=0)?+1:-1;} //X
			if (!freeze_y && pz_good && pz.y>=-1 && pz.y<=+1 && fabs(pz.x)<dy) {dy=fabs(pz.x);sign_y=(pz.y>=0)?+1:-1;} //Z

			//Z
			float dz=1e18f;int sign_z;
            if (!freeze_z && px_good && px.z>=-1 && px.z<=+1 && fabs(px.y)<dz) {dz=fabs(px.y);sign_z=(px.z>=0)?+1:-1;} //X
            if (!freeze_z && py_good && py.z>=-1 && py.z<=+1 && fabs(py.x)<dz) {dz=fabs(py.x);sign_z=(py.z>=0)?+1:-1;} //Y
            
            // find the axis which is nearest
			if (dx<EPSILON && dx<=dy && dx<=dz) {this->ref=REF_X;this->cur_sign=sign_x;}
            if (dy<EPSILON && dy<=dx && dy<=dz) {this->ref=REF_Y;this->cur_sign=sign_y;}
			if (dz<EPSILON && dz<=dx && dz<=dy) {this->ref=REF_Z;this->cur_sign=sign_z;}

			//LOG("Ref %d%s\n",this->cur_sign,this->ref==REF_X?"x":(this->ref==REF_Y?"y":(this->ref==REF_Z?"z":"")));
		}
                
        // good reference
        if (this->ref!=INVALID)
		{
			Vec3f P=(ray.origin+ray.dir*ray.origin.distance(this->C));
			if (this->operation==SCALE) P=Inverse * P;
				
			float new_position;
			if (this->ref==REF_X) new_position=(this->operation==SCALE)?P.x:X*P;
			else if (this->ref==REF_Y) new_position=(this->operation==SCALE)?P.y:Y*P;
			else                       new_position=(this->operation==SCALE)?P.z:Z*P;

            // check not too close to the origin (need by scaling which divide by this->cur_position)
            if (bFindNewRef && this->operation==SCALE && fabs(new_position)<0.1f)
			{
                this->ref=INVALID;
			}
			else
			{
                // apply transformation
                if (this->bRunning) 
				{
                    if (this->operation==TRANSLATE)
					{
                        Vec3f Translate;
						
						     if (this->ref==REF_X) Translate=this->X*(new_position-this->cur_position);
						else if (this->ref==REF_Y) Translate=this->Y*(new_position-this->cur_position);
						else                       Translate=this->Z*(new_position-this->cur_position);

						
						(*T)= Mat4f::translate(Translate.x,Translate.y,Translate.z) * (*T);
					}
					//scale
                    else
					{
                        Vec3f vs;
						Vec3f vt;
						float f=new_position/this->cur_position;
                        
						if (Keyboard::isShiftPressed()) 
						{
							vs=Vec3f(f,f,f);
							vt=this->box.center();
						}
						else if (Keyboard::isControlPressed())
						{
							vs=Vec3f(ref==REF_X?f:1 , ref==REF_Y?f:1 , ref==REF_Z?f:1);
							vt=this->box.center();
						}
						else
						{
							vs=Vec3f(ref==REF_X?f:1 , ref==REF_Y?f:1 , ref==REF_Z?f:1);
							vt=this->box.center();
							vt[this->ref]=cur_sign>0?box.p1[this->ref]:box.p2[this->ref];
						}


						(*T) = (*T) * Mat4f::translate(vt) * Mat4f::scale(vs) * Mat4f::translate(-1*vt);
					}
				}
                this->cur_position=new_position;
			}
		}
	}

    // from now on...
	this->bRunning= (args.type==MouseEvent::MouseMoved && this->bRunning) || (args.type==MouseEvent::MousePressed && this->ref!=INVALID);
} 


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void Manipulator::Render(Engine* engine)
{     

	Mat4f global_transformation=(*T) * this->T_to_box;

	circle_x->matrix=global_transformation * Mat4f::rotatey((float)M_PI/2);
	circle_y->matrix=global_transformation * Mat4f::rotatex((float)M_PI/2);
	circle_z->matrix=global_transformation;

	line_x->matrix=global_transformation;
	line_y->matrix=global_transformation;
	line_z->matrix=global_transformation;

	engine->SetPolygonMode(Batch::LINES);
	engine->SetLineWidth(3);


    
    // rotate
    if (this->operation==ROTATE)
	{
		if (!freeze_y && !freeze_z) {if (this->ref==REF_X) circle_x->setColor(Color4f::Yellow()); else circle_x->setColor(Color4f::Red  ());engine->Render(circle_x);}
		if (!freeze_x && !freeze_z) {if (this->ref==REF_Y) circle_y->setColor(Color4f::Yellow()); else circle_y->setColor(Color4f::Green());engine->Render(circle_y);}
		if (!freeze_x && !freeze_y) {if (this->ref==REF_Z) circle_z->setColor(Color4f::Yellow()); else circle_z->setColor(Color4f::Blue ());engine->Render(circle_z);}
	}    

    // translateX
    else if (this->operation==TRANSLATE)
	{
		if (!freeze_x) {if (this->ref==REF_X) line_x->setColor(Color4f::Yellow()); else line_x->setColor(Color4f::Red  ());engine->Render(line_x);}
		if (!freeze_y) {if (this->ref==REF_Y) line_y->setColor(Color4f::Yellow()); else line_y->setColor(Color4f::Green());engine->Render(line_y);}
		if (!freeze_z) {if (this->ref==REF_Z) line_z->setColor(Color4f::Yellow()); else line_z->setColor(Color4f::Blue ());engine->Render(line_z);}
	}  
	else if (this->operation==SCALE)
	{
		if (!freeze_x) {if (this->ref==REF_X) line_x->setColor(Color4f::Yellow()); else line_x->setColor(Color4f::Red  ());engine->Render(line_x);}
        if (!freeze_y) {if (this->ref==REF_Y) line_y->setColor(Color4f::Yellow()); else line_y->setColor(Color4f::Green());engine->Render(line_y);}
        if (!freeze_z) {if (this->ref==REF_Z) line_z->setColor(Color4f::Yellow()); else line_z->setColor(Color4f::Blue ());engine->Render(line_z);}
	}

	engine->SetLineWidth(1);
	engine->SetPolygonMode(Batch::POLYGON);

	
}



///////////////////////////////////////////////////////////////////////
#include <xge/Viewer.h>

class ManipulatorSelfTest: public Viewer
{
public:

    Manipulator manipulator;
    Box3f box;
    Mat4f T;

    ManipulatorSelfTest()
		:Viewer()
    {
	    this->box=Box3f(Vec3f(0,0,0),Vec3f(1,1,1));
	    this->T=Mat4f(); // identity matrix;
	    this->manipulator.setObject(this->box,&this->T);
	    this->frustum.guessBestPosition(this->box);
	    this->Redisplay();
    }

	virtual void Keyboard(int key,int x,int y)
	{
		if (key=='t' || key=='T')
		{
			manipulator.setOperation(Manipulator::TRANSLATE);
			this->manipulator.setObject(this->box,&this->T);
			this->Redisplay();
		}
		else if (key=='s' || key=='S')
		{
			manipulator.setOperation(Manipulator::SCALE);
			this->manipulator.setObject(this->box,&this->T);
			this->Redisplay();
		}
		else if (key=='r' || key=='R')
		{
			manipulator.setOperation(Manipulator::ROTATE);
			this->manipulator.setObject(this->box,&this->T);
			this->Redisplay();
		}
		else
		{
			Viewer::Keyboard(key,x,y);
		}
	}
		
    virtual void Mouse(MouseEvent args)
    {
        Ray3f ray=this->frustum.unproject(args.x,args.y);
        this->manipulator.Mouse(args,ray);
        this->Redisplay();
    }


    virtual  void Render()
    {
		engine->ClearScreen();
		engine->SetViewport(frustum.x,frustum.y,frustum.width,frustum.height);
		engine->SetProjectionMatrix(frustum.projection_matrix);
		engine->SetModelviewMatrix(frustum.getModelviewMatrix());
		engine->SetDefaultLight(this->frustum.pos);

		SmartPointer<Batch> cube=Batch::Cube(this->box);
		cube->matrix=T * cube->matrix;
		engine->Render(cube);
		manipulator.Render(engine);
		engine->FlushScreen();
    }
}; //end class


void Manipulator::SelfTest()
{
	ManipulatorSelfTest v;
	v.Run();
	v.Wait();
}
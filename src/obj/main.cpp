
#define NOMINMAX
#include <xge/xge.h>
#include <xge/glcanvas.h>
#include <xge/batch.h>

#include <iostream>
#include <chrono>
#include <sstream>

#include <JUCE/AppConfig.h>
#include <JUCE/modules/juce_gui_basics/juce_gui_basics.h>

#include "xge/xge_gl.h"

typedef std::string String;

/////////////////////////////////////////////////////////////////////
static double cdouble(String value)
{
  std::istringstream parser(value);
  double ret;
  parser>>ret;
  return ret;
}

static String cstring(int value)
{
  std::ostringstream out;
  out<<value;
  return out.str();
}



///////////////////////////////////////////////////////////////////////////////
class Filler
{

  int n=0;
  SmartPointer<Array> array;

public:

  
  //constructor
  Filler() : array(new Array()){
    array->resize(1024*1024);
  }

  //destructor
  ~Filler() {
  }

  //getArray
  SmartPointer<Array> getArray()
  {
    array->resize(this->n);
    return array;
  }

  //empty
  bool empty() const {
    return n==0;
  }

  //size
  int size() const{
    return n;
  }

  //resize
  void resize(int n) {
  
    while (n>array->size())
      array->resize(array->size()<<1);
    this->n=n;
  }

  //operator[]
  const float& operator[](int index) const {
    return ((float*)array->c_ptr())[index];
  }

  //operator[]
  float& operator[](int index)  {
    return ((float*)array->c_ptr())[index];
  }

  //push_back
  void push_back(float value)
  {
    if (n==array->size())
      array->resize(array->size()<<1);

    (*this)[n++]=value;
  }


};


//////////////////////////////////////////////////////////////////////////////////////////////////////
class VerticesDB
{
public:

  typedef std::pair<float,std::pair<float,float> > Vertex;
  std::map<Vertex,int> map;

  //exportVertex
  int exportVertex(FILE* file,float x,float y,float z)
  {
    Vertex v(x,std::make_pair(y,z));
    auto it=map.find(v);
    
    if (it!=map.end()) 
      return it->second;
    
    int ret=map.size()+1;
    map[v]=ret;
    fprintf(file,"v %f %f %f\n",x,y,z);
    return ret;
  }

};


///////////////////////////////////////////////////////////////////////////////////
class OBJFile
{
public:

  std::vector< SmartPointer<Batch> > batches;

  //constructor
  OBJFile() {
  }

  //getBoundingBox
  Box3f getBoundingBox()
  {
    Box3f ret;
    for (int I=0;I<(int)batches.size();I++)
      ret.add(batches[I]->getBox());
    return ret;
  }

  //open
  bool open(String filename,String filter="")
  {
    this->batches.clear();

    Clock t1;
    std::cout<<"Loading obj "<<filename<<" filter("<<filter<<")..."<<std::endl;

    FILE* file=fopen(filename.c_str(),"rt");

    if (!file)  
	    return false;

    Filler vertices;
    Filler quads;
    Filler normals;

    char  line[1024];
    int   i0,i1,i2,i3;
    float X,Y,Z;

    Box3f box;

    bool bComputeNormals=false;

    while (fgets(line, sizeof(line),file))
    {
	    switch (line[0]) 
	    {		
	      case 'v':
		    {
				  sscanf(line+1, "%f %f %f", &X, &Y, &Z);
				  vertices.push_back(X); 
				  vertices.push_back(Y); 
				  vertices.push_back(Z); 
          box.add(Vec3f(X,Y,Z));
			    break;
		    }
		
	      case 'f':
		    {	
				  sscanf(line+1,"%d %d %d %d",&i0,&i1,&i2,&i3); //NOTE I need quads here!
          i0=(i0-1)*3; Vec3f v0(vertices[i0+0],vertices[i0+1],vertices[i0+2]);
          i1=(i1-1)*3; Vec3f v1(vertices[i1+0],vertices[i1+1],vertices[i1+2]);
          i2=(i2-1)*3; Vec3f v2(vertices[i2+0],vertices[i2+1],vertices[i2+2]);
          i3=(i3-1)*3; Vec3f v3(vertices[i3+0],vertices[i3+1],vertices[i3+2]);
          quads.push_back(v0.x); quads.push_back(v0.y); quads.push_back(v0.z);
          quads.push_back(v1.x); quads.push_back(v1.y); quads.push_back(v1.z);
          quads.push_back(v2.x); quads.push_back(v2.y); quads.push_back(v2.z);
          quads.push_back(v3.x); quads.push_back(v3.y); quads.push_back(v3.z);

          if (bComputeNormals)
          {
            Vec3f n=(Plane4f(v0,v1,v2).getNormal()+Plane4f(v0,v2,v3).getNormal()).normalize();
            normals.push_back(n.x); normals.push_back(n.y); normals.push_back(n.z);
            normals.push_back(n.x); normals.push_back(n.y); normals.push_back(n.z);
            normals.push_back(n.x); normals.push_back(n.y); normals.push_back(n.z);
            normals.push_back(n.x); normals.push_back(n.y); normals.push_back(n.z);
          }

			    break;
		    }

	      default: 
		    {
			    break;
		    }
      }
    }

    if (vertices.empty())
      return false;

    //filter boundary quads
    if (!filter.empty())
    {
      const float DoNotFilter=std::numeric_limits<float>::max();

      Vec3f f0,f1;
      f0.x=filter.find("x0")!=String::npos? box.p1.x : DoNotFilter; f1.x=filter.find("x1")!=String::npos? box.p2.x : DoNotFilter;
      f0.y=filter.find("y0")!=String::npos? box.p1.y : DoNotFilter; f1.y=filter.find("y1")!=String::npos? box.p2.y : DoNotFilter;
      f0.z=filter.find("z0")!=String::npos? box.p1.z : DoNotFilter; f1.z=filter.find("z1")!=String::npos? box.p2.z : DoNotFilter;

      int N=0;
      for (int I=0;I<(int)quads.size();I+=12)
      {
        float X0=min4(quads[I+0],quads[I+3],quads[I+6],quads[I+ 9]); float X1=max4(quads[I+0],quads[I+3],quads[I+6],quads[I+ 9]);
        float Y0=min4(quads[I+1],quads[I+4],quads[I+7],quads[I+10]); float Y1=max4(quads[I+1],quads[I+4],quads[I+7],quads[I+10]);
        float Z0=min4(quads[I+2],quads[I+5],quads[I+8],quads[I+11]); float Z1=max4(quads[I+2],quads[I+5],quads[I+8],quads[I+11]);
        if (X0==X1 && (X0==f0.x || X0==f1.x)) continue; 
        if (Y0==Y1 && (Y0==f0.y || Y0==f1.y)) continue; 
        if (Z0==Z1 && (Z0==f0.z || Z0==f1.z)) continue; 
        for (int J=0;J<12;J++,N++) 
        {
          quads[N]=quads[I+J];
          if (!normals.empty())
            normals[N]=normals[I+J];
        }
      }

      int old_num_quads=quads.size()/12;
      quads.resize(N);

      if (!normals.empty())
        normals.resize(N);

      int new_num_quads=quads.size()/12;
      std::cout<<"Removed "<<(old_num_quads-new_num_quads)<<" boundary quads"<<std::endl;
    }

    SmartPointer<Batch> batch(new Batch());
    batch->primitive=Batch::QUADS;
    batch->vertices=quads.getArray();

    if (!normals.empty())
      batch->normals=normals.getArray();

    std::cout<<"...done in "<<t1.msec()<<"msec box("<<batch->getBox().str()<<")"<<std::endl;

    this->batches.push_back(batch);
    return true;
  }

  //translate
  void translate(Vec3f vt)
  {
    for (int B=0;B<(int)batches.size();B++)
    {
      int tot=batches[B]->vertices->size();
      float* V=(float*)batches[B]->vertices->c_ptr();
      for (int I=0;I<tot;I+=3,V+=3){
        V[0]+=vt.x;V[1]+=vt.y;V[2]+=vt.z;
      }
      batches[B]->invalidateBox();
    }
  }

  //save
  bool save(String filename)
  {
    Clock t1;
    std::cout<<"Saving obj file "<<filename<<"..."<<std::endl;
    FILE* file=fopen(filename.c_str(),"wt");
	  if (!file)
      return false;

    VerticesDB vertices;

	  for (int I=0;I<(int)batches.size();I++)
	  {
		  SmartPointer<Batch> batch=batches[I];
		  XgeReleaseAssert(batch->primitive==Batch::QUADS);

		  int nquads=batch->vertices->size()/12;
		  float* V=(float*)batch->vertices->c_ptr();
		  for (int i=0;i<nquads;i++,V+=12)
		  {
        float v0[3]={V[ 0],V[ 1],V[ 2]} ; 
        float v1[3]={V[ 3],V[ 4],V[ 5]} ; 
			  float v2[3]={V[ 6],V[ 7],V[ 8]} ; 
        float v3[3]={V[ 9],V[10],V[11]} ; 
			  fprintf(file,"f %d %d %d %d\n",
          vertices.exportVertex(file,v0[0],v0[1],v0[2]),
          vertices.exportVertex(file,v1[0],v1[1],v1[2]),
          vertices.exportVertex(file,v2[0],v2[1],v2[2]),
          vertices.exportVertex(file,v3[0],v3[1],v3[2]));
		  }
	  }
	  fclose(file);
    std::cout<<"...done in "<<t1.msec()<<"msec"<<std::endl;
    return true;
  }

  //glRender
  void glRender(GLCanvas& gl,int& NUM_VERTICES,const int MAX_VERTICES)
  {
    for (int I=0;I<(int)batches.size() && NUM_VERTICES<MAX_VERTICES;I++)
    {
      SmartPointer<Batch>& batch=batches[I];
      int nv=batch->vertices->size()/3;
      int delta=1024*1024;
      int offset=0;

      while (offset<nv && NUM_VERTICES<MAX_VERTICES)
      {
        if (gl.draw_lines)
        {
          gl.setPolygonOffset(1.0);
          gl.renderBatch(batch,offset,delta);

          gl.setPolygonOffset(0);
          gl.setDepthWrite(false);
          
          gl.setLineWidth(1);
          gl.setPolygonMode(Batch::LINES);
          Color4f ambient=batch->ambient;
          Color4f diffuse=batch->diffuse;
          batch->setColor(Color4f(0,0,0,0.02f));
          gl.renderBatch(batch,offset,delta);
          batch->ambient=ambient;
          batch->diffuse=diffuse;
          gl.setDepthWrite(true);
          gl.setPolygonMode(Batch::POLYGON);
          gl.setLineWidth(2);
        }
        else
        {
          gl.renderBatch(batch,offset,delta);
        }
      
        offset+=delta;
        NUM_VERTICES+=delta;
      }
    }
  }

};



/////////////////////////////////////////////////////////////////////
class PovRay
{
public:

  FILE*  master_file=nullptr;
  FILE*  mesh_file=nullptr;
  String filename;
  int    num_mesh=0;

  //constructor
  PovRay(String filename_) : filename(filename_)
  {
    master_file=fopen(filename.c_str(),"wt");

    fprintf(master_file,"#include \"colors.inc\"\n"); 
    fprintf(master_file,"#include \"shapes.inc\" \n"); 
    fprintf(master_file,"#include \"textures.inc\" \n"); 
    fprintf(master_file,"#include \"metals.inc\"\n"); 
    fprintf(master_file,"#include \"skies.inc\"\n"); 

    fprintf(master_file,"global_settings\n");
    fprintf(master_file,"{\n");
    fprintf(master_file,"  assumed_gamma 1\n");
    fprintf(master_file,"  max_trace_level 5\n");
    fprintf(master_file,"  radiosity\n");
    fprintf(master_file,"  {\n");
    fprintf(master_file,"    pretrace_start 0.08\n");
    fprintf(master_file,"    pretrace_end   0.02\n");
    fprintf(master_file,"    count 50\n");
    fprintf(master_file,"    nearest_count 5\n");
    fprintf(master_file,"    error_bound 0.15\n");
    fprintf(master_file,"    recursion_limit 1\n");
    fprintf(master_file,"    low_error_factor .5\n");
    fprintf(master_file,"    gray_threshold 0.0\n");
    fprintf(master_file,"    minimum_reuse 0.015\n");
    fprintf(master_file,"    brightness 0.6\n");
    fprintf(master_file,"    adc_bailout 0.01/2\n");
    fprintf(master_file,"  }\n");
    fprintf(master_file,"}\n");  
    fprintf(master_file,"\n");
  }

  //destructor
  ~PovRay()
  {
    close();
  }

  //opened
  bool opened() const {
    return master_file?true:false;
  }

  //close
  void close()
  {
    if (!master_file) return;
    fclose(master_file);
    master_file=nullptr;
  }

  //setBackgroundColor
  void setBackgroundColor(Color4f color)
  {
    fprintf(master_file,"background {rgb<%f,%f,%f>}\n",(float)color.r,(float)color.g,(float)color.b);     
    fprintf(master_file,"\n");
  }

  //exportReferenceAxis
  void exportReferenceAxis(Vec3f p1,Vec3f p2)
  {
    double thickness=(p2.x-p1.z)*0.005;

    //x
    fprintf(master_file,"object {cylinder {<%f,%f,%f>,<%f,%f,%f>,%f texture{ pigment{rgb<1,0,0>} }}}\n",(float)p1.x,(float)p1.y,(float)p1.z,(float)p2.x,(float)p1.y,(float)p1.z,(float)thickness);
    fprintf(master_file,"object {cylinder {<%f,%f,%f>,<%f,%f,%f>,%f texture{ pigment{rgb<1,0,0>} }}}\n",(float)p1.x,(float)p2.y,(float)p1.z,(float)p2.x,(float)p2.y,(float)p1.z,(float)thickness);
    fprintf(master_file,"object {cylinder {<%f,%f,%f>,<%f,%f,%f>,%f texture{ pigment{rgb<1,0,0>} }}}\n",(float)p1.x,(float)p1.y,(float)p2.z,(float)p2.x,(float)p1.y,(float)p2.z,(float)thickness);
    fprintf(master_file,"object {cylinder {<%f,%f,%f>,<%f,%f,%f>,%f texture{ pigment{rgb<1,0,0>} }}}\n",(float)p1.x,(float)p2.y,(float)p2.z,(float)p2.x,(float)p2.y,(float)p2.z,(float)thickness);

    //y
    fprintf(master_file,"object {cylinder {<%f,%f,%f>,<%f,%f,%f>,%f texture{ pigment{rgb<0,1,0>} }}}\n",(float)p1.x,(float)p1.y,(float)p1.z,(float)p1.x,(float)p2.y,(float)p1.z,(float)thickness);
    fprintf(master_file,"object {cylinder {<%f,%f,%f>,<%f,%f,%f>,%f texture{ pigment{rgb<0,1,0>} }}}\n",(float)p2.x,(float)p1.y,(float)p1.z,(float)p2.x,(float)p2.y,(float)p1.z,(float)thickness);
    fprintf(master_file,"object {cylinder {<%f,%f,%f>,<%f,%f,%f>,%f texture{ pigment{rgb<0,1,0>} }}}\n",(float)p1.x,(float)p1.y,(float)p2.z,(float)p1.x,(float)p2.y,(float)p2.z,(float)thickness);
    fprintf(master_file,"object {cylinder {<%f,%f,%f>,<%f,%f,%f>,%f texture{ pigment{rgb<0,1,0>} }}}\n",(float)p2.x,(float)p1.y,(float)p2.z,(float)p2.x,(float)p2.y,(float)p2.z,(float)thickness);

    //z
    fprintf(master_file,"object {cylinder {<%f,%f,%f>,<%f,%f,%f>,%f texture{ pigment{rgb<0,0,1>} }}}\n",(float)p1.x,(float)p1.y,(float)p1.z,(float)p1.x,(float)p1.y,(float)p2.z,(float)thickness);
    fprintf(master_file,"object {cylinder {<%f,%f,%f>,<%f,%f,%f>,%f texture{ pigment{rgb<0,0,1>} }}}\n",(float)p2.x,(float)p1.y,(float)p1.z,(float)p2.x,(float)p1.y,(float)p2.z,(float)thickness);
    fprintf(master_file,"object {cylinder {<%f,%f,%f>,<%f,%f,%f>,%f texture{ pigment{rgb<0,0,1>} }}}\n",(float)p1.x,(float)p2.y,(float)p1.z,(float)p1.x,(float)p2.y,(float)p2.z,(float)thickness);
    fprintf(master_file,"object {cylinder {<%f,%f,%f>,<%f,%f,%f>,%f texture{ pigment{rgb<0,0,1>} }}}\n",(float)p2.x,(float)p2.y,(float)p1.z,(float)p2.x,(float)p2.y,(float)p2.z,(float)thickness);

    fprintf(master_file,"\n");
  }

  //exportCamera
  void exportCamera(Vec3f pos,Vec3f vdir,Vec3f up,double angle)
  {
    fprintf(master_file,"camera {location <%f,%f,%f> direction<%f,%f,%f> up<%f,%f,%f> angle %f sky<0,0,1>}\n",
      (float)pos.x,(float)pos.y,(float)pos.z,
      (float)vdir.x,(float)vdir.y,(float)vdir.z,
      0.0f,0.0f,1.0f,/*(float)up.x,(float)up.y,(float)up.z,*/
      (float)angle);
    fprintf(master_file,"\n");
  }

  //exportLight
  void exportLight(Vec3f pos,Color4f color)
  {
    fprintf(master_file,"light_source { <%f,%f,%f> color rgb<%f,%f,%f> }\n",(float)pos.x,(float)pos.y,(float)pos.z,(float)color.r,(float)color.g,(float)color.b);
    fprintf(master_file,"\n");
  }

  //beginMesh
  void beginMesh()
  {
    String mesh_filename=this->filename+"."+cstring(num_mesh++)+".pov";
    fprintf(master_file,"union{\n");
    fprintf(master_file,"#include \"%s\"\n",mesh_filename.c_str());
    this->mesh_file=fopen(mesh_filename.c_str(),"wt");
  }

  //endMesh
  void endMesh()
  {
    fclose(mesh_file);
    mesh_file=nullptr;
    fprintf(master_file,"texture {pigment {color rgb <0.6,0.6,0.6>}}\n");
    fprintf(master_file,"}\n\n");
  }

  //exportFlatTriangle
  void exportFlatTriangle(float* v0,float* v1,float* v2)
  {
		fprintf(mesh_file,"  triangle{<%e,%e,%e>,<%e,%e,%e>,<%e,%e,%e>}\n",
			(float)v0[0],(float)v0[1],(float)v0[2],
			(float)v1[0],(float)v1[1],(float)v1[2],
			(float)v2[0],(float)v2[1],(float)v2[2]);
  }

  //exportSmoothTriangle
  void exportSmoothTriangle(float* v0,float* v1,float* v2,float* n0,float* n1,float* n2)
  {
		fprintf(mesh_file,"  smooth_triangle{<%e,%e,%e>,<%e,%e,%e>,<%e,%e,%e>,<%e,%e,%e>,<%e,%e,%e>,<%e,%e,%e>}\n",
			(float)v0[0],(float)v0[1],(float)v0[2],(float)n0[0],(float)n0[1],(float)n0[2],
			(float)v1[0],(float)v1[1],(float)v1[2],(float)n1[0],(float)n1[1],(float)n1[2],
			(float)v2[0],(float)v2[1],(float)v2[2],(float)n2[0],(float)n2[1],(float)n2[2]);
  }

  //exportFlatQuad
  void exportFlatQuad(float* v0,float* v1,float* v2,float* v3)
  {
    exportFlatTriangle(v0,v1,v2);
    exportFlatTriangle(v0,v2,v3);
  }

  //exportSmoothQuad
  void exportSmoothQuad(float* v0,float* v1,float* v2,float* v3,float* n0,float* n1,float* n2,float* n3)
  {
    exportSmoothTriangle(v0,v1,v2,n0,n1,n2);
    exportSmoothTriangle(v0,v2,v3,n0,n2,n3);
  }

  //exportOBJ
  void exportOBJ(OBJFile& src)
  {
    for (int B=0;B<(int)src.batches.size();B++)
    {
      SmartPointer<Batch> batch=src.batches[B];

      beginMesh();

      float* V=(float*)(batch->vertices->c_ptr());
      float* N=nullptr;//(float*)(batch->normals?batch->normals->c_ptr() : nullptr);

      if(batch->primitive==Batch::TRIANGLES)
      {
        for (int J=0;J<batch->vertices->size();J+=9,V+=9,N=N?N+9:nullptr)
        {
          N? exportSmoothQuad(V+0,V+3,V+6,V+9,N+0,N+3,N+6,N+9) : exportFlatQuad(V+0,V+3,V+6,V+9);
        }
      }
      else if(batch->primitive==Batch::QUADS)
      {
        for (int J=0;J<batch->vertices->size();J+=12,V+=12,N=N?N+12:nullptr)
        {
          N? exportSmoothQuad(V+0,V+3,V+6,V+9,N+0,N+3,N+6,N+9) : exportFlatQuad(V+0,V+3,V+6,V+9);
        }
      }
      else
      {
        assert(false);
      }

      endMesh();
    }
  }

};

/////////////////////////////////////////////////////////////////////
class OBJViewer : public GLCanvas
{
public:


  //_________________________________________________________
  class OneShotTimer : public juce::Timer{
  
  public:

    std::function<void()> fn;

    //constructor
    OneShotTimer(int msec,std::function<void()> fn_) : fn(fn_){
      juce::Timer::startTimer(msec);
    }

    //destructor
    virtual ~OneShotTimer() {
    }

    //timerCallback
    virtual void timerCallback() override {
      juce::Timer::stopTimer();
      fn();
    }
  };

  Box3f                                world_box;
  std::vector< SmartPointer<OBJFile> > objs;
  bool                                 bFullDisplay=true;
  SmartPointer<OneShotTimer>           schedule_full_redisplay;

  //constructor
  OBJViewer() {
    this->draw_lines=true;
  }

  //destructor
  virtual ~OBJViewer(){
  }

  //guessPosition
  void guessPosition()
  {
    this->world_box=Box3f();
    for (int I=0;I<(int)objs.size();I++)
      this->world_box.add(objs[I]->getBoundingBox());
    this->trackball_center =world_box.center();
    this->frustum->guessBestPosition(world_box);
    this->frustum->walk_speed*=3;
    this->redisplay();
    this->scheduleFullRedisplay();
  }

  //addObjFile
  void addObjFile(SmartPointer<OBJFile> value)
  {
    objs.push_back(value);
  }
  
  //getWordBox
  virtual Box3f getWorldBox() override {

    return this->world_box;
  }

  //scheduleFullRedisplay
  void scheduleFullRedisplay()
  {
    this->schedule_full_redisplay.reset(new OneShotTimer(100,[this](){
      this->bFullDisplay=true;
      this->redisplay();
    }));
    this->bFullDisplay=false;
  }

  //onMouseDown
	virtual void onMouseDown(int button,int x,int y) override{
    GLCanvas::onMouseDown(button,x,y);
    scheduleFullRedisplay();
  }

  //onMouseMove
	virtual void onMouseMove(int button,int x,int y)override{
    GLCanvas::onMouseMove(button,x,y);
    scheduleFullRedisplay();
  }

  //onMouseUp
	virtual void onMouseUp(int button,int x,int y)override{
    GLCanvas::onMouseUp(button,x,y);
    scheduleFullRedisplay();
  }

  //onMouseWheel
  virtual void onMouseWheel(int delta)override{
    GLCanvas::onMouseWheel(delta);
    scheduleFullRedisplay();
  }

  //enlapsedMsec
  static int enlapsedMsec(std::chrono::system_clock::time_point& t1)
  {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-t1).count();
  }

  //renderWorldBox
  void renderWorldBox()
  {
    Vec3f p1=world_box.p1;
    Vec3f p2=world_box.p2;

    setLineWidth(1);
    glColor3f(0.5,0.5,0.5);
    glBegin(GL_LINE_LOOP);
    glVertex3f(p1.x,p1.y,p1.z); 
    glVertex3f(p2.x,p1.y,p1.z); 
    glVertex3f(p2.x,p2.y,p1.z); 
    glVertex3f(p1.x,p2.y,p1.z); 
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(p1.x,p1.y,p2.z); 
    glVertex3f(p2.x,p1.y,p2.z); 
    glVertex3f(p2.x,p2.y,p2.z); 
    glVertex3f(p1.x,p2.y,p2.z); 
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(p1.x,p1.y,p1.z); glVertex3f(p1.x,p1.y,p2.z); 
    glVertex3f(p2.x,p1.y,p1.z); glVertex3f(p2.x,p1.y,p2.z); 
    glVertex3f(p2.x,p2.y,p1.z); glVertex3f(p2.x,p2.y,p2.z); 
    glVertex3f(p1.x,p2.y,p1.z); glVertex3f(p1.x,p2.y,p2.z); 
    glEnd();

    setLineWidth(1);
  }

  //renderModel
  virtual void renderModel() override
  {
    int NUM_VERTICES=0;
    int MAX_VERTICES=bFullDisplay? std::numeric_limits<int>::max() : 4*1024*1014;

    for (int O=0;O<(int)objs.size();O++)
      objs[O]->glRender(*this,NUM_VERTICES,MAX_VERTICES);

    if (NUM_VERTICES==MAX_VERTICES)
      scheduleFullRedisplay();
  }


  //////////////////////////////////////////////////////////////
  virtual void renderOpenGL() override
  {
    glClearColor(1,1,1,0);
    clearScreen();
    setViewport(frustum->x,frustum->y,frustum->width,frustum->height);
    setProjectionMatrix(frustum->projection_matrix);
    setModelviewMatrix(frustum->getModelviewMatrix());
    setDefaultLight(this->frustum->pos,this->frustum->dir);
    renderWorldBox();
    renderModel();

  }

};



/////////////////////////////////////////////////////////////////////
int main(int nargs,char** args)
{	
  XgeModule::init();

  SmartPointer<OBJViewer> viewer;

  if (true)
  {
    int cycle=0;
    std::vector< std::pair<String, Vec3f> > crops;
    crops.push_back(std::make_pair("F:/cad2016/high-res-data/ciclo"+cstring(cycle)+"/output-cropXY_0_1024_0_1024/output000%d/stl/model-2.obj"      ,Vec3f(   0,   0,   0)));
    crops.push_back(std::make_pair("F:/cad2016/high-res-data/ciclo"+cstring(cycle)+"/output-cropXY_1024_2048_0_1024/output000%d/stl/model-2.obj"   ,Vec3f(1023,   0,   0)));
    crops.push_back(std::make_pair("F:/cad2016/high-res-data/ciclo"+cstring(cycle)+"/output-cropXY_1024_2048_1024_2048/output000%d/stl/model-2.obj",Vec3f(1023,1023,   0)));
    crops.push_back(std::make_pair("F:/cad2016/high-res-data/ciclo"+cstring(cycle)+"/output-cropXY_0_1024_1024_2048/output000%d/stl/model-2.obj"   ,Vec3f(   0,1023,   0)));

    SmartPointer<PovRay> povray(new PovRay("mesh.pov"));
    if (povray)
    {

      Box3f world_box(Vec3f(0,0,0),Vec3f(2048,2048,2048));
      povray->setBackgroundColor(Color4f::White());
      povray->exportCamera(Vec3f(3000,3000,3000),Vec3f(-3000,-3000,-3000).normalize(),Vec3f(0,0,1),DEFAULT_FOV);

      Vec3f p1    =world_box.p1;
      Vec3f p2    =world_box.p2;
      Vec3f dim   =world_box.size();
      Vec3f middle=0.5*(p1+p2);

      povray->exportReferenceAxis(p1,p2);
      povray->exportLight(Vec3f(middle.x,middle.y,p1.z+5*dim.z),Color4f(0.8f,0.8f,0.8f));
      povray->exportLight(Vec3f(    p2.x,    p1.y,p1.z+2*dim.z),Color4f(0.6f,0.6f,0.6f));
    }

    for (int Crop=0;Crop<4;Crop++)
    {
      String filename_template=crops[Crop].first;
      Vec3f vt=crops[Crop].second;
      for (int I=0;I<30;I++)
      {
        SmartPointer<OBJFile> obj(new OBJFile());
        obj->open(Utils::Replace(filename_template,"%d",cstring(I)));
        int depth=obj->getBoundingBox().size().z;
        obj->translate(vt);

        if (povray)
          povray->exportOBJ(*obj);
        
        if (viewer)
          viewer->addObjFile(obj);

        vt.z+=30;
      }
    }

    if (povray)
    {
      povray->close();
      povray.reset();
    }

    if (viewer)
    {
      viewer->guessPosition();
      viewer->runLoop();
    }
  }

#if _WIN32 && _DEBUG
  std::cout<<"Press char to exit"<<std::endl;
  getchar();
#endif

  return 0;
}



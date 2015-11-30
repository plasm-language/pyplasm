
%module(directors="1") xgepy

%{ #include "xge/xge.h" %}

%include <stl.i>
%include <std_shared_ptr.i>

#define SmartPointer std::shared_ptr


////////////////////////////////////////////////////////
//common templates for STL vector
////////////////////////////////////////////////////////

%template(StdVectorUnsignedChar ) std::vector< unsigned char  >;
%template(StdVectorInt          ) std::vector< int            >;
%template(StdVectorStdVectorInt ) std::vector< std::vector< int> >;
%template(StdVectorFloat        ) std::vector< float          >;
%template(StdVectorString       ) std::vector< std::string    >;

////////////////////////////////////////////////////////
// Python specific stuff
////////////////////////////////////////////////////////

%rename(__str__) str;
%rename(__repr__) repr;
%rename(assign) *::operator=;
%rename(__getitem__    ) *::operator[](int) const;
%rename(__getitem_ref__) *::operator[](int idx); 
%ignore  *::operator++;
%ignore  *::operator--;
%ignore SinglePool;
%ignore MemPool;




/////////////////////////////////////////////////////////////////////////////////////////////////
//here configuration for your classes 
//NOTE: do not mix std::vector with both shared (std::vector<SmartPointer<SwigClass> >) and non shared (std::vector<SmartPointer*> ) for the same class
/////////////////////////////////////////////////////////////////////////////////////////////////

%ignore FrustumIterator::Item;
%ignore RayIterator::Item;
%ignore SinglePool;
%ignore MemPool;


%feature("director") GLCanvas;

%shared_ptr(Matf)
%shared_ptr(Array)
%shared_ptr(Texture)
%shared_ptr(Graph)
%shared_ptr(Hpc)
%shared_ptr(Batch)
%shared_ptr(Octree)
%shared_ptr(Frustum)

%template(StdVectorVec3f)                             std::vector< Vec3f >;
%template(StdVectorSmartPointerHpc)                   std::vector< SmartPointer<Hpc> >;
%template(StdVectorSmartPointerBatch)                 std::vector< SmartPointer<Batch> >;
%template(StdVectorSmartPointerTexture)               std::vector< SmartPointer<Texture> >;

////////////////////////////////////////////////////////////
//your header here
////////////////////////////////////////////////////////////

%include <xge/xge.h>
%include <xge/config.h>
%include <xge/utils.h>
%include <xge/log.h>
%include <xge/clock.h>
%include <xge/keyboard.h>
%include <xge/spinlock.h>
%include <xge/mempool.h>
%include <xge/archive.h>
%include <xge/encoder.h>
%include <xge/filesystem.h>
//%include <xge/thread.h>
%include <xge/vec.h>
%include <xge/triangle.h>
%include <xge/plane.h>
%include <xge/mat.h>
%include <xge/ball.h>
%include <xge/box.h>
%include <xge/ray.h>
%include <xge/quaternion.h>
%include <xge/localsystem.h>
%include <xge/array.h>
%include <xge/color4f.h>
%include <xge/frustum.h>
%include <xge/texture.h>
%include <xge/manipulator.h>
%include <xge/batch.h>
%include <xge/pick.h>
%include <xge/graph.h>
%include <xge/octree.h>
%include <xge/glcanvas.h>
%include <xge/unwrapper.h>
%include <xge/bake.h>
%include <xge/plasm.h>

%init %{
   XgeModule::init();
%}


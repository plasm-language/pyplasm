#ifndef _VIEWER_H__
#define _VIEWER_H__

#include <xge/xge.h>
#include <xge/Octree.h>


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class XGE_API Viewer : protected Thread
{
public:

	// rendering engine
	Engine* engine;

	// the os window
	int64 window;

	//! navigation using trackball
	bool  trackball_mode;
	Vec3f trackball_center;

	//! internal mouse position
	int  mouse_beginx,mouse_beginy;

	//if to draw lines or not
	bool DrawLines;
	bool DrawAxis;

	//! current frustum
	Frustum frustum;

	//! for debugging purpouse, fix a frustum and navigate to see the frustum
	Frustum* debug_frustum;

	//octree
	SmartPointer<Octree> octree;

	//default
	Viewer();

	//constructor
	Viewer(SmartPointer<Octree> octree);

	//! destructor
	virtual ~Viewer();

	//! virtual render
	virtual void Render();

	//!ask for a redisplay
	virtual void Redisplay();

	//! virtuals
	virtual void Keyboard (int key,int x,int y) ;
	virtual void Mouse    (MouseEvent args);
	virtual void Resize   (int width,int height);

	// OS specific functions
	virtual void Close();
	
	//!Run
	virtual void Run();

	//!Wait
	virtual void Wait();

	//! self test (progressive scene)
	static void SelfTest(int nx=100,int ny=100,int nz=1);

	bool m_close;

protected:


	//signal for redisplay
	bool m_redisplay;

		//for the thread
	virtual void doJob(int);
};





#endif //_VIEWER_H__


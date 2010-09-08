#include <xge/xge.h>
#include <xge/box.h>

//////////////////////////////////////////////////////////////////////
int Box3f::SelfTest()
{
	Log::printf("Testing Box3f...\n");

	Box3f b;
	ReleaseAssert(!b.isValid());//at the beginning shoud not be valud

	//add a point
	Vec3f p1(1,2,3);
	b.add(p1);
	ReleaseAssert(b.p1==p1 && b.p2==p1);

	//add a second point
	Vec3f p2(-1,+3,+4);
	b.add(p2);
	ReleaseAssert(
		   b.p1==Vec3f(min2(p1.x,p2.x),min2(p1.y,p2.y),min2(p1.z,p2.z)) 
		&& b.p2==Vec3f(max2(p1.x,p2.x),max2(p1.y,p2.y),max2(p1.z,p2.z))); 

	//test volume and surface
	float s=b.surface();
	float v=b.volume();
	ReleaseAssert(s==10.0f && v==2.0);

	//add a second bounding box
	Box3f b2;
	b2.add(Vec3f(10,11,12));
	b2.add(Vec3f(20,21,22));
	b.add(b2);
	ReleaseAssert(b.p1==Vec3f(-1,2,3) && b.p2==Vec3f(20,21,22));

	return 0;
}



//////////////////////////////////////////////////////////////////////
int Boxf::SelfTest()
{
	Log::printf("Testing Boxf...\n");

	Boxf b(5);
	ReleaseAssert(!b.isValid());//at the beginning shoud not be valid

	//add a point
	float _p1[]={1, 1,2,3,4,5};
	Vecf p1(5,_p1);

	b.add(p1);
	ReleaseAssert(b.p1==p1 && b.p2==p1 && b.isValid());

	//add a second point
	float _p2[]={1, -1,3,4,5,6};
	Vecf p2(5,_p2);

	b.add(p2);

	ReleaseAssert(
		b.p1[0]==1 && b.p1[1]==-1 && b.p1[2]==2 && b.p1[3]==3 && b.p1[4]==4 && b.p1[5]==5 &&
		b.p2[0]==1 && b.p2[1]==+1 && b.p2[2]==3 && b.p2[3]==4 && b.p2[4]==5 && b.p2[5]==6 
		); 

	//test volume and surface
	float v=b.volume();
	ReleaseAssert(b.isValid());


	ReleaseAssert(v==2);

	//add a second bounding box
	Boxf b2(5);

	float _p3[]={1, 10,11,12,13,14};Vecf p3(5,_p3);
	float _p4[]={1, 20,21,22,23,24};Vecf p4(5,_p4);

	b2.add(p3);
	b2.add(p4);
	b.add(b2);
	ReleaseAssert(b.p1[0]==1 && b.p1[1]==-1 && b.p1[2]==2 && b.p1[3]==3 && b.p1[4]==4 && b.p1[5]==5 && b.p2==p4);

	return 0;
}
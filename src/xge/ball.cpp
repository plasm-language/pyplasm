#include <xge/xge.h>
#include <xge/ball.h>
 
//////////////////////////////////////////////////////////////////////
int Ball3f::SelfTest()
{
	Log::printf("Testing Ball3f...\n");

	//add a point
	std::vector<float> points;
	

	points.push_back(-1);points.push_back(-1);points.push_back(-1);
	points.push_back(+1);points.push_back(-1);points.push_back(-1);
	points.push_back(+1);points.push_back(+1);points.push_back(-1);
	points.push_back(-1);points.push_back(+1);points.push_back(-1);
	points.push_back(-1);points.push_back(-1);points.push_back(+1);
	points.push_back(+1);points.push_back(-1);points.push_back(+1);
	points.push_back(+1);points.push_back(+1);points.push_back(+1);
	points.push_back(-1);points.push_back(+1);points.push_back(+1);

	Ball3f b=Ball3f::bestFittingBall(points);

	float radius=Vec3f(1,1,1).module();
	ReleaseAssert(b.center.fuzzyEqual(Vec3f(0,0,0)) && Utils::FuzzyEqual(b.radius,radius));

	//test volume and surface
	ReleaseAssert(Utils::FuzzyEqual(b.surface(),(4*(float)M_PI*radius*radius),0.0001f) && Utils::FuzzyEqual(b.volume(),4*(float)M_PI*radius*radius*radius/3,0.0001f));

	return 0;
}



 

//////////////////////////////////////////////////////////////////////
int Ballf::SelfTest()
{
	Log::printf("Testing Ballf...\n");


	//add a point
	std::vector<float> P;

	P.push_back(1.0f);P.push_back(-1.0f);P.push_back(-1.0f);P.push_back(-1.0f);
	P.push_back(1.0f);P.push_back(+1.0f);P.push_back(-1.0f);P.push_back(-1.0f);
	P.push_back(1.0f);P.push_back(+1.0f);P.push_back(+1.0f);P.push_back(-1.0f);
	P.push_back(1.0f);P.push_back(-1.0f);P.push_back(+1.0f);P.push_back(-1.0f);
	P.push_back(1.0f);P.push_back(-1.0f);P.push_back(-1.0f);P.push_back(+1.0f);
	P.push_back(1.0f);P.push_back(+1.0f);P.push_back(-1.0f);P.push_back(+1.0f);
	P.push_back(1.0f);P.push_back(+1.0f);P.push_back(+1.0f);P.push_back(+1.0f);
	P.push_back(1.0f);P.push_back(-1.0f);P.push_back(+1.0f);P.push_back(+1.0f);

	Ballf b=Ballf::bestFittingBall(3,P);
	ReleaseAssert(b.dim()==3 && b.center.fuzzyEqual(Vecf(1.0f, 0.0f,0.0f,0.0f)) && Utils::FuzzyEqual(b.radius,Vecf(0.0f, 1.0f,1.0f,1.0f).module()));

	return 0;
}
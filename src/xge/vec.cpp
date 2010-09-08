#include <xge/xge.h>
#include <xge/vec.h>



/////////////////////////////////////////////////////////////
Vec4f Vec4f::operator*(const Mat4f& m) const
{
	const Vec4f& v=(*this);
	const float* mmat=m.mat;

	return Vec4f(
		(mmat[ 0]*v.x+mmat[ 4]*v.y+mmat[ 8]*v.z+mmat[12]*v.w),
		(mmat[ 1]*v.x+mmat[ 5]*v.y+mmat[ 9]*v.z+mmat[13]*v.w),
		(mmat[ 2]*v.x+mmat[ 6]*v.y+mmat[10]*v.z+mmat[14]*v.w),
		(mmat[ 3]*v.x+mmat[ 7]*v.y+mmat[11]*v.z+mmat[15]*v.w)
	);
}



/////////////////////////////////////////////////////////////
Vecf Vecf::operator*(const Matf& m) const
{
	const Vecf& h=*this;

	DebugAssert(m.dim==h.dim);
	int dim=h.dim;
	Vecf ret(dim);
	for (int c=0;c<=dim;c++)
	for (int r=0;r<=dim;r++) 
		ret.mem[c]+=h.mem[r]*(float)m(r,c);
	return ret;
}
 



/////////////////////////////////////////////////////////////
int Vec3f::SelfTest()
{
	Log::printf("Testing Vec3f...\n");

	Vec3f v;

	ReleaseAssert(v==Vec3f(0,0,0));

	float _vf[]={10,11,12};
	v=Vec3f(_vf);
	ReleaseAssert(v==Vec3f(10,11,12));

	double _vd[]={100,110,120};
	v=Vec3f(_vd);
	ReleaseAssert(v==Vec3f(100,110,120));


	ReleaseAssert(v*0.1f==Vec3f(10,11,12) && 0.1f*v==Vec3f(10,11,12));
	ReleaseAssert(v/0.1f==Vec3f(1000,1100,1200) );


	ReleaseAssert(Vec3f(1,2,3).Min(Vec3f(0,5,6))==Vec3f(0,2,3));
	ReleaseAssert(Vec3f(1,2,3).Max(Vec3f(0,5,6))==Vec3f(1,5,6));

	float epsilon=0.0001f;
	ReleaseAssert(Vec3f(1+epsilon,2-epsilon,3+epsilon).fuzzyEqual(Vec3f(1,2,3)));

	ReleaseAssert(Vec3f(1,2,3)+Vec3f(1,2,3)==Vec3f(2,4,6));
	ReleaseAssert(Vec3f(1,2,3)-Vec3f(1,2,3)==Vec3f(0,0,0));

	ReleaseAssert(Vec3f(1,2,3).module2()==14);

	ReleaseAssert(Vec3f(1,1,1).normalize().fuzzyEqual(Vec3f(0.5773f,0.5773f,0.5773f)));

	ReleaseAssert(Vec3f(1,0,0).cross(Vec3f(0,1,0))==Vec3f(0,0,1));
	
	return 0;
}



/////////////////////////////////////////////////////////////
int Vec4f::SelfTest()
{
	Log::printf("Testing Vec4f...\n");

	Vec4f v;
	

	ReleaseAssert(v==Vec4f(0,0,0,0));

	v=Vec4f(1,2,3,4);
	ReleaseAssert(v==Vec4f(1,2,3,4));

	float _vf[]={10,11,12,13};
	v=Vec4f(_vf);
	ReleaseAssert(v==Vec4f(10,11,12,13));

	double _vd[]={100,110,120,130};
	v=Vec4f(_vd);
	ReleaseAssert(v==Vec4f(100,110,120,130));

	ReleaseAssert(Vec3f(v.x,v.y,v.z)==Vec3f(100,110,120));

	ReleaseAssert(v*0.1f==Vec4f(10,11,12,13) && 0.1f*v==Vec4f(10,11,12,13));
	ReleaseAssert(v/0.1f==Vec4f(1000,1100,1200,1300) );


	ReleaseAssert(Vec4f(1,2,3,4).Min(Vec4f(0,5,6,7))==Vec4f(0,2,3,4));
	ReleaseAssert(Vec4f(1,2,3,4).Max(Vec4f(0,5,6,7))==Vec4f(1,5,6,7));

	float epsilon=0.0001f;
	ReleaseAssert(Vec4f(1+epsilon,2-epsilon,3+epsilon,4-epsilon).fuzzyEqual(Vec4f(1,2,3,4)));

	ReleaseAssert(Vec4f(1,2,3,4)+Vec4f(1,2,3,4)==Vec4f(2,4,6,8));
	ReleaseAssert(Vec4f(1,2,3,4)-Vec4f(1,2,3,4)==Vec4f(0,0,0,0));

	ReleaseAssert(Vec4f(1,1,1,1).normalize().fuzzyEqual(Vec4f(0.5f,0.5f,0.5f,0.5f)));

	return 0;
}



/////////////////////////////////////////////////////////////
int Vecf::SelfTest()
{
	Log::printf("Testing Vecf...\n");

	Vecf v1(0);v1.mem[0]=0.0f;
	ReleaseAssert(v1.dim==0 && v1[0]==0 && v1==v1);

	Vecf v2(1);v2.mem[0]=0.0f;v2.mem[1]=1.0f;
	ReleaseAssert(v2.dim==1 && v2[0]==0.0f && v2[1]==1.0f && v2==v2);

	Vecf v3(0.0f,1.0f,2.0f);
	ReleaseAssert(v3.dim==2 && v3[0]==0.0f && v3[1]==1.0f && v3[2]==2.0f && v3==v3);

	Vecf v4(0.0f,1.0f,2.0f,3.0f);
	ReleaseAssert(v4.dim==3 && v4[0]==0.0f && v4[1]==1.0f && v4[2]==2.0f && v4[3]==3.0f && v4==v4);



	Vecf v=Vecf(1.0,2.0,3.0);
	{
		v*=0.1f;
		ReleaseAssert(v.fuzzyEqual(Vecf(0.1f,0.2f,0.3f)));
	}

	{
		v=Vecf(1.0,2.0,3.0);
		v/=0.1f;
		ReleaseAssert(v.fuzzyEqual(Vecf(10.0,20.0,30.0)));
	}

	//test + -
	{
		Vecf v1(1.0f,2.0f,3.0f);
		Vecf v2(0.1f,0.2f,0.3f);

		ReleaseAssert((v1+v2).fuzzyEqual(Vecf(1.1f,2.2f,3.3f)));
		ReleaseAssert((v1-v2).fuzzyEqual(Vecf(0.9f,1.8f,2.7f)));

		ReleaseAssert(fabs(v1*v2-(v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2]))<=0.001f);
	}

	return 0;
}

#include <xge/xge.h>
#include <xge/quaternion.h>



const float Quaternion::ms_fEpsilon = 1e-03f;
const Quaternion Quaternion::ZERO(0.0f,0.0f,0.0f,0.0f);
const Quaternion Quaternion::IDENTITY(1.0f,0.0f,0.0f,0.0f);


////////////////////////////////////////////////////////////////////////
void Quaternion::fromMatrix(Mat4f& m)
{
	float kRot[3][3]=
	 {
		m.a11(),m.a12(),m.a13(),
		m.a21(),m.a22(),m.a23(),
		m.a31(),m.a32(),m.a33()
	 };

	 // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	 // article "Quaternion Calculus and Fast Animation".
 
	 float fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
	 float fRoot;
 
	 if ( fTrace > 0.0 )
	 {
		 // |w| > 1/2, may as well choose w > 1/2
		 fRoot = (float)sqrt(fTrace + 1.0);  // 2w
		 w = 0.5f*fRoot;
		 fRoot = 0.5f/fRoot;  // 1/(4w)
		 x = (kRot[2][1]-kRot[1][2])*fRoot;
		 y = (kRot[0][2]-kRot[2][0])*fRoot;
		 z = (kRot[1][0]-kRot[0][1])*fRoot;
	 }
	 else
	 {
		 // |w| <= 1/2
		 static int s_iNext[3] = { 1, 2, 0 };
		 int i = 0;
		 if ( kRot[1][1] > kRot[0][0] )
			 i = 1;
		 if ( kRot[2][2] > kRot[i][i] )
			 i = 2;
		 int j = s_iNext[i];
		 int k = s_iNext[j];
 
		 fRoot = (float)sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0);
		 float* apkQuat[3] = { &x, &y, &z };
		 *apkQuat[i] = 0.5f * fRoot;
		 fRoot = (float)0.5f/fRoot;
		 w = (kRot[k][j]-kRot[j][k])*fRoot;
		 *apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
		 *apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
	 }
}


////////////////////////////////////////////////////////////////////////////
int Quaternion::SelfTest()
{
	Log::printf("Testing Quaternion...\n");

	Quaternion q;
	ReleaseAssert(q.getMatrix().almostIdentity(0.001f) && Utils::FuzzyEqual(q.getAngle(),0));

	q=Quaternion(Vec3f(1,1,1),(float)M_PI/4);
	ReleaseAssert(Utils::FuzzyEqual((q.getAxis()-Vec3f(1,1,1).normalize()).module(),0,0.0001f));
	ReleaseAssert(Utils::FuzzyEqual(q.getAngle(),(float)M_PI/4,0.0001f));

	//inverse
	ReleaseAssert((q*q.Inverse()).getMatrix().almostIdentity(0.001f));

	//multiply a vector
	Vec3f V=Vec3f(Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1)).normalize();
	ReleaseAssert(((q.getMatrix()* V)-(q * V)).module()<0.001f);

	Vec3f rotations[]=
	{
		Vec3f(-1,-1,-1),
		Vec3f(+1,-1,-1),
		Vec3f(-1,+1,-1),
		Vec3f(+1,+1,-1),
		Vec3f(-1,-1,+1),
		Vec3f(+1,-1,+1),
		Vec3f(-1,+1,+1),
		Vec3f(+1,+1,+1)
	};

	for (int i=0;i<8;i++)
	{
		float angle=(float)M_PI/4.0f;

		rotations[i]=rotations[i].normalize();

		Mat4f _rotmat=Mat4f::rotate(rotations[i],angle);
		Quaternion q(_rotmat);
		ReleaseAssert((q.getAxis()-rotations[i]).module()<0.001f && fabs(q.getAngle()-angle)<0.001f);

		Mat4f m1=Mat4f::rotate(rotations[i],angle);
		Mat4f m2=q.getMatrix();
		ReleaseAssert((m2 - m1).almostZero(0.001f));

		{
			q=Quaternion(rotations[i],angle);
			ReleaseAssert((q.getAxis()-rotations[i]).module()<0.001f && fabs(q.getAngle()-angle)<0.001f);
		}

		ReleaseAssert(((q.getMatrix()* V)-(q * V)).module()<0.001f);
	}

	q=Quaternion(Vec3f(1,0,0),Vec3f(0,1,0),Vec3f(0,0,1));
	Vec3f X,Y,Z;
	q.getAxis(X,Y,Z);
	ReleaseAssert( (X-Vec3f(1,0,0)).module()<0.001);
	ReleaseAssert( (Y-Vec3f(0,1,0)).module()<0.001);
	ReleaseAssert( (Z-Vec3f(0,0,1)).module()<0.001);
	ReleaseAssert(q.getMatrix().almostIdentity(0.001f));

	//test product
	Quaternion qx(Vec3f(1,0,0),(float)M_PI/4.0);
	Quaternion qy(Vec3f(0,1,0),(float)-M_PI/3.0);
	Quaternion qz(Vec3f(0,0,1),(float)M_PI/8.0);

	Mat4f M1=(qx*qy*qz).getMatrix();
	Mat4f M2=(Mat4f::rotatex((float)M_PI/4.0) * Mat4f::rotatey(-(float)M_PI/3.0) * Mat4f::rotatez((float)M_PI/8.0));
	ReleaseAssert((M1 - M2 ).almostZero(0.0001f));
	ReleaseAssert(((qx*qy*qz)*V - M2*V).module()<0.001f );


	return 0;
}